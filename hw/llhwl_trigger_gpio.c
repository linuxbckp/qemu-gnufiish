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

#define S3C_IO_BANKS  9

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

/*
 * Stuff for dumping GPIO configuration
 */

static const char * funcs_a[] = {
	"ADDR0",
	"ADDR16",
	"ADDR17",
	"ADDR18",
	"ADDR19",
	"ADDR20",
	"ADDR21",
	"ADDR22",
	"ADDR23",
	"ADDR24",
	"ADDR25",
	"ADDR26",
	"nGCS[1]",
	"nGCS[2]",
	"nGCS[3]",
	"nGCS[4]",
	"nGCS[5]",
	"CLE",
	"ALE",
	"nFWE",
	"nFRE",
	"nRSTOUT",
	"nFCE",
	NULL,
	NULL
};


static const char * funcs_b2[] = {
	"TOUT0",
	"TOUT1",
	"TOUT2",
	"TOUT3",
	"TCLK[0]",
	"nXBACK",
	"nXBREQ",
	"nXDACK1",
	"nXDREQ1",
	"nXDACK0",
	"nXDREQ0",
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
	"LEND",
	"VCLK",
	"VLINE",
	"VFRAME",
	"VM",
	"LCD_LPCOE",
	"LCD_LPCREV",
	"LCD_LPCREVB",
	"VD[0]",
	"VD[1]",
	"VD[2]",
	"VD[3]",
	"VD[4]",
	"VD[5]",
	"VD[6]",
	"VD[7]",
};
static const char * funcs_c3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	"I2SSDI",
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
	"VD[8]",
	"VD[9]",
	"VD[10]",
	"VD[11]",
	"VD[12]",
	"VD[13]",
	"VD[14]",
	"VD[15]",
	"VD[16]",
	"VD[17]",
	"VD[18]",
	"VD[19]",
	"VD[20]",
	"VD[21]",
	"VD[22]",
	"VD[23]",
};
static const char * funcs_d3[] = {
	"nSPICS1",
	"SPICLK1",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"SPIMISO1",
	"SPIMOSI1",
	"SPICLK1",
	NULL,
	NULL,
	NULL,
	"nSS1",
	"nSS0",
};

static const char * funcs_e2[] = {
	"I2SLRCK",
	"I2SSCLK",
	"CDCLK",
	"I2SDI",
	"I2SDO",
	"SDCLK",
	"SDCMD",
	"SDDAT0",
	"SDDAT1",
	"SDDAT2",
	"SDDAT3",
	"SPIMISO0",
	"SPIMOSI0",
	"SPICLK0",
	"IICSCL",
	"IICSDA",
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
	"EINT[0]",
	"EINT[1]",
	"EINT[2]",
	"EINT[3]",
	"EINT[4]",
	"EINT[5]",
	"EINT[6]",
	"EINT[7]",
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
	"EINT[8]",
	"EINT[9]",
	"EINT[10]",
	"EINT[11]",
	"EINT[12]",
	"EINT[13]",
	"EINT[14]",
	"EINT[15]",
	"EINT[16]",
	"EINT[17]",
	"EINT[18]",
	"EINT[19]",
	"EINT[20]",
	"EINT[21]",
	"EINT[22]",
	"EINT[23]",
};
static const char * funcs_g3[] = {
	NULL,
	NULL,
	"nSS0",
	"nSS1",
	"LCD_PWRDN",
	"SPIMISO1",
	"SPIMOSI1",
	"SPICLK1",
	NULL,
	"nRTS1",
	"nCTS1",
	"TCLK[1]",
	"nSPICS0",
	NULL,
	NULL,
	NULL,
};

static const char * funcs_h2[] = {
	"nCTS0",
	"nRTS0",
	"TXD[0]",
	"RXD[0]",
	"TXD[1]",
	"RXD[1]",
	"TXD[2]",
	"RXD[2]",
	"UEXTCLK",
	"CLKOUT0",
	"CLKOUT1",
};
static const char * funcs_h3[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"nRTS1",
	"nCTS1",
	NULL,
	"nSPICS0",
	NULL,
};

static const char * funcs_j2[] = {
	"CAMDATA[0]",
	"CAMDATA[1]",
	"CAMDATA[2]",
	"CAMDATA[3]",
	"CAMDATA[4]",
	"CAMDATA[5]",
	"CAMDATA[6]",
	"CAMDATA[7]",
	"CAMPCLK",
	"CAMVSYNC",
	"CAMHREF",
	"CAMCLKOUT",
	"CAMRESET",
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
        if(reg_cfg[n].postfix == regnum ||
           reg_cfg[n].postfix == regnum + 1) {
            return &reg_cfg[n];
        }
    }
    return NULL;
}

#define LINE_SIZE 5000

/*static void pretty_dump(uint32_t cfg, uint32_t state, uint32_t pull,
			const char ** function_names_2,
			const char ** function_names_3,
			const char * prefix,
			int count)*/
