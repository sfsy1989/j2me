#ifndef __O_ENV_H__
#define __O_ENV_H__

#define O_UBOOT_ENV_NORM           "norm"
#define O_UBOOT_ENV_UPGFLAG        "upgflag"
#define O_UBOOT_ENV_MACADDR        "ethaddr"
#define O_UBOOT_ENV_STBID          "stbid"
#define O_UBOOT_ENV_HWVER          "hwversion"
#define O_UBOOT_ENV_LOGOADDR       "logoaddr"

int o_uboot_env_init();
int o_uboot_env_get(char *key , char *value, int length);
int o_uboot_env_set(char *key, char *value);
int o_uboot_env_save();

#endif
