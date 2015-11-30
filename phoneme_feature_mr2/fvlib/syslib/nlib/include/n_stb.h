#ifndef __F_STB_H__
#define __F_STB_H__

#include "f_lib.h"

#define N_STB_PARAM_PATH "/data/iptv/paras"
#define N_FIXED_CONFIG_PATH PREFIX "/etc"

#include "n_stb_limit.h"
#include "n_stb_device.h"
#include "n_stb_errorcode.h"
#include "n_stb_event.h"
#include "n_stb_hw.h"
#include "n_stb_tr069.h"
#include "n_stb_upgrade.h"
#include "n_log.h"
#include "n_stb_default.h"

#include "n_stb_mod.h"
#include "n_stb_signal.h"
#include "n_stb_service.h"

#include "n_stb_brows.h"
#include "n_stb_dev.h"
#include "n_stb_display.h"
#include "n_stb_osd.h"
#include "n_stb_mplayer.h"
#include "n_stb_net.h"
#include "n_stb_cpe.h"
#include "n_stb_ilogd.h"
#include "n_stb_cfg.h"
#include "n_board_config.h"
#include "n_osd.h"

#define BROWSER_SERVER_PORT     11101
#define CFG_SERVER_PORT         11102
#define TR069_SERVER_PORT       11103
#define DEVICE_SERVER_PORT      11104
#define IPTVLOGD_SERVER_PORT    11105
#define MP_SERVER_PORT          11106
#define NET_SERVER_PORT         11107
#define OSD_SERVER_PORT         11108
#define UPG_SERVER_PORT         11109

#ifdef __cplusplus
extern "C" {
#endif

/*
 * config tools
 */
 typedef enum{
    LED_IR = 0,
    LED_PW,
    LED_LK
}f_led_id;

int n_check_stbid(char *stbid,int len);
int n_check_version(char *version,int len);
int n_check_mac(char *mac,int len);
int n_check_upUrl(char * upUrl, int len);

/*
 * mplayer rapid cmdif
 */
struct f_mp_ins {
	int id;
	unsigned int playtime;
};
typedef struct {
    int volume;
    struct f_mp_ins ins[5];
} f_mp_rapid_cmd;
/*
 * jvm sound
 */
#define IPANEL_SND_BUF_NUM    (8)
#define IPANEL_SND_BUF_SIZE   (4 * 1024)

#define BROWS_SND_BUF_NUM           (2 * IPANEL_SND_BUF_NUM)
#define BROWS_SND_BUF_SIZE          (IPANEL_SND_BUF_SIZE)
#define BROWS_SND_RING_BUF_SIZE    (BROWS_SND_BUF_NUM * BROWS_SND_BUF_SIZE)

#define BROWS_MAX_SND_INS   2

typedef enum {
    BROWS_SND_PCM = 1,
    BROWS_SND_MP3 = 2,
    BROWS_SND_WMA = 3,
} brows_snd_fmt_e;

typedef struct {
    unsigned int read_p;
    unsigned int write_p;
} f_sound_buf_ctl;

//#ifdef DMALLOC
//#include"dmalloc.h"
//#endif

/*upnp action name*/
#define SETAVTRANSPORTURI "SetAVTransportURI"
#define GETMEDIAINFO "GetMediaInfo"
#define GETTRANSPORTINFO "GetTransportInfo"
#define GETDEVICECAPABILITIES "GetDeviceCapabilities"
#define GETTRANSPORTSETTTINGS "GetTransportSettings"
#define STOP "Stop"
#define PLAY "Play"
#define SEEK "Seek"
#define NEXT "Next"
#define PREVIOUS "Previous"
#define GETPROTOCOLINFO "GetProtocolInfo"
#define PREPAREFORCONNECTION "PrepareForConnection"
#define CONNECTIONCOMPLETE "ConnectionComplete"
#define GETCURRENTCONNECTIONIDS "GetCurrentConnectionIDs"
#define GETCURRENTCONNECTIONINFO "GetCurrentConnectionInfo"
#define LISTPRESETS "ListPresets"
#define SELECTPRESET "SelectPreset"
#define GETMUTE "GetMute"
#define GETVOLUME "GetVolume"
#define SETVOLUME "SetVolume"
#define GETVOLUMEDB "GetVolumeDB"
#define SETVOLUMEDB "SetVolumeDB"
#define GETVOLUMEDBRANGE "GetVolumeDBRange"
#define GETSTATEVARIABLES "GetStateVariables"
#define SETSTATEVARIABLES "SetStateVariables"

/*flash dump*/
#define FV_BASICFS_VERSION_FILE "/ipstb/etc/app_version"
#define FV_IPSTB_VERSION_FILE  "/ipstb/etc/ipstb_version"

#if defined FV_THREAD_MODEL
void mplayer_thread();
void      n_set_disp_hdl(HI_HANDLE hdl);
HI_HANDLE n_get_disp_hdl();
#endif

int n_brows_msg_evt(int type, int param1, int param2, int param3, int param4);
int n_brows_open_url(char* url);
int n_brows_clear_cache();
int n_brows_msg(int type, char* buf, int len);
int n_brows_enable_msg_evt(fboolean enable);
int n_brows_tr069_upgrade(char *upg_url, char *upg_filename);
int n_brows_jvm_exit(int exit_status);
int n_brows_upnp_action(char *action_name, char *param1, char *param2, char *param3);
int n_device_reboot();
int n_device_ir_lock();
int n_device_logout_respond(char *config_arg, int *config_flag);
int n_device_net_led(int config_type,int config_arg, int *config_flag);
int n_device_ir_unlock();
int n_device_emit_irkey_signal(int fakeirkey);
int n_device_usb_start();
int n_device_usb_stop();
int n_device_standby();

int tr069_format_time(time_t now_time, char *tr069_time, int size);

//int f_reset_iptvlod_paras();

typedef struct {
	fboolean ntp_done;
	fboolean net_status;
	fboolean excl_flag;
	n_mod_stat_e mod_stat[N_MOD_MAX];
	char tr069_session[F_TR069_SESSION_ID_SIZE];
} f_stb_status_s;

f_stb_status_s *n_get_stb_stat();
int n_stb_stat_init();

const char *n_stb_get_lang();
void n_osd_get_common_pic_path(char *pic_path, int osd_w, int osd_h);
void n_osd_get_lang_pic_path(char *pic_path, int osd_w, int osd_h);

int n_stb_get_screen_width();
int n_stb_get_screen_height();
char* n_stb_get_factory_test_page();

#ifdef __cplusplus
}
#endif

#endif