static void pretty_dump(struct s3c_gpio_state_s *state, 
                        struct s3c_gpio_state_s *state_cache,
                        int bank,
                        int con,
                        int write,
                        const char ** function_names_2,
                        const char ** function_names_3,
                        const char *  prefix,
                        int count,
                        int *changed)
{
	int n;
	uint32_t cfg, dat, pull;
    const char *tag_type = NULL,
		   *tag_state = NULL,
		   *tag_pulldown = NULL,
		   * level0 = "0",
		   * level1 = "1";

    cfg = state->bank[bank].con;
    dat = state->bank[bank].dat;
    pull = state->bank[bank].up;
    
    *changed = 0;

    for(n = 0; n < count; n++) {
        /* If nothing has changed continue with the next pin */
        if ( (((cfg >> (2 * n)) & 3) == ((state_cache->bank[bank].con >> (2 * n)) & 3)) &&
             (((dat >> n) & 1) == ((state_cache->bank[bank].dat >> n) & 1)))
            continue;

        if(!*changed) {
            if(con)
                printf("GP%cCON: ", 'A' + bank);
            else 
                printf("GP%cDAT: ", 'A' + bank);
        }

        *changed = 1;
        
        switch((cfg >> (2 * n)) & 3) {
        case 0:
            tag_type = "in";
            break;
        case 1:
            tag_type = "out";
            break;
        case 2:
            if(function_names_2) {
                if(function_names_2[n])
                    tag_type = function_names_2[n];
                else 
                    tag_type = "ILLEGAL_CFG";
            } else
                tag_type = "function";
            break;
        default:
            if(function_names_3) {
                if(function_names_3[n])
                    tag_type = function_names_3[n];
                else
                    tag_type = "ILLEGAL_CFG";
            } else
                tag_type = "function";
            break;
        }
        
        if((dat >> n) & 1)
            tag_state = level1;
        else 
            tag_state = level0;
        
        if(((pull >> n) & 1))
            tag_pulldown = "";
        else
            tag_pulldown = "pulldown ";
        
        printf("GP%s%02d(%s)=%s %s", prefix, n, tag_type, tag_state,tag_pulldown);
    }

    if(*changed)
        printf("\n");
}

/*static void pretty_dump_a(uint32_t cfg, uint32_t state,
			  const char ** function_names,
			  const char * prefix,
			  int count)*/
static void pretty_dump_a(struct s3c_gpio_state_s *state,
                          struct s3c_gpio_state_s *state_cache,
                          int bank,
                          int con,
                          int write,
                          const char ** function_names,
                          const char * prefix,
                          int count,
                          int *changed)
{
	int n;
    uint32_t cfg, dat;
	const char *tag_type = NULL,
		   *tag_state = NULL,
		   * level0 = "0",
		   * level1 = "1";
 
    cfg = state->bank[bank].con;
    dat = state->bank[bank].dat;

    *changed = 0;

	for (n = 0; n < count; n++) {
        /* If nothing has changed continue with the next pin */
        if ( 
             (((cfg >> (2 * n)) & 3) == ((state_cache->bank[bank].con >> (2 * n)) & 3)) &&
             (((dat >> n) & 1) == ((state_cache->bank[bank].dat >> n) & 1)))
            continue;

        if(!*changed) {
            if(con)
                printf("GP%cCON: ", 'A' + bank);
            else
                printf("GP%cDAT: ", 'A' + bank);
        }

        *changed = 1;

		switch ((cfg >> n) & 1) {
		case 0:
			tag_type = "out";
			break;
		default:
			if (function_names) {
				if (function_names[n])
					tag_type = function_names[n];
				else
					tag_type = "ILLEGAL_CFG";
			} else
				tag_type = "function";
			break;
		}
		if ((dat >> n) & 1)
			tag_state = level1;
		else
			tag_state = level0;
        
        printf("GP%s%02d(%s)=%s ", prefix, n, tag_type, tag_state);
	}

    if(*changed)
        printf("\n");
}

void cache_state(void *vstate, void *vcache) {
    int n;
    struct s3c_gpio_state_s *state = (struct s3c_gpio_state_s*)vstate;
    struct s3c_gpio_state_s *cache = (struct s3c_gpio_state_s*)vcache;

    for(n=0; n<S3C_IO_BANKS; n++) {
        cache->bank[n].n = state->bank[n].n;
        cache->bank[n].con = state->bank[n].con;
        cache->bank[n].dat = state->bank[n].dat;
        cache->bank[n].up = state->bank[n].up;
        cache->bank[n].mask = state->bank[n].mask;
    }
}

void dump_register(void *opaque, void *cache, int bank, int read, int conf)
{
    struct reg *r = NULL;
    struct s3c_gpio_state_s *s = (struct s3c_gpio_state_s *) opaque;
    struct s3c_gpio_state_s *state_cache = (struct s3c_gpio_state_s*) cache;
    const char *port = "port",
         *configuration = "con",
         *destination = NULL;
    int changed = 0;

    if(conf)
        destination = configuration;
    else
        destination = port;

    r = get_reg_cfg('A' + bank);
    if(!r) {
        printf("bank=%i A+%i=%c\n",bank,bank,'A'+bank);
        printf("no configuration for register\n");
    }
    if(r && bank > 0)
        pretty_dump(s, state_cache, bank, conf, read, r->func_names2, r->func_names3, &r->postfix, r->num, &changed); 
    else if(r && bank == 0)
        pretty_dump_a(s, state_cache, bank, conf, read, r->func_names2, &r->postfix, r->num, &changed);
}

