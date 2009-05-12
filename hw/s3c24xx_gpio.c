/*
 * Samsung S3C24xx series I/O ports.
 *
 * Copyright (c) 2007 OpenMoko, Inc.
 * Author: Andrzej Zaborowski <andrew@openedhand.com>
 * With:	Michel Pollet <buserror@gmail.com>
 *
 * This code is licenced under the GNU GPL v2.
 */

#include "s3c.h"
#include "hw.h"

/*
 * Stuff for dumping GPIO configuration
 */

static const char * funcs_a[] = {
	"ADDR0      ",
	"ADDR16     ",
	"ADDR17     ",
	"ADDR18     ",
	"ADDR19     ",
	"ADDR20     ",
	"ADDR21     ",
	"ADDR22     ",
	"ADDR23     ",
	"ADDR24     ",
	"ADDR25     ",
	"ADDR26     ",
	"nGCS[1]    ",
	"nGCS[2]    ",
	"nGCS[3]    ",
	"nGCS[4]    ",
	"nGCS[5]    ",
	"CLE        ",
	"ALE        ",
	"nFWE       ",
	"nFRE       ",
	"nRSTOUT    ",
	"nFCE       ",
	NULL,
	NULL
};


static const char * funcs_b2[] = {
	"TOUT0      ",
	"TOUT1      ",
	"TOUT2      ",
	"TOUT3      ",
	"TCLK[0]    ",
	"nXBACK     ",
	"nXBREQ     ",
	"nXDACK1    ",
	"nXDREQ1    ",
	"nXDACK0    ",
	"nXDREQ0    ",
};
static const char * funcs_b3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

static const char * funcs_c2[] = {
	"LEND       ",
	"VCLK       ",
	"VLINE      ",
	"VFRAME     ",
	"VM         ",
	"LCD_LPCOE  ",
	"LCD_LPCREV ",
	"LCD_LPCREVB",
	"VD[0]      ",
	"VD[1]      ",
	"VD[2]      ",
	"VD[3]      ",
	"VD[4]      ",
	"VD[5]      ",
	"VD[6]      ",
	"VD[7]      ",
};
static const char * funcs_c3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	"I2SSDI     ",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

static const char * funcs_d2[] = {
	"VD[8]      ",
	"VD[9]      ",
	"VD[10]     ",
	"VD[11]     ",
	"VD[12]     ",
	"VD[13]     ",
	"VD[14]     ",
	"VD[15]     ",
	"VD[16]     ",
	"VD[17]     ",
	"VD[18]     ",
	"VD[19]     ",
	"VD[20]     ",
	"VD[21]     ",
	"VD[22]     ",
	"VD[23]     ",
};
static const char * funcs_d3[] = {
	"nSPICS1    ",
	"SPICLK1    ",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"SPIMISO1   ",
	"SPIMOSI1   ",
	"SPICLK1    ",
	NULL,
	NULL,
	NULL,
	"nSS1       ",
	"nSS0       ",
};

static const char * funcs_e2[] = {
	"I2SLRCK    ",
	"I2SSCLK    ",
	"CDCLK      ",
	"I2SDI      ",
	"I2SDO      ",
	"SDCLK      ",
	"SDCMD      ",
	"SDDAT0     ",
	"SDDAT1     ",
	"SDDAT2     ",
	"SDDAT3     ",
	"SPIMISO0   ",
	"SPIMOSI0   ",
	"SPICLK0    ",
	"IICSCL     ",
	"IICSDA     ",
};
static const char * funcs_e3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

static const char * funcs_f2[] = {
	"EINT[0]    ",
	"EINT[1]    ",
	"EINT[2]    ",
	"EINT[3]    ",
	"EINT[4]    ",
	"EINT[5]    ",
	"EINT[6]    ",
	"EINT[7]    ",
};
static const char * funcs_f3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};


