#ifndef __O_XENV_H__
#define __O_XENV_H__

#define XENV_MACADDR_SAVELEN 17
#define XENV_STBID_SAVELEN 32
#define XENV_HARDVER_SAVELEN 32

#define XNEV_DEFAULT_BOOT                  "z.default_boot"
#define XENV_UPG_FLAG_ADDR                 "a.upgrade.flag"
#define XENV_UPG_KERNEL_ADDR               "a.upgrade.kernel"
#define XENV_UPG_SPLASH_ADDR               "a.upgrade.splash"
#define XENV_UPG_APP_ADDR                  "a.upgrade.app"
#define XENV_UPG_KERNEL_ADDR_BAK           "a.upgrade.kernel.bak"
#define XENV_UPG_SPLASH_ADDR_BAK           "a.upgrade.splash.bak"
#define XENV_UPG_APP_ADDR_BAK              "a.upgrade.app.bak"
#define XENV_STBID_ADDR                    "a.stbid"
#define XENV_MAC_ADDR                      "a.eth_mac"
#define XENV_HARDVER_ADDR                  "a.hardversion"

int o_xenv_read_value_from_file(char* value, int len);
int o_xenv_string2int(char* value);
int o_xenv_remove_tmpfile(char* filename);
int o_xenv_set_int(char* name, int value);
int o_xenv_get_int(char* name, int* value);
int o_xenv_set_string(char* name, char* value, int len);
int o_xenv_get_string(char* name, char* value, int len);

#endif
