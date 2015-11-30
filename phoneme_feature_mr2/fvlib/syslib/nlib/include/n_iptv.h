#ifndef __N_IPTV_H__
#define __N_IPTV_H__

#include "f_lib.h"
#include "n_iptv_limit.h"
#include "n_iptv_chanlist.h"
#include "n_iptv_errorcode.h"
#include "n_iptv_statistics.h"
#include "n_iptv_mplayer.h"
#include "n_iptv_log.h"
#include "n_iptv_log_sqm.h"
#include "f_lib.h"

#define N_IPTV_OK    0

#define IPTV_MUTE_ENABLE    1
#define IPTV_MUTE_DISABLE   0

enum {
    DP_MIN = 0,
    DP_STB_STATUS,
    DP_EPG_CFG,
    DP_FVBROWSER_DBG,
    DP_FVBROWSER_IPTV,
    DP_FVBROWSER_SQM,
    DP_DEVICE_DBG,
    DP_DEVICE_IPTV,
    DP_OSD_DBG,
    DP_OSD_IPTV,
    DP_FVNET_DBG,
    DP_FVNET_IPTV,
    DP_MPLAYER_DBG,
    DP_MPLAYER_IPTV,
    DP_MPLAYER_SQM,
    DP_CPE_DBG,
    DP_CPE_IPTV,
    DP_IPTVLOGD_DBG,
    DP_IPTVLOGD_IPTV,
    DP_SOUND_BUF1,
    DP_SOUND_BUF_CTL1,
    DP_SOUND_BUF2,
    DP_SOUND_BUF_CTL2,
    DP_MPLAYER_RAPID_CMD,
    DP_IPTV_CHANNEL,
    DP_MAX
};

enum {
	N_IPTVS_NULL = -1,
    N_IPTVS_DEFAULT = 0,
	N_IPTVS_CTC_2_2,
    N_IPTVS_CTC_3_0,
	N_IPTVS_CU_2_2,
	N_IPTVS_CU_3_0,
    N_IPTVS_2_2_HARB,
    N_IPTVS_2_2_YUEQING,
    N_IPTVS_GUANGDONG
};

enum {
    N_IPTVS_CU,
    N_IPTVS_CTC
};

int n_iptv_stand();
char *n_iptv_stand_s();
int n_iptv_standard();
fboolean n_iptv_stand_matching(char *stand);
fboolean n_iptv_proc_volume_in_stb();
fboolean n_iptv_no_page_volume_bar();
fboolean n_iptv_save_volume();
fboolean n_iptv_proc_back_in_trickmode();
fboolean n_iptv_check_hw_keytable();


#endif

