/*
 * Copyright (c) 2016, OpenAV Productions,
 * Harry van Haaren <harryhaaren@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OPENAV_CTLRA_DEVICE_IMPL
#define OPENAV_CTLRA_DEVICE_IMPL

#include "ctlra.h"
#include "event.h"

struct ctlra_dev_t;

/* Functions each device must implement */
typedef uint32_t (*ctlra_dev_impl_poll)(struct ctlra_dev_t *dev);
typedef int32_t (*ctlra_dev_impl_disconnect)(struct ctlra_dev_t *dev);
typedef void (*ctlra_dev_impl_light_set)(struct ctlra_dev_t *dev,
					   uint32_t light_id,
					   uint32_t light_status);
typedef void (*ctlra_dev_impl_light_flush)(struct ctlra_dev_t *dev,
					  uint32_t force);
typedef int32_t (*ctlra_dev_impl_grid_light_set)(struct ctlra_dev_t *dev,
						uint32_t grid_id,
						uint32_t light_id,
						uint32_t light_status);
typedef const char* (*ctlra_dev_impl_control_get_name)
						(struct ctlra_dev_t *dev,
						uint32_t control_id);

#define CTLRA_USB_IFACE_PER_DEV 2

struct ctlra_dev_t {
	/* Static Device Info  */
	int vendor_id;
	int product_id;
	int class_id;

	/* usb handle for this hardware device. An array of them is
	 * available as certain complex controllers require more than one
	 * usb interface to be fully controlled (typically screen/buttons
	 * are on bulk/interrupt interfaces). The controller is responsible
	 * for providing the correct interface_id to the usb_read/write()
	 * functions */
	void *hidapi_usb_handle[CTLRA_USB_IFACE_PER_DEV];

	/* Event callback function */
	ctlra_event_func event_func;
	void* event_func_userdata;

	/* Function pointers to poll events from device */
	ctlra_dev_impl_poll poll;
	ctlra_dev_impl_disconnect disconnect;

	/* Function pointers to write feedback to device */
	ctlra_dev_impl_light_set light_set;
	ctlra_dev_impl_grid_light_set grid_light_set;
	ctlra_dev_impl_light_flush light_flush;

	/* Function pointer to retrive info about a particular control */
	ctlra_dev_impl_control_get_name control_get_name;

	/* Internal representation of the controller info */
	struct ctlra_dev_info_t info;
};

/** Connect function to instantiate a dev from the driver */
typedef struct ctlra_dev_t *(*ctlra_dev_connect_func)(ctlra_event_func event_func,
						    void *userdata,
						    void *future);

/** Opens the libusb handle for the given vid:pid pair, claiming the given
 * interface number. The implementation skips the first *num_skip* entries,
 * to allow opening the N(th) of a single type of controller.
 * Implementation in usb.c.
 * @retval 0 on Success
 * @retval -ENODEV when device not found */
int ctlra_dev_impl_usb_open(struct ctlra_dev_t *dev,
			   int vid,
			   int pid,
			   int interface,
			   uint32_t handle_idx);

/** Read bytes from the usb device, this is a non-blocking function but
 * _not_ realtime safe function. It polls the usb handle specified by *idx*
 * of the device *dev*, reading bytes up to *size* into the buffer pointed
 * to by *data*. */
int ctlra_dev_impl_usb_read(struct ctlra_dev_t *dev, uint32_t idx,
			   uint8_t *data, uint32_t size);


int ctlra_dev_impl_usb_write(struct ctlra_dev_t *dev, uint32_t idx,
			    uint8_t *data, uint32_t size);

/** Xfer bytes to a specific handle and endpoint. Some complex USB HID
 * controllers have multiple Interfaces (selected by the handle_idx), and
 * more than one *endpoint* per Interface. Reading or Writing is chosen
 * based on the endpoint type - in or out (from the host POV).
 * Eg: Endpoing 0x81 is in (so the host recieves data from it)
 *     Performing a xfer() with endpoint 0x81 causes a read of the device.
 * @retval 0 No bytes transferred
 * @retval positive Amount of bytes xfered to/from endpoint
 * @retval negative Error occurred in transfer.
 */
int ctlra_dev_impl_usb_xfer(struct ctlra_dev_t *dev, int handle_idx,
			   int endpoint, uint8_t *data, uint32_t size);

/** Close the USB device handles, returning them to the kernel */
void ctlra_dev_impl_usb_close(struct ctlra_dev_t *dev);

/* IMPLEMENTATION DETAILS ONLY BELOW HERE */

/* Macro extern declaration for the connect function */
#define DECLARE_DEV_CONNECT_FUNC(name)					\
extern struct ctlra_dev_t *name(ctlra_event_func event_func,		\
			    void *userdata, void *future)

#endif /* OPENAV_CTLRA_DEVICE_IMPL */

