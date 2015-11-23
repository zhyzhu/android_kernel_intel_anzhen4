/*
 * platform_camera.c: Camera platform library file
 *
 * (C) Copyright 2008 Intel Corporation
 * Author:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/lnw_gpio.h>
#include <linux/atomisp_platform.h>
#include <linux/module.h>
#include <media/v4l2-subdev.h>
#include <asm/intel-mid.h>
#include <asm/intel_scu_pmic.h>
#include "platform_camera.h"
#include "platform_imx175.h"
#include "platform_imx134.h"
#include "platform_ov2722.h"
#include "platform_bf3905.h"
#include "platform_gc0339.h"
#include "platform_gc2155.h"
#include "platform_gc2235.h"
#include "platform_gc2235f.h"
#include "platform_gc5004.h"
#include "platform_gc0310.h"
#include "platform_ov2685.h"
#include "platform_ov2685f.h"
#include "platform_gc0310soc.h"
#include "platform_ov7736.h"
#include "platform_ov5670.h"
#include "platform_ov5648.h"
#include "platform_ov2680b.h"
#include "platform_ov2680f.h"
#include "platform_ov8858.h"
#include "platform_ov5693.h"
#include "platform_lm3554.h"
#include "platform_lm3642.h"
#include "platform_hm2056.h"
#include "platform_ap1302.h"
#include "platform_pixter.h"
#include "platform_m10mo.h"
#include "platform_bf3a20.h"
#include "platform_hm5040.h"
#ifdef CONFIG_CRYSTAL_COVE
#include <linux/mfd/intel_mid_pmic.h>
#endif

/*
 * TODO: Check whether we can move this info to OEM table or
 *       set this info in the platform data of each sensor
 */
const struct intel_v4l2_subdev_id v4l2_ids[] = {
	{"mt9e013", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"ov8830", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"imx175", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"imx135", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"imx135fuji", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"imx134", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"gc2235b", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"hm2056", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"bf3a20", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"imx132", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"ov9724", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"ov2722", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"bf3905", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"gc0339", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"gc2235f", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"gc0310", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"gc0310soc", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"gc2155", SOC_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"gc5004", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"ov5693", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"ov5648", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"ov2680b", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"ov8858", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"ov2680f", RAW_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"ov2685", SOC_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"ov2685f", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"ov7736", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"ov5670", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"mt9d113", SOC_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"mt9m114", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"mt9v113", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"s5k8aay", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"ap1302", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"hm5040", RAW_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"lm3554", LED_FLASH, -1},
	{"lm3559", LED_FLASH, -1},
	{"lm3560", LED_FLASH, -1},
	{"lm3642", LED_FLASH, -1},
	{"xactor_a", SOC_CAMERA, ATOMISP_CAMERA_PORT_PRIMARY},
	{"xactor_b", SOC_CAMERA, ATOMISP_CAMERA_PORT_SECONDARY},
	{"xactor_c", SOC_CAMERA, ATOMISP_CAMERA_PORT_TERTIARY},
	{"pixter_0", PIXTER_0_TYPE, ATOMISP_CAMERA_PORT_PRIMARY},
	{"pixter_1", PIXTER_1_TYPE, ATOMISP_CAMERA_PORT_SECONDARY},
	{"pixter_2", PIXTER_2_TYPE, ATOMISP_CAMERA_PORT_TERTIARY},
	{},
};

struct camera_device_table {
	struct sfi_device_table_entry entry;
	struct devs_id dev;
};

/* Baytrail Cherrytrail camera devs table */
#ifdef CONFIG_ACPI
static struct camera_device_table byt_ffrd10_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 4, 0x10, 0x0, 0x0, "imx175"},
		{"imx175", SFI_DEV_TYPE_I2C, 0, &imx175_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x36, 0x0, 0x0, "ov2722"},
		{"ov2722", SFI_DEV_TYPE_I2C, 0, &ov2722_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x53, 0x0, 0x0, "lm3554"},
		{"lm3554", SFI_DEV_TYPE_I2C, 0, &lm3554_platform_data_func,
			&intel_register_i2c_camera_device}
	}
};