static const char * funcs_g2[] = {
	"EINT[8]    ",
	"EINT[9]    ",
	"EINT[10]   ",
	"EINT[11]   ",
	"EINT[12]   ",
	"EINT[13]   ",
	"EINT[14]   ",
	"EINT[15]   ",
	"EINT[16]   ",
	"EINT[17]   ",
	"EINT[18]   ",
	"EINT[19]   ",
	"EINT[20]   ",
	"EINT[21]   ",
	"EINT[22]   ",
	"EINT[23]   ",
};
static const char * funcs_g3[] = {
	NULL,
	NULL,
	"nSS0       ",
	"nSS1       ",
	"LCD_PWRDN  ",
	"SPIMISO1   ",
	"SPIMOSI1   ",
	"SPICLK1    ",
	NULL,
	"nRTS1      ",
	"nCTS1      ",
	"TCLK[1]    ",
	"nSPICS0    ",
	NULL,
	NULL,
	NULL,
};

static const char * funcs_h2[] = {
	"nCTS0      ",
	"nRTS0      ",
	"TXD[0]     ",
	"RXD[0]     ",
	"TXD[1]     ",
	"RXD[1]     ",
	"TXD[2]     ",
	"RXD[2]     ",
	"UEXTCLK    ",
	"CLKOUT0    ",
	"CLKOUT1    ",
};
static const char * funcs_h3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"nRTS1      ",
	"nCTS1      ",
	NULL,
	"nSPICS0    ",
	NULL,
};

static const char * funcs_j2[] = {
	"CAMDATA[0] ",
	"CAMDATA[1] ",
	"CAMDATA[2] ",
	"CAMDATA[3] ",
	"CAMDATA[4] ",
	"CAMDATA[5] ",
	"CAMDATA[6] ",
	"CAMDATA[7] ",
	"CAMPCLK    ",
	"CAMVSYNC   ",
	"CAMHREF    ",
	"CAMCLKOUT  ",
	"CAMRESET   ",
};
static const char * funcs_j3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

struct reg {
    const char postfix;
    const char **func_names2;
    const char **func_names3;
    int num;
};

static struct reg reg_cfg[] = {
    {
        .postfix = 'A',
        .num = 25,
        .func_names2 = funcs_a,
        .func_names3 = NULL,
    },
    {
        .postfix = 'B',
        .num = 11,
        .func_names2 = funcs_b2,
        .func_names3 = funcs_b3,
    },
    {
        .postfix = 'C',
        .num = 16,
        .func_names2 = funcs_c2,
        .func_names3 = funcs_c3,
    },
    {
        .postfix = 'D',
        .num = 16,
        .func_names2 = funcs_d2,
        .func_names3 = funcs_d3,
    },
    {
        .postfix = 'E',
        .num = 16,
        .func_names2 = funcs_e2,
        .func_names3 = funcs_e3,
    },
    {
        .postfix = 'F',
        .num = 8,
        .func_names2 = funcs_f2,
        .func_names3 = funcs_f3,
    },
    {
        .postfix = 'G',
        .num = 16,
        .func_names2 = funcs_g2,
        .func_names3 = funcs_g3,
    },
    {
        .postfix = 'H',
        .num = 11,
        .func_names2 = funcs_h2,
        .func_names3 = funcs_h3,
    },
    {
        .postfix = 'J',
        .num = 13,
        .func_names2 = funcs_j2,
        .func_names3 = funcs_j3,
    },
};

static struct reg* get_reg_cfg(int regnum)
{
    int n;
    for(n = 0; n < 10; n++) {
        if(reg_cfg[n].postfix == regnum) {
            return &reg_cfg[n];
        }
    }
    return NULL;
}

static void pretty_dump(uint32_t cfg, uint32_t state, uint32_t pull,
			const char ** function_names_2,
			const char ** function_names_3,
			const char * prefix,
			int count)
{
	int n;
	const char *tag_type = NULL,
		   *tag_state = NULL,
		   *tag_pulldown = NULL,
		   * level0 = "0",
		   * level1 = "1";

	for (n = 0; n < count; n++) {
		switch ((cfg >> (2 * n)) & 3) {
		case 0:
			tag_type = "input      ";
			break;
		case 1:
			tag_type = "OUTPUT     ";
			break;
		case 2:
			if (function_names_2) {
				if (function_names_2[n])
					tag_type = function_names_2[n];
				else
					tag_type = "*** ILLEGAL CFG (2) *** ";
			} else
				tag_type = "(function) ";
			break;
		default:
			if (function_names_3) {
				if (function_names_3[n])
					tag_type = function_names_3[n];
				else
					tag_type = "*** ILLEGAL CFG (3) *** ";
			} else
				tag_type = "(function) ";
			break;
		}
		if ((state >> n) & 1)
			tag_state = level1;
		else
			tag_state = level0;

        
		if (((pull >> n) & 1))
			tag_pulldown = "";
		else
			tag_pulldown = "(pulldown)";
       
		printf("GP%s%02d: %s %s \n", prefix, n, tag_type,
						      tag_state);
	}
	printf("\n");
}

