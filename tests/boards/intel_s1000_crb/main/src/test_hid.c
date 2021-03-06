/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(main);

#include <zephyr/zephyr.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#define REPORT_ID_1	0x01
#define REPORT_ID_2	0x02

/* size of stack area used by each thread */
#define STACKSIZE               1024

/* scheduling priority used by each thread */
#define PRIORITY                7

/* Some HID sample Report Descriptor */
static const uint8_t hid_report_desc[] = {
	/* 0x05, 0x01,		USAGE_PAGE (Generic Desktop)		*/
	HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP,
	/* 0x09, 0x00,		USAGE (Undefined)			*/
	HID_LI_USAGE, USAGE_GEN_DESKTOP_UNDEFINED,
	/* 0xa1, 0x01,		COLLECTION (Application)		*/
	HID_MI_COLLECTION, COLLECTION_APPLICATION,
	/* 0x15, 0x00,			LOGICAL_MINIMUM one-byte (0)	*/
	HID_GI_LOGICAL_MIN(1), 0x00,
	/* 0x26, 0xff, 0x00,		LOGICAL_MAXIMUM two-bytes (255)	*/
	HID_GI_LOGICAL_MAX(2), 0xFF, 0x00,
	/* 0x85, 0x01,			REPORT_ID (1)			*/
	HID_GI_REPORT_ID, REPORT_ID_1,
	/* 0x75, 0x08,			REPORT_SIZE (8) in bits		*/
	HID_GI_REPORT_SIZE, 0x08,
	/* 0x95, 0x01,			REPORT_COUNT (1)		*/
	HID_GI_REPORT_COUNT, 0x01,
	/* 0x09, 0x00,			USAGE (Undefined)		*/
	HID_LI_USAGE, USAGE_GEN_DESKTOP_UNDEFINED,
	/* v0x81, 0x82,			INPUT (Data,Var,Abs,Vol)	*/
	HID_MI_INPUT, 0x82,
	/* 0x85, 0x02,			REPORT_ID (2)			*/
	HID_GI_REPORT_ID, REPORT_ID_2,
	/* 0x75, 0x08,			REPORT_SIZE (8) in bits		*/
	HID_GI_REPORT_SIZE, 0x08,
	/* 0x95, 0x01,			REPORT_COUNT (1)		*/
	HID_GI_REPORT_COUNT, 0x01,
	/* 0x09, 0x00,			USAGE (Undefined)		*/
	HID_LI_USAGE, USAGE_GEN_DESKTOP_UNDEFINED,
	/* 0x91, 0x82,			OUTPUT (Data,Var,Abs,Vol)	*/
	HID_MI_OUTPUT, 0x82,
	/* 0xc0			END_COLLECTION			*/
	HID_MI_COLLECTION_END,
};

void hid_thread(void)
{
	uint8_t report_1[2] = { REPORT_ID_1, 0x00 };
	const struct device *hid_dev;
	int ret, wrote;

	LOG_DBG("Starting application");

	hid_dev = device_get_binding("HID_0");
	if (hid_dev == NULL) {
		LOG_ERR("Cannot get USB HID Device");
		return;
	}

	usb_hid_register_device(hid_dev, hid_report_desc,
				sizeof(hid_report_desc), NULL);

	usb_hid_init(hid_dev);

	while (true) {

		k_sleep(K_SECONDS(1));

		report_1[1]++;

		ret = hid_int_ep_write(hid_dev, report_1, sizeof(report_1),
				       &wrote);
		LOG_DBG("Wrote %d bytes with ret %d", wrote, ret);
	}
}

K_THREAD_DEFINE(hid_thread_id, STACKSIZE, hid_thread, NULL, NULL, NULL,
		PRIORITY, 0, 0);
