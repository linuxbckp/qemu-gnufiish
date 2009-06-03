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

#define MAX_STACK_DEPTH     5

struct watch_registrant {
    void *data;
    void (*handler)(int event_type, void *state, void *data);
    struct watch_registrant *next;
};

struct watch {
    int event_type;
    uint32_t enable;
    struct watch_registrant registrant;
    void *stack[MAX_STACK_DEPTH];
    uint32_t stack_pos;
};

struct watch watches[LLHWL_EVENT_COUNT];
int enable = 0;

static void registrant_remove(struct watch_registrant *registrant) 
{
    if(registrant->next) {
        registrant_remove(registrant->next);
        qemu_free(registrant->next);
    }
    registrant->handler = NULL;
    qemu_free(registrant->data);
}

void llhwl_watch_remove_all(void) 
{
    int n;
    
    /* disable all watches and remove all registrants*/
    for(n=0; n<LLHWL_EVENT_COUNT; n++) {
        watches[n].enable = 0;
        registrant_remove(&watches[n].registrant);
    }

}

void llhwl_watch_remove(int event_type) {
    watches[event_type].enable = 0;
    registrant_remove(&watches[event_type].registrant);
}

/* Find a slot for our watch handler */
static struct watch_registrant* registrant_new(struct watch_registrant *registrant) 
{
    if(registrant->next && registrant->handler)
        return registrant_new(registrant->next);
    else if(!registrant->next) {
        registrant->next = (struct watch_registrant*) qemu_mallocz(sizeof(struct watch_registrant));
    }
    
    registrant->next->next = NULL;
    return registrant->next;
}

void llhwl_watch_add(int event_type, void (*handler)(int event_type, void *state, void *data), void
                     *data) {
    int n;
    struct watch_registrant *registrant;

    switch(event_type) {
    case LLHWL_EVENT_SPI:
        llhwl_watch_add(LLHWL_EVENT_SPI_READ, handler, data);
        llhwl_watch_add(LLHWL_EVENT_SPI_WRITE, handler, data);
        return;
    case LLHWL_EVENT_DMA:
        llhwl_watch_add(LLHWL_EVENT_DMA_READ, handler, data);
        llhwl_watch_add(LLHWL_EVENT_DMA_WRITE, handler, data);
        return;
    case LLHWL_EVENT_GPIO:
        llhwl_watch_add(LLHWL_EVENT_GPCON_READ, handler, data);
        llhwl_watch_add(LLHWL_EVENT_GPCON_WRITE, handler, data);
        llhwl_watch_add(LLHWL_EVENT_GPDAT_READ, handler, data);
        llhwl_watch_add(LLHWL_EVENT_GPDAT_WRITE, handler, data);
        return;
    case LLHWL_EVENT_I2C:
        llhwl_watch_add(LLHWL_EVENT_I2C_READ, handler, data);
        llhwl_watch_add(LLHWL_EVENT_I2C_WRITE, handler, data);
        return;
    /* FIXME add missing compiste event types */
    default:
        break;
    }

    /* enable watch */
    watches[event_type].enable = 1;

    /* initialize stack */
    watches[event_type].stack_pos = 0;
    for(n=0; n<MAX_STACK_DEPTH; n++) {
        watches[event_type].stack[n] = NULL;
    }

    /* add handler callback */
    registrant = registrant_new(&watches[event_type].registrant);
    registrant->handler = handler;
    registrant->data = data;
}

void llhwl_init(void) 
{
    llhwl_trigger_init();
}

void llhwl_release(void)
{
    llhwl_watch_remove_all();
}

void llhwl_enable(int en) 
{
    enable = en ? 1 : 0;
}

void llhwl_report_event_data_put(int event_type, void *data)
{
    int pos;

    if(!enable) return;

    /* FIXME Currently we ignore the fact that we can't register more than
     * MAX_STACK_DEPTH attributes per event 
     */
    if(watches[event_type].stack_pos >= MAX_STACK_DEPTH) 
        return;
    
    pos = watches[event_type].stack_pos;
    watches[event_type].stack[pos] = data;
    watches[event_type].stack_pos++;
}

void* llhwl_report_event_data_push(int event_type)
{
    int pos = watches[event_type].stack_pos;

    if(!enable) return NULL;

    if(pos <= 0)
        return NULL;

    watches[event_type].stack_pos--;
    return watches[event_type].stack[pos];
}

int llhwl_report_event_data_count(int event_type)
{
    return watches[event_type].stack_pos;
}

void* llhwl_report_event_data_get(int event_type, int num) 
{
    if(num >= watches[event_type].stack_pos)
        return NULL;

    return watches[event_type].stack[num];
}

static void tell_registrant(struct watch_registrant *registrant, int event_type, void *state) 
{
    if(registrant->handler)
        registrant->handler(event_type, state, registrant->data);
    if(registrant->next)
        tell_registrant(registrant->next, event_type, state);
}

void llhwl_report_event(int event_type, void *state) 
{
    if(!enable) return;

    /* tell all registrants that the event has occured */
    tell_registrant(&watches[event_type].registrant, event_type, state);

    /* reset stack */
    watches[event_type].stack_pos = 0;
}

