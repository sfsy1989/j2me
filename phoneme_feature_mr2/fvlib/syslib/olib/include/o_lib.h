#ifndef __O_LIB_H__
#define __O_LIB_H__

#include "f_lib.h"

#if FVT_SDK(Hi3560E_V100R001C01SPC050) || FVT_SDK(Hi3560E_V100R001C01SPC080)
#include "o_uboot_env.h"
#include "o_flash_hg600a.h"
#endif

#if FVT_SDK(SMP865x_3_11_1)
#include "o_xenv.h"
#include "o_flash_hg602.h"
#endif

#if FVT_SDK(MTK_8652)
#include "o_flash_hg650o.h"
#endif

#if FVT_SDK(MTK_8550)
#include "o_flash_hc606.h"
#endif

#include "o_board.h"
#include "o_dev_fip.h"
#include "o_dev_led.h"
#include "o_dev_ir.h"
#include "o_dev_usb.h"
#include "o_display.h"
#include "o_osd.h"
#include "o_product.h"
#include "o_dev_net.h"
#include "o_soc_cap.h"
#include "o_upg.h"

#endif
