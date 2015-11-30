#ifndef __O_DEV_NET_H__
#define __O_DEV_NET_H__
#include "f_lib.h"

#if FVT_SDK(Hi3560E_V100R001C01SPC050) || FVT_SDK(Hi3560E_V100R001C01SPC080)
#define WIRELESS_TOOL_PATH "/ipstb/sbin"
#endif

#if FVT_SDK(SMP865x_3_11_1)
#define WIRELESS_TOOL_PATH "/ipstb/sbin"
#endif

#if FVT_SDK(MTK_8652)
#define WIRELESS_TOOL_PATH "/sbin"
#endif

#if FVT_SDK(MTK_8550)
#define WIRELESS_TOOL_PATH "/sbin"
#endif

int o_get_wireless_dev_name(char *dev_name, int len);
int o_dev_wireless_scanning(char *wireless_interface);
int o_dev_net_if_up(const char *net_if);
int o_dev_net_if_down(const char *net_if);

#endif
