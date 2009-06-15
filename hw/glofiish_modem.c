/* (C) 2009 by Simon Busch <morphis@gravedo.de>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "qemu-common.h"
#include "qemu-timer.h"
#include "qemu-char.h"
#include "sysemu.h"
#include "llhwl.h"

#define MS_START            0
#define MS_OFFLINE          1
#define MS_ONLINE           2
#define MS_TRANSMIT_1       3
#define MS_TRANSMIT_2       4

#define MAX_STATE           4

#define MST_RST             0
#define MST_nRST            1
#define MST_PWRON           2
#define MST_nPWRON          3
#define MST_GPA11           4
#define MST_RECEIVE_DATA    5

struct glofiish_modem_state {
    int enable;
    CharDriverState chr;
    QEMUTimer *out_tm;
    int64_t baud_delay;
#define FIFO_LEN    4096
    int out_start;
    int out_len;
    char outfifo[FIFO_LEN];
};

int nbytes = 0;
int modem_state = MS_START;
int modem_uart = 0;

void glofiish_modem_send(struct glofiish_modem_state *s, const char *data, int size);

static void handle_state(struct glofiish_modem_state *s) 
{
    char buffer[200];
    int size;
    switch(modem_state)
    {
    case MS_TRANSMIT_1:
        /* send 200 bytes */
/*        size = 200;
        memset(&buffer, 0, size);
        glofiish_modem_send(s, &buffer, size); */
        break;
    case MS_TRANSMIT_2:
        modem_uart = 1;
        /* send 8 bytes */
        size = 8;
        snprintf(buffer, 8, "0a3dc49k");
        glofiish_modem_send(s, &buffer, size);
        break;
    default:
        break;
    }
}

static void change_state(struct glofiish_modem_state *s, int transition)
{
    /*
    printf("change_state) state = %i, transition = %i\n", modem_state, transition);
    */

    switch(modem_state) 
    {
    case MS_START:
        if(transition == MST_RST) {
            modem_state = MS_OFFLINE;
            printf("entering MS_OFFLINE\n");
        }
        break;
    case MS_OFFLINE:
        if(transition == MST_PWRON) {
            modem_state = MS_ONLINE;
            printf("entering MS_ONLINE\n");
        }
        break;
    case MS_ONLINE:
        if(transition == MST_nPWRON) { 
            modem_state = MS_TRANSMIT_1;
            handle_state(s);
            printf("entering MS_TRANSMIT_1\n");
        }
        break;
    case MS_TRANSMIT_1:
        if(transition == MST_RECEIVE_DATA) {
            modem_state = MS_TRANSMIT_2;
            printf("entering MS_TRANSMIT_2\n");
            handle_state(s);
        }
        break;
    default: 
        break;
    }
}

static inline void glofiish_modem_fifo_wake(struct glofiish_modem_state *s)
{
    printf("glofiish_modem_fifo_wake\n");
    
    if (!s->enable || !s->out_len)
        return;

    if (qemu_chr_can_read(&s->chr) && s->chr.chr_read) {
        s->chr.chr_read(s->chr.handler_opaque,
                        s->outfifo + s->out_start ++, 1);
        s->out_len --;
        s->out_start &= FIFO_LEN - 1;
    }

    if (s->out_len)
        qemu_mod_timer(s->out_tm, qemu_get_clock(vm_clock) + s->baud_delay);

}

void glofiish_modem_send(struct glofiish_modem_state *s, const char *data, int size)
{
 /*   int len, off;

    len = size;
    if (len + s->out_len > FIFO_LEN) {
        s->out_len = 0;
        return;
    }
    off = (s->out_start + s->out_len) & (FIFO_LEN - 1);
    if (off + len > FIFO_LEN) {
        memcpy(s->outfifo + off, data, FIFO_LEN - off);
        memcpy(s->outfifo, data + (FIFO_LEN - off), off + len - FIFO_LEN);
    } else
        memcpy(s->outfifo + off, data, len);
    s->out_len += len;
    glofiish_modem_fifo_wake(s);*/

    s->chr.chr_read(s->chr.handler_opaque, data, size);
}

const char answer8[] = {0xff, 0xef, 0x34, 0x3f, 0xa4, 0x50, 0x1e, 0x9b};

static int glofiish_modem_write(struct CharDriverState *chr, const uint8_t *buf, int len)
{
    struct glofiish_modem_state *s = (struct glofiish_modem_state*) chr->opaque;
    int n;

    printf("glofiish_modem_write: ");
    for(n=0; n<len; n++)
        printf("%02x", buf[n]);
    printf("\n");
    
    change_state(s, MST_RECEIVE_DATA);

    return len;
}

static int glofiish_modem_ioctl(struct CharDriverState *chr, int cmd, void *arg)
{
    QEMUSerialSetParams *ssp;
    struct glofiish_modem_state *s = (struct glofiish_modem_state*) chr->opaque;

    switch(cmd) {
    case CHR_IOCTL_SERIAL_SET_PARAMS:
        ssp = (QEMUSerialSetParams*)arg;
        printf("baudrate: %i\n", ssp->speed);
        s->baud_delay = ticks_per_sec / ssp->speed;
        break;
    case CHR_IOCTL_MODEM_HANDSHAKE:
        if(!s->enable)
            return -ENOTSUP;
        break;
    default:
        return -ENOTSUP;
    }

    return 0;
}

static void glofiish_modem_out_tick(void *opaque) 
{
    glofiish_modem_fifo_wake((struct glofiish_modem_state*) opaque);
}

void glofiish_modem_reset(struct glofiish_modem_state *s, int reset)
{
    if(reset)
        change_state(s, MST_RST);
    else
        change_state(s, MST_nRST);
}

void glofiish_modem_gpa11(struct glofiish_modem_state *s, int level)
{
    if(level)
        change_state(s, MST_GPA11);
}

void glofiish_modem_enable(struct CharDriverState *chr, int enable)
{
    struct glofiish_modem_state *s = (struct glofiish_modem_state*) chr->opaque;
    s->enable = enable;
    if(s->enable) {
        change_state(s, MST_PWRON);
        s->out_len = 0;
        s->baud_delay = ticks_per_sec;
    }
    else change_state(s, MST_nPWRON);
}

struct CharDriverState* glofiish_modem_init()
{
    struct glofiish_modem_state *s = 
        (struct glofiish_modem_state*)qemu_mallocz(sizeof(struct glofiish_modem_state));
    s->chr.opaque = s;
    s->chr.chr_write = glofiish_modem_write;
    s->chr.chr_ioctl = glofiish_modem_ioctl;
    s->out_tm = qemu_new_timer(vm_clock, glofiish_modem_out_tick, s);
    modem_state = MS_START;
    printf("glofiish_modem_init: state = %i\n", modem_state);

    return &s->chr;
}



