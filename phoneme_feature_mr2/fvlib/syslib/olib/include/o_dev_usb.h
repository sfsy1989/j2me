#ifndef __O_DEV_USB_H__
#define __O_DEV_USB_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	USB_MOUNT_OK = 1,
	USB_UMOUNT_OK
};

int o_dev_usb_init();
void o_dev_usb_uninit();
int o_dev_usb_start();
int o_dev_usb_stop();
int o_dev_usb_check_once(void *arg);

#ifdef __cplusplus
}
#endif

#endif

