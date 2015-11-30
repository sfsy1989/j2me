#ifndef __N_BOARD_CONFIG_H__
#define __N_BOARD_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	N_BCFG_MAC = 0,
	N_BCFG_STBID,
	N_BCFG_HW_VERSION,
	N_BCFG_UPGRADE_FLAG,
	N_BCFG_NORM,
	N_BCFG_LOGO_FLAG,
	N_BCFG_HDMI_MODE,
	N_BCFG_COMPONENT_MODE,
};

enum {
	N_BCFG_SAVE_FILE = 0,
	N_BCFG_SAVE_ENV
};

enum {
	N_BCFG_NORM_NTSC=0,
	N_BCFG_NORM_PAL
};

#define N_BOARD_ANALOG_INDEX_PAL          134 /* PAL_BG */
#define N_BOARD_ANALOG_INDEX_NTSC         123 /* NTSC_M */
#define N_BOARD_HDMI_INDEX_720p50         54
#define N_BOARD_HDMI_INDEX_1080i50        55
#define N_BOARD_HDMI_INDEX_1080p50        63
#define N_BOARD_HDMI_INDEX_720p59         33
#define N_BOARD_HDMI_INDEX_1080i59        35
#define N_BOARD_HDMI_INDEX_1080p59        51
#define N_BOARD_COMPONENT_INDEX_720p50    114
#define N_BOARD_COMPONENT_INDEX_1080i50   102
#define N_BOARD_COMPONENT_INDEX_1080p50   99
#define N_BOARD_COMPONENT_INDEX_720p59    113
#define N_BOARD_COMPONENT_INDEX_1080i59   101
#define N_BOARD_COMPONENT_INDEX_1080p59   98

int n_bcfg_init();
int n_bcfg_get_int(int id, int *rval);
int n_bcfg_set_int(int id, int val);
int n_bcfg_get_string(int id, char *buf, int len);
int n_bcfg_set_string(int id, char *buf);
int n_bcfg_save(int mode);

#ifdef __cplusplus
}
#endif

#endif

