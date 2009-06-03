#ifndef LLHWL_H_
#define LLHWL_H_

/* Event definitions */
#define LLHWL_EVENT_COUNT           25
#define LLHWL_EVENT_SPI_READ        (0)
#define LLHWL_EVENT_SPI_WRITE       (1)
#define LLHWL_EVENT_SPI_RESET       (2)
#define LLHWL_EVENT_SPI_UPDATE      (3)
#define LLHWL_EVENT_I2C_READ        (4)
#define LLHWL_EVENT_I2C_WRITE       (5)
#define LLHWL_EVENT_I2S_READ        (6)
#define LLHWL_EVENT_I2S_WRITE       (7)
#define LLHWL_EVENT_MC_READ         (8)
#define LLHWL_EVENT_MC_WRITE        (9)
#define LLHWL_EVENT_CLKPWR_READ     (10)
#define LLHWL_EVENT_CLKPWR_WRITE    (11)
#define LLHWL_EVENT_DMA_READ        (12)
#define LLHWL_EVENT_DMA_WRITE       (13)
#define LLHWL_EVENT_TIMERS_READ     (14)
#define LLHWL_EVENT_TIMERS_WRITE    (15)
#define LLHWL_EVENT_UART_READ       (16)
#define LLHWL_EVENT_UART_WRITE      (17)
#define LLHWL_EVENT_GPCON_READ      (18)
#define LLHWL_EVENT_GPCON_WRITE     (19)
#define LLHWL_EVENT_GPDAT_READ      (20)
#define LLHWL_EVENT_GPDAT_WRITE     (21)
#define LLHWL_EVENT_GLOFIISH_CPLD_READ 22
#define LLHWL_EVENT_GLOFIISH_CPLD_WRITE 23

#define LLHWL_EVENT_SPI             LLHWL_EVENT_COUNT + 1
#define LLHWL_EVENT_DMA             LLHWL_EVENT_COUNT + 2
#define LLHWL_EVENT_GPIO            LLHWL_EVENT_COUNT + 3
#define LLHWL_EVENT_I2C             LLHWL_EVENT_COUNT + 4
#define LLHWL_EVENT_I2S             LLHWL_EVENT_COUNT + 5
#define LLHWL_EVENT_MC              LLHWL_EVENT_COUNT + 6
#define LLHWL_EVENT_CLKPWR          LLHWL_EVENT_COUNT + 7
#define LLHWL_EVENT_TIMERS          LLHWL_EVENT_COUNT + 8
#define LLHWL_EVENT_UART            LLHWL_EVENT_COUNT + 9

void llhwl_init(void);
void llhwl_release(void);
void llhwl_enable(int enable);


void llhwl_trigger_init(void);

void llhwl_watch_add(int event_type, void (*handler)(int event_type, void *state, void *data), void
                     *data);
void llhwl_watch_remove(int event_type);
void llhwl_watch_remove_all(void);

void llhwl_report_event_data_put(int event_type, void *data);
void* llhwl_report_event_data_push(int event_type);
int llhwl_report_event_data_count(int event_type);
void* llhwl_report_event_data_get(int event_type, int num);
void llhwl_report_event(int event_type, void *state);

void dump_register(void *opaque, void *cache, int bank, int read, int conf);
void cache_state(void *vstate, void *vcache);

#endif

