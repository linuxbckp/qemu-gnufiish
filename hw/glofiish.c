/*
 * E-Ten Glofiish smartphone M800 / X800
 *
 * This code is licensed under the GNU GPL v2.
 */

#include "hw.h"
#include "s3c.h"
#include "arm-misc.h"
#include "sysemu.h"
#include "i2c.h"
#include "qemu-timer.h"
#include "qemu-char.h"
#include "devices.h"
#include "audio/audio.h"
#include "boards.h"
#include "console.h"
#include "usb.h"
#include "net.h"
#include "sd.h"
#include "dm9000.h"
#include "eeprom24c0x.h"
#include "glofiish.h"


struct glofiish_s {
    struct s3c_state_s *cpu;
    unsigned int ram;
    const char * kernel;
    SDState * mmc;
    struct nand_flash_s *nand;
    struct CharDriverState *modem;
    QEMUTimer *modem_timer;
};


static int glofiish_load_from_nand(struct nand_flash_s *nand,
		uint32_t nand_offset, uint32_t s3c_base_offset, uint32_t size)
{
	uint8_t buffer[512];
	uint32_t src = 0;
	int page = 0;
	uint32_t dst = 0;

	if (!nand)
		return 0;

	for (page = 0; page < (size / 512); page++, src += 512, dst += 512) {

		fprintf(stderr, "src: %d, dst: %d\n", src, dst);

		if (nand_readraw(nand, nand_offset + src, buffer, 512)) {
			cpu_physical_memory_write(s3c_base_offset + dst, buffer, 512);
		} else {
			fprintf(stderr, "%s: failed to load nand %d:%d\n", __FUNCTION__,
			        nand_offset + src, 512 + 16);
			return 0;
		}
	}
	return (int) size;
}

static void glofiish_reset(void *opaque)
{
    struct glofiish_s *s = (struct glofiish_s *) opaque;
    uint32_t image_size;

	/*
	 * Normally we would load 4 KB of nand to SRAM and jump there, but
	 * it is not working perfectly as expected, so we cheat and load
	 * it from nand directly relocated to 0x33f80000 and jump there
	 */
#if 0
	if (mini2440_load_from_nand(s->nand, 0, S3C_RAM_BASE | 0x03f80000, 256*1024)> 0) {
		fprintf(stderr, "%s: loaded default u-boot from NAND\n", __FUNCTION__);
		s->cpu->env->regs[15] = S3C_RAM_BASE | 0x03f80000; /* start address, u-boot already relocated */
	}
#endif

	if (glofiish_load_from_nand(s->nand, 0, S3C_SRAM_BASE_NANDBOOT, S3C_SRAM_SIZE) > 0) {
	    s->cpu->env->regs[15] = S3C_SRAM_BASE_NANDBOOT;	/* start address, u-boot relocating code */
	    fprintf(stderr, "%s: 4KB SteppingStone loaded from NAND\n", __FUNCTION__);
	}
	fprintf(stderr, "%d %d\n", S3C_SRAM_BASE_NANDBOOT, S3C_SRAM_SIZE);
#if 0
	/*
	 * if a u--boot is available as a file, we always use it
	 */
	{
	    image_size = load_image("mini2440/u-boot.bin", phys_ram_base + 0x03f80000);
	    if (image_size < 0)
		    image_size = load_image("u-boot.bin", phys_ram_base + 0x03f80000);
		if (image_size > 0) {
			if (image_size & (512 -1))	/* round size to a NAND block size */
				image_size = (image_size + 512) & ~(512-1);
	        fprintf(stderr, "%s: loaded override u-boot (size %x)\n", __FUNCTION__, image_size);
		    s->cpu->env->regs[15] = S3C_RAM_BASE | 0x03f80000;	/* start address, u-boot already relocated */
		}
	}
	/*
	 * if a kernel was explicitly specified, we load it too
	 */
	if (s->kernel) {
		image_size = load_image(s->kernel, phys_ram_base + 0x02000000);
		if (image_size > 0) {
			if (image_size & (512 -1))	/* round size to a NAND block size */
				image_size = (image_size + 512) & ~(512-1);
	        fprintf(stderr, "%s: loaded %s (size %x)\n", __FUNCTION__, s->kernel, image_size);
	    }
	}
#endif
}

static void glofiish_spi_setup(struct glofiish_s *board)
{
    /* Init the jbt6k74 controller */
    void *jbt6k74 = jbt6k74_init();
    s3c_spi_attach(board->cpu->spi, 0, 0, 0, 0);
    s3c_spi_attach(board->cpu->spi, 1, jbt6k74_txrx, jbt6k74_btxrx, jbt6k74);
}

