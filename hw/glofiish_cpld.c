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
#include "qemu-timer.h"
#include "qemu-char.h"
#include "console.h"
#include "devices.h"
#include "arm-misc.h"
#include "i2c.h"
#include "pxa.h"
#include "sysemu.h"
#include "llhwl.h"

struct glofiish_cpld_state { 
    target_phys_addr_t base;
};

static void glofiish_cpld_write(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    struct glofiish_cpld_state *s = (struct glofiish_cpld_state*) opaque;

#ifdef CONFIG_LLHWL
    llhwl_report_event_data_put(LLHWL_EVENT_GLOFIISH_CPLD_WRITE, &value);
    llhwl_report_event_data_put(LLHWL_EVENT_GLOFIISH_CPLD_WRITE, &addr);
    llhwl_report_event(LLHWL_EVENT_GLOFIISH_CPLD_WRITE, s);
#endif
}

static uint32_t glofiish_cpld_read(void *opaque, target_phys_addr_t addr) 
{
    struct glofiish_cpld_state *s = (struct glofiish_cpld_state*) opaque;

#ifdef CONFIG_LLHWL
    llhwl_report_event_data_put(LLHWL_EVENT_GLOFIISH_CPLD_READ, &addr);
    llhwl_report_event(LLHWL_EVENT_GLOFIISH_CPLD_READ, s);
#endif
}

void glofiish_cpld_reset(struct glofiish_cpld_state *cpld)
{
}

static CPUReadMemoryFunc *glofiish_cpld_readfn[] = {
    glofiish_cpld_read,
    glofiish_cpld_read,
    glofiish_cpld_read
};

static CPUWriteMemoryFunc *glofiish_cpld_writefn[] = {
    glofiish_cpld_write,
    glofiish_cpld_write,
    glofiish_cpld_write
};

static void glofiish_cpld_save(QEMUFile *f, void *opaque)
{
}

static int glofiish_cpld_load(QEMUFile *f, void *opaque, int version_id)
{
    return 0;
}

struct glofiish_cpld* glofiish_cpld_init(target_phys_addr_t base) 
{
    int iomemtype;
    struct glofiish_cpld_state *s = 
        (struct glofiish_cpld_state*) qemu_mallocz(sizeof(struct glofiish_cpld_state));
    
    s->base = base;

    glofiish_cpld_reset(s);

    iomemtype = cpu_register_io_memory(0, glofiish_cpld_readfn, glofiish_cpld_writefn, s);
    cpu_register_physical_memory(s->base, 0x20, iomemtype); 

    register_savevm("glofiish_cpld", 0, 0, glofiish_cpld_save, glofiish_cpld_load, s);

    return s;
}