static struct camera_device_table byt_ffrd8_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 4, 0x10, 0x0, 0x0, "imx134"},
		{"imx134", SFI_DEV_TYPE_I2C, 0, &imx134_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x36, 0x0, 0x0, "ov2722"},
		{"ov2722", SFI_DEV_TYPE_I2C, 0, &ov2722_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 3, 0x53, 0x0, 0x0, "lm3554"},
		{"lm3554", SFI_DEV_TYPE_I2C, 0, &lm3554_platform_data_func,
			&intel_register_i2c_camera_device}
	}
};
#ifdef CONFIG_MRD7
static struct camera_device_table byt_crv2_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 2, 0x3c, 0x0, 0x0, "ov2685"},
		{"ov2685", SFI_DEV_TYPE_I2C, 0, &ov2685_platform_data,
			&intel_register_i2c_camera_device}
	},  {
		{SFI_DEV_TYPE_I2C, 2, 0x3c, 0x0, 0x0, "ov2685f"},
		{"ov2685f", SFI_DEV_TYPE_I2C, 0, &ov2685f_platform_data,
			&intel_register_i2c_camera_device}
	},  {
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "ov5670"},
		{"ov5670", SFI_DEV_TYPE_I2C, 0, &ov5670_platform_data,
			&intel_register_i2c_camera_device}
	}, 	{
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "gc2155"},
		{"gc2155", SFI_DEV_TYPE_I2C, 0, &gc2155_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "gc2235b"},
		{"gc2235b", SFI_DEV_TYPE_I2C, 0, &gc2235b_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x24, 0x0, 0x0, "hm2056"},
		{"hm2056", SFI_DEV_TYPE_I2C, 0, &hm2056_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "gc5004"},
		{"gc5004", SFI_DEV_TYPE_I2C, 0, &gc5004_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "ov5648"},
		{"ov5648", SFI_DEV_TYPE_I2C, 0, &ov5648_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		// {SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "hm5040"},
        {SFI_DEV_TYPE_I2C, 2, 0x1b, 0x0, 0x0, "hm5040"},
        {"hm5040", SFI_DEV_TYPE_I2C, 0, &hm5040_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "ov2680b"},
		{"ov2680b", SFI_DEV_TYPE_I2C, 0, &ov2680b_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x10, 0x0, 0x0, "ov8858"},
		{"ov8858", SFI_DEV_TYPE_I2C, 0, &ov8858_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "ov8858"},
		{"ov8858", SFI_DEV_TYPE_I2C, 0, &ov8858_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x21, 0x0, 0x0, "gc0339"},
		{"gc0339", SFI_DEV_TYPE_I2C, 0, &gc0339_platform_data,
			&intel_register_i2c_camera_device}
	},  {
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "ov7736"},
		{"ov7736", SFI_DEV_TYPE_I2C, 0, &ov7736_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x10, 0x0, 0x0, "ov2680f"},
		{"ov2680f", SFI_DEV_TYPE_I2C, 0, &ov2680f_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x21, 0x0, 0x0, "gc0310"},
		{"gc0310", SFI_DEV_TYPE_I2C, 0, &gc0310_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "gc2235f"},
		{"gc2235f", SFI_DEV_TYPE_I2C, 0, &gc2235f_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x21, 0x0, 0x0, "gc0310soc"},
		{"gc0310soc", SFI_DEV_TYPE_I2C, 0, &gc0310soc_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x6f, 0x0, 0x0, "bf3905"},
		{"bf3905", SFI_DEV_TYPE_I2C, 0, &bf3905_platform_data,
			&intel_register_i2c_camera_device}
	},{
		{SFI_DEV_TYPE_I2C, 2, 0x6f, 0x0, 0x0, "bf3a20"},
		{"bf3a20", SFI_DEV_TYPE_I2C, 0, &bf3a20_platform_data,
			&intel_register_i2c_camera_device}
	}
};

#elif defined CONFIG_MRD8
static struct camera_device_table byt_crv2_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 2, 0x3c, 0x0, 0x0, "ov2685"},
		{"ov2685", SFI_DEV_TYPE_I2C, 0, &ov2685_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x3c, 0x0, 0x0, "ov2685f"},
		{"ov2685f", SFI_DEV_TYPE_I2C, 0, &ov2685f_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "ov7736"},
		{"ov7736", SFI_DEV_TYPE_I2C, 0, &ov7736_platform_data,
			&intel_register_i2c_camera_device}
	},{
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "ov5670"},
		{"ov5670", SFI_DEV_TYPE_I2C, 0, &ov5670_platform_data,
			&intel_register_i2c_camera_device}
	},  {
		{SFI_DEV_TYPE_I2C, 2, 0x10, 0x0, 0x0, "ov5693"},
		{"ov5693", SFI_DEV_TYPE_I2C, 0, &ov5693_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "ov2722"},
		{"ov2722", SFI_DEV_TYPE_I2C, 0, &ov2722_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "gc2155"},
		{"gc2155", SFI_DEV_TYPE_I2C, 0, &gc2155_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x21, 0x0, 0x0, "gc0310soc"},
		{"gc0310soc", SFI_DEV_TYPE_I2C, 0, &gc0310soc_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "gc2235f"},
		{"gc2235f", SFI_DEV_TYPE_I2C, 0, &gc2235f_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "gc5004"},
		{"gc5004", SFI_DEV_TYPE_I2C, 0, &gc5004_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "ov5648"},
		{"ov5648", SFI_DEV_TYPE_I2C, 0, &ov5648_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "ov2680b"},
		{"ov2680b", SFI_DEV_TYPE_I2C, 0, &ov2680b_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x21, 0x0, 0x0, "gc0339"},
		{"gc0339", SFI_DEV_TYPE_I2C, 0, &gc0339_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x10, 0x0, 0x0, "ov2680f"},
		{"ov2680f", SFI_DEV_TYPE_I2C, 0, &ov2680f_platform_data,
			&intel_register_i2c_camera_device}
	}, {
                 // {SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "hm5040"},
                 {SFI_DEV_TYPE_I2C, 2, 0x1b, 0x0, 0x0, "hm5040"},
                 {"hm5040", SFI_DEV_TYPE_I2C, 0, &hm5040_platform_data,
                         &intel_register_i2c_camera_device}
	},
};

