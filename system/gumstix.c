/*
 *  linux/arch/arm/mach-pxa/gumstix.c
 *
 *  Support for the Gumstix computer platform
 *
 *  Author:	Craig Hughes
 *  Created:	December 8 2004
 *  Copyright:	(C) 2004, Craig Hughes
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <asm/types.h>

#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <asm/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/arch/ohci.h>
#include <asm/arch/udc.h>
#include <asm/arch/mmc.h>
#include <asm/arch/pxa-regs.h>
#include <asm/arch/pxafb.h>
#include <asm/arch/gumstix.h>

#include "generic.h"

static struct pxamci_platform_data gumstix_mci_platform_data;

static int gumstix_mci_init(struct device *dev, irq_handler_t gumstix_detect_int, void *data)
{
#ifndef CONFIG_ARCH_GUMSTIX_VERDEX 
	int err;

	pxa_gpio_mode(GPIO6_MMCCLK_MD);
	pxa_gpio_mode(GPIO53_MMCCLK_MD);
	pxa_gpio_mode(GPIO8_MMCCS0_MD);

	pxa_gpio_mode(GUMSTIX_GPIO_nSD_DETECT | GPIO_IN);
	set_irq_type(GUMSTIX_IRQ_GPIO_nSD_DETECT, IRQT_BOTHEDGE);
	pxa_gpio_mode(GUMSTIX_GPIO_nSD_WP | GPIO_IN);

	gumstix_mci_platform_data.detect_delay = msecs_to_jiffies(250);

	err = request_irq(GUMSTIX_IRQ_GPIO_nSD_DETECT, gumstix_detect_int, SA_INTERRUPT,
			     "MMC card detect", data);
	if (err) {
		printk(KERN_ERR "gumstix_mci_init: MMC/SD: can't request MMC card detect IRQ\n");
		return -1;
	}

	err = set_irq_type(GUMSTIX_IRQ_GPIO_nSD_DETECT, IRQT_BOTHEDGE);
#else
	// Setup GPIOs for MMC on the 120-pin connector
	// There is no card detect on a uSD connector so no interrupt to register
	// There is no WP detect GPIO line either
	pxa_gpio_mode(GPIO92_MMCDAT0_MD);
	pxa_gpio_mode(GPIO112_MMCCMD_MD);
	pxa_gpio_mode(GPIO110_MMCDAT2_MD);
	pxa_gpio_mode(GPIO111_MMCDAT3_MD);
	pxa_gpio_mode(GPIO109_MMCDAT1_MD);
	pxa_gpio_mode(GPIO32_MMCCLK_MD);
#endif

	return 0;
}

static int gumstix_mci_get_ro(struct device *dev)
{
#ifdef CONFIG_ARCH_GUMSTIX_VERDEX
	return 0;	// microSD is always writable on verdex
#else
	int ro;
	ro = GPLR(GUMSTIX_GPIO_nSD_WP) & GPIO_bit(GUMSTIX_GPIO_nSD_WP);
	return ro;
#endif
}

static void gumstix_mci_exit(struct device *dev, void *data)
{
	free_irq(GUMSTIX_IRQ_GPIO_nSD_DETECT, data);
}

static struct pxamci_platform_data gumstix_mci_platform_data = {
	.ocr_mask	= MMC_VDD_32_33|MMC_VDD_33_34,
	.init		= gumstix_mci_init,
	.get_ro		= gumstix_mci_get_ro,
	.exit		= gumstix_mci_exit,
};

static struct pxa2xx_udc_mach_info gumstix_udc_info __initdata = {
	.gpio_vbus	= GPIO_GUMSTIX_USB_GPIOn,
	.gpio_pullup	= GPIO_GUMSTIX_USB_GPIOx,
};

static struct platform_device gum_audio_device = {
	.name		= "pxa2xx-ac97",
	.id		= -1,
};


#if defined(CONFIG_FB_PXA_SHARP_LQ043_PSP) || defined(CONFIG_FB_PXA_SAMSUNG_LTE430WQ_F0C)
static void gumstix_lcd_backlight(int on_or_off)
{
	if(on_or_off)
	{
		pxa_gpio_mode(17 | GPIO_IN);
	} else {
		GPCR(17) = GPIO_bit(17);
		pxa_gpio_mode(17 | GPIO_OUT);
		GPCR(17) = GPIO_bit(17);
	}
}
#endif


#ifdef CONFIG_FB_PXA_ALPS_CDOLLAR
static struct pxafb_mode_info gumstix_fb_mode = {
        .pixclock       = 150000,
        .xres           = 320,
        .yres           = 240,
        .bpp            = 16,
        .hsync_len      = 30,
        .left_margin    = 12,
        .right_margin   = 38,
        .vsync_len      = 3,
        .upper_margin   = 15,
        .lower_margin   = 4,
        .sync           = FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
};

static struct pxafb_mach_info gumstix_fb_info = {
        .modes          = &gumstix_fb_mode,
        .num_modes      = 1,
        .lccr0          = LCCR0_Act | LCCR0_Sngl | LCCR0_Color,
        .lccr3          = LCCR3_PixFlEdg,
};

#elif defined(CONFIG_FB_PXA_SHARP_LQ043_PSP)
static struct pxafb_mode_info gumstix_fb_mode = {
	.pixclock	= 110000,
	.xres		= 480,
	.yres		= 272,
	.bpp		= 16,
	.hsync_len	= 41,
	.left_margin	= 2,
	.right_margin	= 2,
	.vsync_len	= 10,
	.upper_margin	= 2,
	.lower_margin	= 2,
	.sync		= 0, // Hsync and Vsync both active low
};

static struct pxafb_mach_info gumstix_fb_info = {
	.modes			= &gumstix_fb_mode,
	.num_modes		= 1,
	.lccr0			= LCCR0_Act | LCCR0_Sngl | LCCR0_Color,
	.lccr3			= LCCR3_OutEnH | LCCR3_PixFlEdg | (0 << 30),
	.pxafb_backlight_power	= &gumstix_lcd_backlight,
};
#elif defined(CONFIG_FB_PXA_SAMSUNG_LTE430WQ_F0C)
static struct pxafb_mode_info gumstix_fb_mode = {
	.pixclock	= 108696, // 9.2MHz typical DOTCLK from datasheet
	.xres		= 480,
	.hsync_len	= 41, // HLW from datasheet: 41 typ
	.left_margin	= 4, // HBP - HLW from datasheet: 45 - 41 = 4
	.right_margin	= 8, // HFP from datasheet: 8 typ
	.yres		= 272,
	.vsync_len	= 10, // VLW from datasheet: 10 typ
	.upper_margin	= 2, // VBP - VLW from datasheet: 12 - 10 = 2
	.lower_margin	= 4, // VFP from datasheet: 4 typ
	.bpp		= 16,
	.sync		= 0, // Hsync and Vsync both active low
};

static struct pxafb_mach_info gumstix_fb_info = {
	.modes			= &gumstix_fb_mode,
	.num_modes		= 1,
	.lccr0			= LCCR0_Act | LCCR0_Sngl | LCCR0_Color,
	.lccr3			= LCCR3_OutEnH | LCCR3_PixFlEdg | (0 << 30),
	.pxafb_backlight_power	= &gumstix_lcd_backlight,
};
#endif

static struct platform_device *devices[] __initdata = {
	&gum_audio_device,
};

#ifdef CONFIG_ARCH_GUMSTIX_VERDEX
static int gumstix_ohci_init(struct device *dev)
{
	/* setup Port1 GPIO pin. */
	//pxa_gpio_mode( 88 | GPIO_ALT_FN_1_IN);	/* USBHPWR1 */
	//pxa_gpio_mode( 89 | GPIO_ALT_FN_2_OUT);	/* USBHPEN1 */

	// Turn on port 2 in host mode
	UP2OCR = UP2OCR_HXS | UP2OCR_HXOE | UP2OCR_DPPDE | UP2OCR_DMPDE;

	UHCHR = (UHCHR) &
		~(UHCHR_SSEP1 | UHCHR_SSEP2 | UHCHR_SSEP3 | UHCHR_SSE);

	return 0;
}