static void pretty_dump_a(uint32_t cfg, uint32_t state,
			  const char ** function_names,
			  const char * prefix,
			  int count)
{
	int n;
	const char *tag_type = NULL,
		   *tag_state = NULL,
		   * level0 = "0",
		   * level1 = "1";

	for (n = 0; n < count; n++) {
		switch ((cfg >> n) & 1) {
		case 0:
			tag_type = "OUTPUT     ";
			break;
		default:
			if (function_names) {
				if (function_names[n])
					tag_type = function_names[n];
				else
					tag_type = "*** ILLEGAL CFG *** ";
			} else
				tag_type = "(function) ";
			break;
		}
		if ((state >> n) & 1)
			tag_state = level1;
		else
			tag_state = level0;

		printf("GP%s%02d: %s %s\n", prefix, n, tag_type,
						   tag_state);
	}
	printf("\n");
}

/* S3C2410 : A B C D E F G H  = 8
 * S3C2440 : J = 1 */
#define S3C_IO_BANKS	8

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

static void dump_register(void *opaque, int bank, int read, int conf)
{
    struct reg *r = NULL;
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *) opaque;
    const char *port = "port",
         *configuration = "con",
         *destination = NULL;
    
    if(conf)
        destination = configuration;
    else
        destination = port;

    if(read)
        printf("read %s:\n", destination);
    else 
        printf("write %s:\n", destination);
    printf("================================\n");

    r = get_reg_cfg('A' + bank);
    if(!r)
        printf("no configuration for register\n");
    if(r && bank > 0)
        pretty_dump(s->bank[bank].con,
                    s->bank[bank].dat, 
                    s->bank[bank].up, r->func_names2, 
                    r->func_names3, &r->postfix, r->num);
    else if(r && bank == 0)
        pretty_dump_a(s->bank[bank].con,
                      s->bank[bank].dat,
                      r->func_names2, &r->postfix, r->num);
}

static inline void s3c_gpio_extint(struct s3c_gpio_state_s *s, int irq)
{
    if (s->eintmask & (1 << irq))
        return;
    s->eintpend |= (1 << irq) & 0x00fffff0;
    switch (irq) {
    case 0 ... 3:
        qemu_irq_raise(s->pic[S3C_PIC_EINT0 + irq]);
        break;
    case 4 ... 7:
        qemu_irq_raise(s->pic[S3C_PIC_EINT4]);
        break;
    case 8 ... 23:
        qemu_irq_raise(s->pic[S3C_PIC_EINT8]);
        break;
    }
}

static void s3c_gpio_set(void *opaque, int line, int level)
{
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *) opaque;
    int e, eint, bank = line >> 5;
    line &= 0x1f;
    /* Input ports */
    if (bank > 0 && ((s->bank[bank].con >> (2 * line)) & 3) == 0) {
        if (level)
            s->bank[bank].dat |= 1 << line;
        else
            s->bank[bank].dat &= ~(1 << line);
        return;
    }
    /* External interrupts */
    if (((s->bank[bank].con >> (2 * line)) & 3) == 2) {
        switch (bank) {
        case 5:	/* GPF */
            eint = line;
            e = 0;
            break;
        case 6:	/* GPG */
            eint = line + 8;
            e = (line > 15) ? 2 : 1;
            break;
        default:
            return;
        }
        if (level) {
            if (!((s->bank[bank].dat >> line) & 1))
                switch ((s->extint[e] >> (line * 4)) & 7) {
                case 1:
                case 4 ... 7:
                    s3c_gpio_extint(s, eint);
                    break;
                }
            s->bank[bank].dat |= 1 << line;
        } else {
            if ((s->bank[bank].dat >> line) & 1)
                switch ((s->extint[e] >> (line * 4)) & 7) {
                case 1:
                case 4 ... 5:
                    break;
                default:
                    s3c_gpio_extint(s, eint);
                }
            s->bank[bank].dat &= ~(1 << line);
        }
        return;
    }
}