#else


static struct camera_device_table byt_crv2_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 2, 0x3C, 0x0, 0x0, "gc2235b"},
		{"gc2235b", SFI_DEV_TYPE_I2C, 0, &gc2235b_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x10, 0x0, 0x0, "imx134"},
		{"imx134", SFI_DEV_TYPE_I2C, 0, &imx134_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x36, 0x0, 0x0, "ov2722"},
		{"ov2722", SFI_DEV_TYPE_I2C, 0, &ov2722_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x53, 0x0, 0x0, "lm3554"},
		{"lm3554", SFI_DEV_TYPE_I2C, 0, &lm3554_platform_data_func,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 2, 0x63, 0x0, 0x0, "lm3642"},
		{"lm3642", SFI_DEV_TYPE_I2C, 0, &lm3642_platform_data_func,
			&intel_register_i2c_camera_device}
	}
};
#endif

#ifdef CHT_RVP_USE_BYT_CAM_AOB
static struct camera_device_table cht_rvp_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 4, 0x10, 0x0, 0x0, "imx175"},
		{"imx175", SFI_DEV_TYPE_I2C, 0, &imx175_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x36, 0x0, 0x0, "ov2722"},
		{"ov2722", SFI_DEV_TYPE_I2C, 0, &ov2722_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x53, 0x0, 0x0, "lm3554"},
		{"lm3554", SFI_DEV_TYPE_I2C, 0, &lm3554_platform_data_func,
			&intel_register_i2c_camera_device}
	}
};
#else
static struct camera_device_table cht_rvp_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 2, 0x10, 0x0, 0x0, "imx175"},
		{"imx175", SFI_DEV_TYPE_I2C, 0, &imx175_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x3C, 0x0, 0x0, "ap1302"},
		{"ap1302", SFI_DEV_TYPE_I2C, 0, &ap1302_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 1, 0x53, 0x0, 0x0, "lm3554"},
		{"lm3554", SFI_DEV_TYPE_I2C, 0, &lm3554_platform_data_func,
			&intel_register_i2c_camera_device}
	}
};
#endif


static struct camera_device_table cht_ffd_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 4, 0x10, 0x0, 0x0, "ov5693"},
		{"ov5693", SFI_DEV_TYPE_I2C, 0, &ov5693_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x36, 0x0, 0x0, "ov2722"},
		{"ov2722", SFI_DEV_TYPE_I2C, 0, &ov2722_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 1, 0x53, 0x0, 0x0, "lm3554"},
		{"lm3554", SFI_DEV_TYPE_I2C, 0, &lm3554_platform_data_func,
			&intel_register_i2c_camera_device}
	}
};

static struct camera_device_table cht_somc_cam_table[] = {
       {
               {SFI_DEV_TYPE_I2C, 4, 0x1f, 0x0, 0x0, "m10mo"},
               {"m10mo", SFI_DEV_TYPE_I2C, 0, &m10mo_platform_data,
                       &intel_register_i2c_camera_device}
       },
};

#ifdef CONFIG_VIDEO_PIXTER
static struct camera_device_table pixter_cam_table[] = {
	{
		{SFI_DEV_TYPE_I2C, 4, 0x70, 0x0, 0x0, "pixter_0"},
		{"pixter_0", SFI_DEV_TYPE_I2C, 0, &pixter_0_platform_data,
			&intel_register_i2c_camera_device}
	}, {
		{SFI_DEV_TYPE_I2C, 4, 0x72, 0x0, 0x0, "pixter_1"},
		{"pixter_1", SFI_DEV_TYPE_I2C, 0, &pixter_1_platform_data,
			&intel_register_i2c_camera_device}
	}
};
#endif

