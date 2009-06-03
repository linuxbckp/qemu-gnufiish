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

#include "s3c.h"
#include "llhwl.h"

#define S3C_IO_BANKS 9

struct s3c_gpio_state_s {	/* Modelled as an interrupt controller */
	uint32_t cpu_id;
    target_phys_addr_t base;
    qemu_irq *pic;
    qemu_irq *in;

    struct {
        int n;
        uint32_t con;
        uint32_t dat;
        uint32_t up;
        uint32_t mask;
        qemu_irq handler[32];
    } bank[S3C_IO_BANKS];

    uint32_t inform[2];
    uint32_t pwrstat;
    uint32_t misccr;
    uint32_t dclkcon;
    uint32_t extint[3];
    uint32_t eintflt[2];
    uint32_t eintmask;
    uint32_t eintpend;
};


struct trigger_gpio_data {
    struct s3c_gpio_state_s state_cache;
    int initial_cache;
};

static void trigger_gpio_handle(int event_type, void *state, void *data)
{
    struct trigger_gpio_data *tgd = (struct trigger_gpio_data*) data;
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s*) state;
    int bank;
    uint32_t value;

    switch(event_type) {
    case LLHWL_EVENT_GPCON_READ:
/*        bank = *((int*)llhwl_report_event_data_get(event_type, 0));
        dump_register(s, &tgd->state_cache, bank, 1, 1);*/
        break;
    case LLHWL_EVENT_GPCON_WRITE:
        bank = *((int*)llhwl_report_event_data_get(event_type, 1));
        value = *((uint32_t*)llhwl_report_event_data_get(event_type, 0));
        dump_register(s, &tgd->state_cache, bank, 0, 1);
        break;
    case LLHWL_EVENT_GPDAT_READ:
        /*printf("gpdat read\n");
        bank = *((int*)llhwl_report_event_data_get(event_type, 0));
        dump_register(s, &tgd->state_cache, bank, 1, 0);*/
        break;
    case LLHWL_EVENT_GPDAT_WRITE:
        printf("gpdat write\n");
        bank = *((int*)llhwl_report_event_data_get(event_type, 1));
        value = *((uint32_t*)llhwl_report_event_data_get(event_type, 0));
        dump_register(s, &tgd->state_cache, bank, 0, 0);
        break;
    default:
        break;
    }

    cache_state(s, &tgd->state_cache);
}

void llhwl_trigger_init(void) 
{
    /* Register all trigger functions */

    /* GPIO trigger */
    struct trigger_gpio_data *tgd = 
        (struct trigger_gpio_data*) qemu_mallocz(sizeof(struct trigger_gpio_data));
    s3c_gpio_reset(&tgd->state_cache);
    llhwl_watch_add(LLHWL_EVENT_GPIO, trigger_gpio_handle, tgd);
}