qemu_irq *s3c_gpio_in_get(struct s3c_gpio_state_s *s)
{
    return s->in;
}

void s3c_gpio_out_set(struct s3c_gpio_state_s *s, int line, qemu_irq handler)
{
    int bank = line >> 5;
    line &= 0x1f;
    if (bank >= S3C_IO_BANKS || line >= s->bank[bank].n)
        cpu_abort(cpu_single_env, "%s: No I/O port %i\n", __FUNCTION__, line);
    s->bank[bank].handler[line] = handler;
}

void s3c_gpio_set_dat(struct s3c_gpio_state_s *s, int gpio, int level)
{
	int bank = gpio >> 5, bit = gpio & 0x1f;
	s->bank[bank].dat = (s->bank[bank].dat & ~(1 << bit)) | (level ? (1 << bit) : 0);
}

void s3c_gpio_reset(struct s3c_gpio_state_s *s)
{
    int i;
    s->inform[0] = 0;
    s->inform[1] = 0;
    s->misccr = 0x00010330 & ~(1 << 16);
    s->dclkcon = 0x00000000;
    s->extint[0] = 0;
    s->extint[1] = 0;
    s->extint[2] = 0;
    s->eintflt[0] = 0;
    s->eintflt[1] = 0;
    s->eintmask = 0x00fffff0;
    s->eintpend = 0x00000000;

    for (i = 0; i < S3C_IO_BANKS; i ++) {
        s->bank[i].mask = (1 << s->bank[i].n) - 1;
        s->bank[i].con = 0;
        s->bank[i].dat = 0;
        s->bank[i].up = 0;
    }
    s->bank[0].con = 0x07ffffff;
    s->bank[3].up = 0x0000f000;
    s->bank[6].up = 0x0000f800;
}

void s3c_gpio_setpwrstat(struct s3c_gpio_state_s *s, int stat)
{
    s->pwrstat = stat;
}

#define S3C_GPCON	0x00	/* Configuration register */
#define S3C_GPDAT	0x04	/* Data register */
#define S3C_GPUP	0x08	/* Pull-up register */
#define S3C_MISCCR	0x80	/* Miscellaneous Control register */
#define S3C_DCLKCON	0x84	/* DCLK0/1 Control register */
#define S3C_EXTINT0	0x88	/* External Interrupt Control register 0 */
#define S3C_EXTINT1	0x8c	/* External Interrupt Control register 1 */
#define S3C_EXTINT2	0x90	/* External Interrupt Control register 2 */
#define S3C_EINTFLT0	0x94	/* External Interrupt Filter register 0 */
#define S3C_EINTFLT1	0x98	/* External Interrupt Filter register 1 */
#define S3C_EINTFLT2	0x9c	/* External Interrupt Filter register 2 */
#define S3C_EINTFLT3	0xa0	/* External Interrupt Filter register 3 */
#define S3C_EINTMASK	0xa4	/* External Interrupt Mask register */
#define S3C_EINTPEND	0xa8	/* External Interrupt Pending register */
#define S3C_GSTATUS0	0xac	/* External Pin Status register */
#define S3C_GSTATUS1	0xb0	/* Chip ID register */
#define S3C_GSTATUS2	0xb4	/* Reset Status register */
#define S3C_GSTATUS3	0xb8	/* Inform register */
#define S3C_GSTATUS4	0xbc	/* Inform register */