#endif
static struct atomisp_camera_caps default_camera_caps;
/*
 * One-time gpio initialization.
 * @name: gpio name: coded in SFI table
 * @gpio: gpio pin number (bypass @name)
 * @dir: GPIOF_DIR_IN or GPIOF_DIR_OUT
 * @value: if dir = GPIOF_DIR_OUT, this is the init value for output pin
 * if dir = GPIOF_DIR_IN, this argument is ignored
 * return: a positive pin number if succeeds, otherwise a negative value
 */
int camera_sensor_gpio(int gpio, char *name, int dir, int value)
{
	int ret, pin;

	if (gpio == -1) {
		pin = get_gpio_by_name(name);
		if (pin == -1) {
			pr_err("%s: failed to get gpio(name: %s)\n",
						__func__, name);
			return -EINVAL;
		}
		pr_info("camera pdata: gpio: %s: %d\n", name, pin);
	} else {
		pin = gpio;
	}

	ret = gpio_request(pin, name);
	if (ret) {
		pr_err("%s: failed to request gpio(pin %d)\n", __func__, pin);
		return -EINVAL;
	}

	if (dir == GPIOF_DIR_OUT)
		ret = gpio_direction_output(pin, value);
	else
		ret = gpio_direction_input(pin);

	if (ret) {
		pr_err("%s: failed to set gpio(pin %d) direction\n",
							__func__, pin);
		gpio_free(pin);
	}

	return ret ? ret : pin;
}

void camera_sensor_gpio_free(int pin)
{
	if (pin != -1)
		gpio_free(pin);
}

/*
 * Configure MIPI CSI physical parameters.
 * @port: ATOMISP_CAMERA_PORT_PRIMARY or ATOMISP_CAMERA_PORT_SECONDARY
 * @lanes: for ATOMISP_CAMERA_PORT_PRIMARY, there could be 2 or 4 lanes
 * for ATOMISP_CAMERA_PORT_SECONDARY, there is only one lane.
 * @format: MIPI CSI pixel format, see include/linux/atomisp_platform.h
 * @bayer_order: MIPI CSI bayer order, see include/linux/atomisp_platform.h
 */
int camera_sensor_csi(struct v4l2_subdev *sd, u32 port,
			u32 lanes, u32 format, u32 bayer_order, int flag)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct camera_mipi_info *csi = NULL;

	if (flag) {
		csi = kzalloc(sizeof(*csi), GFP_KERNEL);
		if (!csi) {
			dev_err(&client->dev, "out of memory\n");
			return -ENOMEM;
		}
		csi->port = port;
		csi->num_lanes = lanes;
		csi->input_format = format;
		csi->raw_bayer_order = bayer_order;
		v4l2_set_subdev_hostdata(sd, (void *)csi);
		csi->metadata_format = ATOMISP_INPUT_FORMAT_EMBEDDED;
		csi->metadata_effective_width = NULL;
		dev_info(&client->dev,
			 "camera pdata: port: %d lanes: %d order: %8.8x\n",
			 port, lanes, bayer_order);
	} else {
		csi = v4l2_get_subdev_hostdata(sd);
		kfree(csi);
	}

	return 0;
}
static int no_v4l2_dev_ids(void)
{
	const struct intel_v4l2_subdev_id *v4l2_ids_ptr = v4l2_ids;
	int no_v4l2_ids = 0;

	while (v4l2_ids_ptr->name[0]) {
		no_v4l2_ids++;
		v4l2_ids_ptr++;
	}

	return no_v4l2_ids;
}
static const struct intel_v4l2_subdev_id *get_v4l2_ids(int *n_subdev)
{
	if (n_subdev && v4l2_ids)
		*n_subdev = no_v4l2_dev_ids();
	return v4l2_ids;
}

static struct atomisp_platform_data *atomisp_platform_data;

#ifdef CONFIG_ACPI
void atomisp_register_device_table(int entry_num,
	struct camera_device_table *table)
{
	int i;
	for (i = 0; i < entry_num; i++, table++) {
		if (table->dev.device_handler)
			table->dev.device_handler(&table->entry,
				&table->dev);
	}
}
#endif

void intel_register_i2c_camera_device(struct sfi_device_table_entry *pentry,
					struct devs_id *dev)
{
	struct i2c_board_info i2c_info;
	struct i2c_board_info *idev = &i2c_info;
	int bus = pentry->host_num;
	void *pdata = NULL;
	int n_subdev;
	const struct intel_v4l2_subdev_id *vdev = get_v4l2_ids(&n_subdev);
	struct intel_v4l2_subdev_i2c_board_info *info;
	static struct intel_v4l2_subdev_table *subdev_table;
	enum intel_v4l2_subdev_type type = 0;
	enum atomisp_camera_port port;
	static int i;