static struct pxaohci_platform_data gumstix_ohci_platform_data = {
	.port_mode	= PMM_PERPORT_MODE,
	.init		= gumstix_ohci_init,
};
#endif

static void __init gumstix_init(void)
{
	pxa_set_mci_info(&gumstix_mci_platform_data);
#ifdef CONFIG_ARCH_GUMSTIX_VERDEX
	pxa_set_ohci_info(&gumstix_ohci_platform_data);
#endif
	pxa_set_udc_info(&gumstix_udc_info);
#if defined(CONFIG_FB_PXA_ALPS_CDOLLAR) | defined(CONFIG_FB_PXA_SHARP_LQ043_PSP) | defined(CONFIG_FB_PXA_SAMSUNG_LTE430WQ_F0C)
	set_pxa_fb_info(&gumstix_fb_info);
#endif
	(void) platform_add_devices(devices, ARRAY_SIZE(devices));
}

MACHINE_START(GUMSTIX, "The Gumstix Platform")
	.phys_io	= 0x40000000,
	.boot_params	= 0xa0000100,
	.io_pg_offst	= (io_p2v(0x40000000) >> 18) & 0xfffc,
	.timer          = &pxa_timer,
	.map_io		= pxa_map_io,
	.init_irq	= pxa_init_irq,
	.init_machine	= gumstix_init,
MACHINE_END