/* Typical touchscreen calibration values */
static const int glofiish_ts_scale[6] = {
    0, (90 - 960) * 256 / 1021, -90 * 256 * 32,
    (940 - 75) * 256 / 1021, 0, 75 * 256 * 32,
};

#define GLOFIISH_FM_ADDR 0x10

static void glofiish_i2c_setup(struct glofiish_s *board)
{
    i2c_bus *bus = s3c_i2c_bus(board->cpu->i2c);

    
}

static void glofiish_modem_switch_tick(void *opaque)
{
    struct glofiish_s *s = (struct glofiish_s*) opaque;
    glofiish_modem_enable(s->modem, 1);
}

static void glofiish_modem_switch(void *opaque, int line, int level)
{
    struct glofiish_s *s = (struct glofiish_s*) opaque;
    
    if(s->modem) {
        if(level) {
            qemu_mod_timer(s->modem_timer, qemu_get_clock(vm_clock) + (ticks_per_sec >> 4));
        }
        else {
            qemu_del_timer(s->modem_timer);
            glofiish_modem_enable(s->modem, 0);
        }
    }

    fprintf(stderr, "Modem powered %s.\n", level ? "up" : "down");
}

static void glofiish_modem_rst_switch(void *opaque, int line, int level)
{
    if(level)
        fprintf(stderr,"Modem reset\n");
}

static void glofiish_gpio_setup(struct glofiish_s *s)
{
    s3c_gpio_out_set(s->cpu->io, GLOFIISH_M800_MODEM_RST,
                     *qemu_allocate_irqs(glofiish_modem_rst_switch, s, 1));
    s3c_gpio_out_set(s->cpu->io, GLOFIISH_M800_MODEM_PWRON,
                     *qemu_allocate_irqs(glofiish_modem_switch, s, 1));
}

/* Board init.  */
static struct glofiish_s *glofiish_init_common(int ram_size,
                const char *kernel_filename, const char *cpu_model,
                SDState *mmc)
{
    struct glofiish_s *s = (struct glofiish_s *)
            qemu_mallocz(sizeof(struct glofiish_s));
    DisplayState *ds = get_displaystate();

    s->ram = 0x04000000;
    s->kernel = kernel_filename;
    s->mmc = mmc;

    /* Setup CPU & memory */
    if (ram_size < s->ram + S3C_SRAM_SIZE) {
        fprintf(stderr, "This platform requires %i bytes of memory (not %d)\n",
                        s->ram + S3C_SRAM_SIZE, ram_size);
        exit(1);
    }
    if (cpu_model && strcmp(cpu_model, "arm920t")) {
        fprintf(stderr, "This platform requires an ARM920T core\n");
        exit(2);
    }
    s->cpu = s3c24xx_init(S3C_CPU_2440, 12000000 /* 12 mhz */, s->ram, S3C_SRAM_BASE_NANDBOOT, s->mmc);

    /* gpio setup */
    glofiish_gpio_setup(s);

	/* init glofiish cpld */
    glofiish_cpld_init(0x08000000);

    /* init glofiish modem */
    s->modem = glofiish_modem_init();
    s->modem_timer = qemu_new_timer(vm_clock, glofiish_modem_switch_tick, s);
    s3c_uart_attach(s->cpu->uart[0], s->modem);


    /* Setup peripherals */
   // glofiish_gpio_setup(s);
    glofiish_spi_setup(s);
    s3c_adc_setscale(s->cpu->adc, glofiish_ts_scale);

    /* Setup LCM display */
    if(ds) {
        ds->surface = qemu_resize_displaysurface(ds, 480, 640);
        dpy_resize(ds);
    }

    /* Setup initial (reset) machine state */
    qemu_register_reset(glofiish_reset, s);

    return s;
}

static void glofiish_init(ram_addr_t ram_size, int vga_ram_size,
                const char *boot_device,
                const char *kernel_filename, const char *kernel_cmdline,
                const char *initrd_filename, const char *cpu_model)
{
    struct glofiish_s *glofiish;
    int sd_idx = drive_get_index(IF_SD, 0, 0);
    SDState *sd = 0;

    if (sd_idx >= 0)
        sd = sd_init(drives_table[sd_idx].bdrv, 0);

    glofiish = glofiish_init_common(ram_size,
                    kernel_filename, cpu_model, sd);

	glofiish->nand = nand_init(NAND_MFR_SAMSUNG, 0xaa);
    glofiish->cpu->nand->reg(glofiish->cpu->nand, glofiish->nand);

    glofiish_reset(glofiish);
}

QEMUMachine glofiish_machine = {
    "glofiish",
    "E-Ten glofiish smartphone M800/X800 (S3C2442A)",
    .init = glofiish_init,
/*    .ram_require = (0x04000000 + S3C_SRAM_SIZE) | RAMSIZE_FIXED */
};