	if (vdev == NULL) {
		pr_info("ERROR: camera vdev list is NULL\n");
		return;
	}

	memset(&i2c_info, 0, sizeof(i2c_info));
	strncpy(i2c_info.type, pentry->name, SFI_NAME_LEN);
	i2c_info.irq = ((pentry->irq == (u8)0xff) ? 0 : pentry->irq);
	i2c_info.addr = pentry->addr;
	pr_info("camera pdata: I2C bus = %d, name = %16.16s, irq = 0x%2x, addr = 0x%x\n",
		pentry->host_num, i2c_info.type, i2c_info.irq, i2c_info.addr);

	if (!dev->get_platform_data)
		return;
	pdata = dev->get_platform_data(&i2c_info);
	i2c_info.platform_data = pdata;


	while (vdev->name[0]) {
		if (!strncmp(vdev->name, idev->type, 16)) {
			/* compare name */
			type = vdev->type;
			port = vdev->port;
			break;
		}
		vdev++;
	}

	if (!type) /* not found */
		return;

	info = kzalloc(sizeof(struct intel_v4l2_subdev_i2c_board_info),
		       GFP_KERNEL);
	if (!info) {
		pr_err("MRST: fail to alloc mem for ignored i2c dev %s\n",
		       idev->type);
		return;
	}

	info->i2c_adapter_id = bus;
	/* set platform data */
	memcpy(&info->board_info, idev, sizeof(*idev));

	if (atomisp_platform_data == NULL) {
		subdev_table = kzalloc(sizeof(struct intel_v4l2_subdev_table)
			* n_subdev, GFP_KERNEL);

		if (!subdev_table) {
			pr_err("MRST: fail to alloc mem for v4l2_subdev_table %s\n",
			       idev->type);
			kfree(info);
			return;
		}

		atomisp_platform_data = kzalloc(
			sizeof(struct atomisp_platform_data), GFP_KERNEL);
		if (!atomisp_platform_data) {
			pr_err("%s: fail to alloc mem for atomisp_platform_data %s\n",
			       __func__, idev->type);
			kfree(info);
			kfree(subdev_table);
			return;
		}
		i = 0;
		atomisp_platform_data->subdevs = subdev_table;
	}

	memcpy(&subdev_table[i].v4l2_subdev, info, sizeof(*info));
	subdev_table[i].type = type;
	subdev_table[i].port = port;
	i++;
	kfree(info);
	return;
}

#ifdef CONFIG_ACPI
#if 0
static int match_name(struct device *dev, void *data)
{
	const char *name = data;
	struct i2c_client *client = to_i2c_client(dev);
	return !strncmp(client->name, name, strlen(client->name));
}

struct i2c_client *i2c_find_client_by_name(char *name)
{
	struct device *dev = bus_find_device(&i2c_bus_type, NULL,
						name, match_name);
	return dev ? to_i2c_client(dev) : NULL;
}
#endif
/*
 * In BTY, ACPI enumination will register all the camera i2c devices
 * which will cause v4l2_i2c_new_subdev_board() failed called in atomisp
 * driver.
 * Here we unregister the devices registered by ACPI
 */
static void atomisp_unregister_acpi_devices(struct atomisp_platform_data *pdata)
{
	const char *subdev_name[] = {
		"2-0036",	/* ov2722 driver*/
		"2-006f",   /* bf3a20 driver*/
#ifndef CONFIG_MRD7
		"3-0053",	/* FFRD8 lm3554 */
		"4-0036",	/* ov2722 */
		"4-0010",	/* imx1xx Sensor*/
		"4-0053",	/* FFRD10 lm3554 */
		"4-0054",	/* imx1xx EEPROM*/
		"4-000c",	/* imx1xx driver*/
		"2-0053",	/* byt-crv2 lm3554*/
		"2-0063",	/* byt-crv2 lm3642*/
		"2-0010",	/* imx1xx driver*/
		"2-0036",	/* ov2722 driver*/
		"2-003c",	/* gc2235 driver*/
		"2-0010",	/* CHT OV5693 */
		"4-003c",	/* CHT AP1302 */
		"1-0053",	/* CHT lm3554 */
		"4-001f",	/* CHT m10mo */
#if 0
		"INTCF0B:00",	/* From ACPI ov2722 */
		"INTCF1A:00",	/* From ACPI imx175 */
		"INTCF1C:00",	/* From ACPI lm3554 */
#endif
#endif
	};
	struct device *dev;
	struct i2c_client *client;
	struct i2c_board_info board_info;
	int i;
	/* search by device name */
	for (i = 0; i < ARRAY_SIZE(subdev_name); i++) {
		dev = bus_find_device_by_name(&i2c_bus_type, NULL,
					      subdev_name[i]);
		if (dev) {
			client = to_i2c_client(dev);
			board_info.flags = client->flags;
			board_info.addr = client->addr;
			board_info.irq = client->irq;
			strlcpy(board_info.type, client->name,
				sizeof(client->name));
			i2c_unregister_device(client);
		}
	}
#if 0
	/* search by client name */
	for (i = 0; i < ARRAY_SIZE(subdev_name); i++) {
		client = i2c_find_client_by_name(subdev_name[i]);
		if (client) {
			board_info.flags = client->flags;
			board_info.addr = client->addr;
			board_info.irq = client->irq;
			strlcpy(board_info.type, client->name,
				sizeof(client->name));
			i2c_unregister_device(client);
		}
	}
#endif
}
#endif
const struct atomisp_platform_data *atomisp_get_platform_data(void)
{
	if (atomisp_platform_data) {
#ifdef CONFIG_ACPI
		atomisp_unregister_acpi_devices(atomisp_platform_data);
#endif
		atomisp_platform_data->spid = &spid;
		return atomisp_platform_data;
	} else {
		pr_err("%s: no camera device in the SFI table\n", __func__);
		return NULL;
	}
}
EXPORT_SYMBOL_GPL(atomisp_get_platform_data);