static uint32_t s3c_gpio_read(void *opaque, target_phys_addr_t addr)
{
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *) opaque;
    int bank = 0;
    if ((addr >> 4) < S3C_IO_BANKS) {
        bank = addr >> 4;
        addr &= 0xf;
    }

    switch (addr) {
    case S3C_GSTATUS0:
        return 0x0;
    case S3C_GSTATUS1:
        return s->cpu_id;
    case S3C_GSTATUS2:
        return s->pwrstat;
    case S3C_GSTATUS3:
        return s->inform[0];
    case S3C_GSTATUS4:
        return s->inform[1];
    case S3C_MISCCR:
        return s->misccr;
    case S3C_DCLKCON:
        return s->dclkcon;
    case S3C_EXTINT0:
        return s->extint[0];
    case S3C_EXTINT1:
        return s->extint[1];
    case S3C_EXTINT2:
        return s->extint[2];
    case S3C_EINTFLT2:
        return s->eintflt[0];
    case S3C_EINTFLT3:
        return s->eintflt[1];
    case S3C_EINTMASK:
        return s->eintmask;
    case S3C_EINTPEND:
        return s->eintpend;
    /* Per bank registers */
    case S3C_GPCON:
        //printf("%s: read con '%c' (%i) = %08x\n", __FUNCTION__, 'A' + bank, bank, s->bank[bank].con);
        dump_register(s, bank, 1, 1);
        return s->bank[bank].con;
    case S3C_GPDAT:
        dump_register(s, bank, 1, 0);
        //printf("%s: read port '%c' (%i) = %08x\n", __FUNCTION__, 'A' + bank, bank, s->bank[bank].dat); 
        return s->bank[bank].dat;
    case S3C_GPUP:
        return s->bank[bank].up;
    default:
        printf("%s: Bad register 0x%lx\n", __FUNCTION__, (unsigned long)addr);
        break;
    }
    return 0;
}

static void s3c_gpio_write(void *opaque, target_phys_addr_t addr,
                uint32_t value)
{
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *) opaque;
    uint32_t diff;
    int ln, bank = 0;
    if ((addr >> 4) < S3C_IO_BANKS) {
        bank = addr >> 4;
        addr &= 0xf;
    }

    switch (addr) {
    case S3C_GSTATUS2:
        s->pwrstat &= 7 & ~value;
        break;
    case S3C_GSTATUS3:
        s->inform[0] = value;
        break;
    case S3C_GSTATUS4:
        s->inform[1] = value;
        break;
    case S3C_MISCCR:
        if (value & (1 << 16))				/* nRSTCON */
            printf("%s: software reset.\n", __FUNCTION__);
        if ((value ^ s->misccr) & (1 << 3))		/* USBPAD */
            printf("%s: now in USB %s mode.\n", __FUNCTION__,
                            (value >> 3) & 1 ? "host" : "slave");
        s->misccr = value & 0x000f377b;
        break;
    case S3C_DCLKCON:
        s->dclkcon = value & 0x0ff30ff3;
        break;
    case S3C_EXTINT0:
        s->extint[0] = value;
        break;
    case S3C_EXTINT1:
        s->extint[1] = value;
        break;
    case S3C_EXTINT2:
        s->extint[2] = value;
        break;
    case S3C_EINTFLT2:
        s->eintflt[0] = value;
        break;
    case S3C_EINTFLT3:
        s->eintflt[1] = value;
        break;
    case S3C_EINTMASK:
        s->eintmask = value & 0x00fffff0;
        break;
    case S3C_EINTPEND:
        s->eintpend &= ~value;
        break;
    /* Per bank registers */
    case S3C_GPCON:
        //printf("%s: write con '%c' (%i) = %08x\n", __FUNCTION__, 'A' + bank, bank, value);
        //printf("%s: GP%cDAT: %08x\n", __FUNCTION__,'A' + bank, s->bank[bank].dat);
        s->bank[bank].con = value;
        dump_register(s, bank, 0, 1);
        break;
    case S3C_GPDAT:
        diff = (s->bank[bank].dat ^ value) & s->bank[bank].mask;
        s->bank[bank].dat = value;
        dump_register(s, bank, 0, 0);
        //printf("%s: write port '%c' (%i) = %08x\n", __FUNCTION__, 'A' + bank, bank, s->bank[bank].dat);
        while ((ln = ffs(diff))) {
            ln --;
            if (s->bank[bank].handler[ln]) {
                if (bank && ((s->bank[bank].con >> (2 * ln)) & 3) == 1)
                    qemu_set_irq(s->bank[bank].handler[ln], (value >> ln) & 1);
                else if (!bank && ((s->bank[bank].con >> ln) & 1) == 0)
                    qemu_set_irq(s->bank[bank].handler[ln], (value >> ln) & 1);
            }
            diff &= ~(1 << ln);
        }
        break;
    case S3C_GPUP:
        s->bank[bank].up = value;
        break;
    default:
        printf("%s: Bad register 0x%lx\n", __FUNCTION__, (unsigned long)addr);
    }
}

