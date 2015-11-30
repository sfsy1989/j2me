#ifndef __O_DEV_GPIO_H__
#define __O_DEV_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	O_LIB_LED_RED = 0,
	O_LIB_LED_GREEN,
	O_LIB_LED_YELLOW,
	O_LIB_LED_ON,
	O_LIB_LED_OFF
} o_led_color;

enum {
	O_LED_HW_TYPE_155
};

int o_dev_led_init();
void o_dev_led_uninit();
int o_dev_led_start();
int o_dev_led_stop();
int o_dev_led_set_net(int value);
int o_dev_led_set_powser(int value);
int o_dev_led_set_ir(int value);

#ifdef __cplusplus
}
#endif

#endif