const struct atomisp_camera_caps *atomisp_get_default_camera_caps(void)
{
	static bool init;
	if (!init) {
		default_camera_caps.sensor_num = 1;
		default_camera_caps.sensor[0].stream_num = 1;
		default_camera_caps.sensor[0].is_slave = false;
		init = true;
	}
	return &default_camera_caps;
}
EXPORT_SYMBOL_GPL(atomisp_get_default_camera_caps);

static struct vprog_status status[CAMERA_POWER_NUM] = {0};
int gc_camera_set_pmic(bool flag)
{
	/*MRD7 0x1A:default0x00-11=1.8v; 0x28:default0x17, =21=2.8?*/
        /*MRD7 0x12, bit4; 0x13, bit5*/
	//u8 reg_addr[CAMERA_POWER_NUM] = {VPROG_1P8V, VPROG_2P8V};
	//u8 reg_value[2] = {VPROG_DISABLE, VPROG_ENABLE};
	u8 reg_addr[CAMERA_POWER_NUM] = {0x12, 0x13};
	u8 reg_value[2] = {0x2, 0x20};
	u8 reg_adjust_addr[2] = {0x1A, 0x28};
	u8 reg_adjust_value[2] = {22, 21}; /* 1.8v for ELDO2, 2.8v for ALDO1 */
//	static struct vprog_status status;
	static DEFINE_MUTEX(mutex_power);
	int ret = 0;
	int user = status[CAMERA_1P8V].user + status[CAMERA_2P8V].user;

	mutex_lock(&mutex_power);
	/*
	 * only set power at:
	 * first to power on
	 * last to power off
	 */
	if ((flag && user == 0)
	    || (!flag && user == 2))
//		ret = intel_mid_pmic_writeb(reg_addr[pin], reg_value[flag]);
	{
		if (flag)
		{
			ret = intel_mid_pmic_writeb(reg_adjust_addr[CAMERA_1P8V], reg_adjust_value[CAMERA_1P8V]);
			ret |= intel_mid_pmic_setb(reg_addr[CAMERA_1P8V], reg_value[CAMERA_1P8V]);
			ret |= intel_mid_pmic_writeb(reg_adjust_addr[CAMERA_2P8V], reg_adjust_value[CAMERA_2P8V]);
			ret |= intel_mid_pmic_setb(reg_addr[CAMERA_2P8V], reg_value[CAMERA_2P8V]);
		}
		else
		{
			ret |= intel_mid_pmic_clearb(reg_addr[CAMERA_2P8V], reg_value[CAMERA_2P8V]);
			ret |= intel_mid_pmic_clearb(reg_addr[CAMERA_1P8V], reg_value[CAMERA_1P8V]);
		}
	}



	/* no update counter if config failed */
	if (ret)
		goto done;

	if (flag){
		status[CAMERA_1P8V].user++;
		status[CAMERA_2P8V].user++;
	} else
		if (user/2){
			status[CAMERA_1P8V].user--;
			status[CAMERA_2P8V].user--;
		}
done:
	mutex_unlock(&mutex_power);
	return ret;
}
EXPORT_SYMBOL_GPL(gc_camera_set_pmic);


static int camera_af_power_gpio = -1;

static int camera_af_power_ctrl(struct v4l2_subdev *sd, int flag)
{
	if (!INTEL_MID_BOARD(1, TABLET, BYT))
		return gpio_direction_output(camera_af_power_gpio, flag);
	else
		return 0;
}

