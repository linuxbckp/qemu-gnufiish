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

static int glofiish_modem_write(struct CharDriverState *chr, const uint8_t *buf, int len)
{
    struct glofiish_modem_state *s = (struct glofiish_modem_state*) chr->opaque;

    /* FIXME handle incomming data */
    printf("%s\n", __FUNCTION__);

    return len;
}

static int glofiish_modem_ioctl(struct CharDriverState *chr, int cmd, void *arg)
{
    QEMUSerialSetParams *ssp;
    struct glofiish_modem_state *s = (struct glofiish_modem_state*) chr->opaque;

    switch(cmd) {
    case CHR_IOCTL_SERIAL_SET_PARAMS:
        ssp = (QEMUSerialSetParams*)arg;
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

static inline void glofiish_modem_fifo_wake(struct glofiish_modem_state *s)
{
    if(!s->enable || !s->out_len)
        return;

    if(s->chr.chr_can_read && s->chr.chr_can_read(s->chr.handler_opaque) &&
       s->chr.chr_read)
    {
        s->chr.chr_read(s->chr.handler_opaque,
                        s->outfifo + s->out_start++, 1);
        s->out_len--;
        s->out_start &= FIFO_LEN - 1;
    }

    if(s->out_len)
        qemu_mod_timer(s->out_tm, qemu_get_clock(vm_clock) + s->baud_delay);
}

static void glofiish_modem_out_tick(void *opaque) 
{
    glofiish_modem_fifo_wake((struct glofiish_modem_state*) opaque);
}

void glofiish_modem_reset(struct glofiish_modem_state *s)
{
    s->out_len = 0;
    s->baud_delay = ticks_per_sec;
}

void glofiish_modem_enable(struct CharDriverState *chr, int enable)
{
    struct glofiish_modem_state *s = (struct glofiish_modem_state*) chr->opaque;
    s->enable = enable;
    if(s->enable)
        glofiish_modem_reset(s);
}

struct CharDriverState* glofiish_modem_init()
{
    struct glofiish_modem_state *s = 
        (struct glofiish_modem_state*)qemu_mallocz(sizeof(struct glofiish_modem_state));
    s->chr.opaque = s;
    s->chr.chr_write = glofiish_modem_write;
    s->chr.chr_ioctl = glofiish_modem_ioctl;
    s->out_tm = qemu_new_timer(vm_clock, glofiish_modem_out_tick, s);

    return &s->chr;
}