static CPUReadMemoryFunc *s3c_gpio_readfn[] = {
    s3c_gpio_read,
    s3c_gpio_read,
    s3c_gpio_read,
};

static CPUWriteMemoryFunc *s3c_gpio_writefn[] = {
    s3c_gpio_write,
    s3c_gpio_write,
    s3c_gpio_write,
};

static void s3c_gpio_save(QEMUFile *f, void *opaque)
{
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *) opaque;
    int i;
    for (i = 0; i < S3C_IO_BANKS; i ++) {
        qemu_put_be32s(f, &s->bank[i].con);
        qemu_put_be32s(f, &s->bank[i].dat);
        qemu_put_be32s(f, &s->bank[i].up);
        qemu_put_be32s(f, &s->bank[i].mask);
    }

    qemu_put_be32s(f, &s->inform[0]);
    qemu_put_be32s(f, &s->inform[1]);
    qemu_put_be32s(f, &s->pwrstat);
    qemu_put_be32s(f, &s->misccr);
    qemu_put_be32s(f, &s->dclkcon);
    qemu_put_be32s(f, &s->extint[0]);
    qemu_put_be32s(f, &s->extint[1]);
    qemu_put_be32s(f, &s->extint[2]);
    qemu_put_be32s(f, &s->eintflt[0]);
    qemu_put_be32s(f, &s->eintflt[1]);
    qemu_put_be32s(f, &s->eintmask);
    qemu_put_be32s(f, &s->eintpend);
}

static int s3c_gpio_load(QEMUFile *f, void *opaque, int version_id)
{
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *) opaque;
    int i;
    for (i = 0; i < S3C_IO_BANKS; i ++) {
        qemu_get_be32s(f, &s->bank[i].con);
        qemu_get_be32s(f, &s->bank[i].dat);
        qemu_get_be32s(f, &s->bank[i].up);
        qemu_get_be32s(f, &s->bank[i].mask);
    }

    qemu_get_be32s(f, &s->inform[0]);
    qemu_get_be32s(f, &s->inform[1]);
    qemu_get_be32s(f, &s->pwrstat);
    qemu_get_be32s(f, &s->misccr);
    qemu_get_be32s(f, &s->dclkcon);
    qemu_get_be32s(f, &s->extint[0]);
    qemu_get_be32s(f, &s->extint[1]);
    qemu_get_be32s(f, &s->extint[2]);
    qemu_get_be32s(f, &s->eintflt[0]);
    qemu_get_be32s(f, &s->eintflt[1]);
    qemu_get_be32s(f, &s->eintmask);
    qemu_get_be32s(f, &s->eintpend);

    return 0;
}

struct s3c_gpio_state_s *s3c_gpio_init(target_phys_addr_t base, qemu_irq *pic, uint32_t cpu_id)
{
    int iomemtype;
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *)
            qemu_mallocz(sizeof(struct s3c_gpio_state_s));
    
    s->cpu_id = cpu_id;
    s->base = base;
    s->pic = pic;
    s->in = qemu_allocate_irqs(s3c_gpio_set, s, S3C_GP_MAX);

    s->bank[0].n = 23;
    s->bank[1].n = 11;
    s->bank[2].n = 16;
    s->bank[3].n = 16;
    s->bank[4].n = 16;
    s->bank[5].n = 8;
    s->bank[6].n = 16;
    s->bank[7].n = 11;

    s3c_gpio_reset(s);

    iomemtype = cpu_register_io_memory(0, s3c_gpio_readfn,
                    s3c_gpio_writefn, s);
    cpu_register_physical_memory(s->base, 0xffffff, iomemtype);

    register_savevm("s3c24xx_io", 0, 0, s3c_gpio_save, s3c_gpio_load, s);

    return s;
}