const struct camera_af_platform_data *camera_get_af_platform_data(void)
{
	static const int GP_CORE = 96;
	static const int GPIO_DEFAULT = GP_CORE + 76;
	static const char gpio_name[] = "CAM_0_AF_EN";
	static const struct camera_af_platform_data platform_data = {
		.power_ctrl = camera_af_power_ctrl
	};
	int gpio, r;

	if (!INTEL_MID_BOARD(1, TABLET, BYT) && camera_af_power_gpio == -1) {
		gpio = get_gpio_by_name(gpio_name);
		if (gpio < 0) {
			pr_err("%s: can't find gpio `%s',default\n",
						__func__, gpio_name);
			gpio = GPIO_DEFAULT;
		}
		pr_info("camera pdata: af gpio: %d\n", gpio);
		r = gpio_request(gpio, gpio_name);
		if (r)
			return NULL;
		r = gpio_direction_output(gpio, 0);
		if (r)
			return NULL;
		camera_af_power_gpio = gpio;
	}

	return &platform_data;
}
EXPORT_SYMBOL_GPL(camera_get_af_platform_data);

#ifdef CONFIG_INTEL_SCU_IPC_UTIL
/*
 *  Simple power management is needed since camera sensors
 *  share the same power rail. When 2 sensors are working simulatenously,
 *  the power rail should be off after all callers stop.
 */

static int camera_set_vprog3(bool flag, enum camera_vprog_voltage voltage)
{
	/*
	 * Currently it is not possible to control the voltage outside of
	 * intel_scu_ipcut so have to do it manually here
	 */
#define MSIC_VPROG3_MRFLD_CTRL		0xae
#define MSIC_VPROG3_MRFLD_ON_1_05	0x01	/* 1.05V and Auto mode */
#define MSIC_VPROG3_MRFLD_ON_1_83	0x41	/* 1.83V and Auto mode */
#define MSIC_VPROG_MRFLD_OFF		0	/* OFF */

	if (intel_mid_identify_cpu() == INTEL_MID_CPU_CHIP_ANNIEDALE) {
		if (voltage == CAMERA_1_05_VOLT) {
			return intel_scu_ipc_iowrite8(MSIC_VPROG3_MRFLD_CTRL,
			       flag ? MSIC_VPROG3_MRFLD_ON_1_05 :
			       MSIC_VPROG_MRFLD_OFF);
		} else if (voltage == CAMERA_1_83_VOLT) {
			return intel_scu_ipc_iowrite8(MSIC_VPROG3_MRFLD_CTRL,
			       flag ? MSIC_VPROG3_MRFLD_ON_1_83 :
			       MSIC_VPROG_MRFLD_OFF);
		} else {
			pr_err("Error: Unsupported vprog3 voltage\n");
			return -ENODEV;
		}
	} else {
		pr_err("Error: vprog3 not supported\n");
		return -ENODEV;
	}
}

int camera_set_vprog_power(enum camera_vprog vprog, bool flag,
			   enum camera_vprog_voltage voltage)
{
	static struct vprog_status status[CAMERA_VPROG_NUM];
	static DEFINE_MUTEX(mutex_power);
	int ret = 0;

	if (vprog >= CAMERA_VPROG_NUM) {
		pr_err("%s: invalid vprog number: %d\n", __func__, vprog);
		return -EINVAL;
	}

	mutex_lock(&mutex_power);
	/*
	 * only set power at: first to power on last to power off
	 */
	if ((flag && status[vprog].user == 0)
	    || (!flag && status[vprog].user == 1)) {
		switch (vprog) {
		case CAMERA_VPROG1:
			if (voltage == DEFAULT_VOLTAGE) {
				ret = intel_scu_ipc_msic_vprog1(flag);
			} else {
				pr_err("Error: non-default vprog1 voltage\n");
				ret = -EINVAL;
			}
			break;
		case CAMERA_VPROG2:
			if (voltage == DEFAULT_VOLTAGE) {
				ret = intel_scu_ipc_msic_vprog2(flag);
			} else {
				pr_err("Error: non-default vprog2 voltage\n");
				ret = -EINVAL;
			}
			break;
		case CAMERA_VPROG3:
			ret = camera_set_vprog3(flag, voltage);
			break;
		default:
			pr_err("camera set vprog power: invalid pin number.\n");
			ret = -EINVAL;
		}
		if (ret)
			goto done;
	}

	if (flag)
		status[vprog].user++;
	else
		if (status[vprog].user)
			status[vprog].user--;
done:
	mutex_unlock(&mutex_power);
	return ret;
}
EXPORT_SYMBOL_GPL(camera_set_vprog_power);
#endif /* CONFIG_INTEL_SCU_IPC_UTIL */

