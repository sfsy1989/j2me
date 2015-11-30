#ifndef __O_DISPLAY_H__
#define __O_DISPLAY_H__

#define O_DISP_PAL_STR     "PAL"
#define O_DISP_NTSC_STR    "NTSC"
#define O_DISP_480I_STR    "480i"
#define O_DISP_576I_STR    "576i"
#define O_DISP_720P50_STR  "720p50"
#define O_DISP_720P60_STR  "720p60"
#define O_DISP_1080P50_STR "1080p50"
#define O_DISP_1080P60_STR "1080p60"

typedef enum _o_disp_sd_mode_e {
	O_DISP_MODE_PAL = 0,
	O_DISP_MODE_NTSC,
} o_disp_sd_mode_e;

typedef enum _o_disp_hd_mode_e {
	O_DISP_MODE_480I = 0,
	O_DISP_MODE_576I,
	O_DISP_MODE_720P50,
	O_DISP_MODE_720P60,
	O_DISP_MODE_1080P50,
	O_DISP_MODE_1080P60,
} o_disp_hd_mode_e;

typedef enum _o_disp_cap_s {
	O_DC_PAL = 0,
	O_DC_NTSC,
	O_DC_720P,
	O_DC_1080P,
	O_DC_NUM
} o_disp_cap_s;

typedef struct _o_disp_init_param_s {
	int fb_width;
	int fb_height;
	o_disp_sd_mode_e sd_mode;
	o_disp_hd_mode_e hd_mode;
} o_disp_init_param_s;

int o_disp_init(o_disp_init_param_s *init_param);
void o_disp_uninit();
int o_disp_cap_query(int pcap[]);
int o_disp_set_format(o_disp_sd_mode_e sd_mode, o_disp_hd_mode_e hd_mode);
int o_disp_get_handle();
int o_disp_fun_key_set_format(o_disp_sd_mode_e sd_mode, o_disp_hd_mode_e hd_mode);
fboolean o_disp_support_key_set_format();
int o_disp_cfg_mod_set_format(char *sd_mode, char *hd_mode);


#endif

