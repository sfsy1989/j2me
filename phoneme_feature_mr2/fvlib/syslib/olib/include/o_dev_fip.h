#ifndef __O_DEV_FIP_H__
#define __O_DEV_FIP_H__

#ifdef __cplusplus
extern "C" {
#endif

int o_dev_fip_init();
void o_dev_fip_uninit();
int o_dev_fip_start();
int o_dev_fip_stop();
unsigned int o_dev_fip_check();

#ifdef __cplusplus
}
#endif

#endif