#define HEXPREF(x)	((x) != 0 ? "0x" : "")
#define HEX(x)		HEXPREF(x), (x)

char *camera_get_msr_filename(char *buf, int buf_size, char *sensor, int cam)
{
	snprintf(buf, buf_size, "%02d%s-%s%x-%s%x-%s%x.drvb",
		 cam, sensor, HEX(spid.vendor_id),
		 HEX(spid.platform_family_id), HEX(spid.product_line_id));
	return buf;
}

#ifdef CONFIG_CRYSTAL_COVE


/*
 * WA for BTY as simple VRF management
 */
int camera_set_pmic_power(enum camera_pmic_pin pin, bool flag)
{
	/*MRD7 0x1A:default0x00-11=1.8v; 0x28:default0x17, =21=2.8?*/
        /*MRD7 0x12, bit4; 0x13, bit5*/
	//u8 reg_addr[CAMERA_POWER_NUM] = {VPROG_1P8V, VPROG_2P8V};
	//u8 reg_value[2] = {VPROG_DISABLE, VPROG_ENABLE};
	u8 reg_addr[CAMERA_POWER_NUM] = {0x12, 0x13};
	u8 reg_value[2] = {0x2, 0x20};
	u8 reg_adjust_addr[2] = {0x1A, 0x28};
	u8 reg_adjust_value[2] = {22, 21}; /* 1.8v for ELDO2, 2.8v for ALDO1 */

//	static struct vprog_status status[CAMERA_POWER_NUM];
	static DEFINE_MUTEX(mutex_power);
	int ret = 0;

	mutex_lock(&mutex_power);
	/*
	 * only set power at:
	 * first to power on
	 * last to power off
	 */
	if ((flag && status[pin].user == 0)
	    || (!flag && status[pin].user == 1))
//		ret = intel_mid_pmic_writeb(reg_addr[pin], reg_value[flag]);
	{
		if (flag)
		{
			ret = intel_mid_pmic_writeb(reg_adjust_addr[pin], reg_adjust_value[pin]);
			ret |= intel_mid_pmic_setb(reg_addr[pin], reg_value[pin]);
		}
		else
		{
			ret |= intel_mid_pmic_clearb(reg_addr[pin], reg_value[pin]);
		}
	}



	/* no update counter if config failed */
	if (ret)
		goto done;

	if (flag)
		status[pin].user++;
	else
		if (status[pin].user)
			status[pin].user--;
done:
	mutex_unlock(&mutex_power);
	return ret;
}
EXPORT_SYMBOL_GPL(camera_set_pmic_power);
#endif


#ifdef CONFIG_ACPI
void __init camera_init_device(void)
{
	struct camera_device_table *table = NULL;
	int entry_num = 0;
#ifndef CONFIG_VIDEO_PIXTER
	if (INTEL_MID_BOARD(1, TABLET, BYT)) {
		if (spid.hardware_id == BYT_TABLET_BLK_8PR0 ||
		    spid.hardware_id == BYT_TABLET_BLK_8PR1) {
			table = byt_ffrd8_cam_table;
			entry_num = ARRAY_SIZE(byt_ffrd8_cam_table);
		} else if (spid.hardware_id == BYT_TABLET_BLK_CRV2) {
			table = byt_crv2_cam_table;
			entry_num = ARRAY_SIZE(byt_crv2_cam_table);
		} else {
			table = byt_ffrd10_cam_table;
			entry_num = ARRAY_SIZE(byt_ffrd10_cam_table);
		}
	} else if (INTEL_MID_BOARD(1, TABLET, CHT) ||
		   INTEL_MID_BOARD(1, PHONE, CHT)) {
		int fw_type = m10mo_platform_identify_fw();
		if (fw_type != -1) {
			table = cht_somc_cam_table;
			entry_num = ARRAY_SIZE(cht_somc_cam_table);
			pr_info("M10MO for SOMC is detected.\n");
		} else {
			if (spid.hardware_id == CHT_TABLET_RVP1 ||
			    spid.hardware_id == CHT_TABLET_RVP2 ||
			    spid.hardware_id == CHT_TABLET_RVP3 ||
			    spid.hardware_id == CHT_TABLET_FRD_PR0 ||
			    spid.hardware_id == CHT_TABLET_FRD_PR1 ||
			    spid.hardware_id == CHT_TABLET_FRD_PR2) {
				table = cht_ffd_cam_table;
				entry_num = ARRAY_SIZE(cht_ffd_cam_table);
			} else
				pr_warn("unknown CHT platform variant.\n");
		}
	}
#else
	table = pixter_cam_table;
	entry_num = ARRAY_SIZE(pixter_cam_table);
#endif
	atomisp_register_device_table(entry_num, table);
}
device_initcall(camera_init_device);
#endif
