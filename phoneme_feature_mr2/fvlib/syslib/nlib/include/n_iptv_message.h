#ifndef __N_IPTV_MESSAGE_H__
#define __N_IPTV_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROLL_MSG_TYPE "0100"
#define OSD_MSG_TYPE  "02"
#define OSD_MSG_TYPE1 "0201"
#define OSD_MSG_TYPE2 "0202"
#define OSD_MSG_TYPE3 "0203"
#define OSD_MSG_TYPE4 "0204"
#define PAGE_MSG_TYPE "0300"

#define TVMSG_JUMP_CMD "/ipstb/bin/brows_dump -g"
#define TVMSG_UNZIP_CMD "/usr/bin/unzip -o"
#define TVMSG_MKDIR_CMD "/bin/mkdir -p"
#define TVMSG_RM_CMD "/bin/rm -rf"

#define TVMSG_OSD_BG_FILE_PATH "/ipstb/share/tvmsg_osd_bg.png"
#define TVMSG_OSD_JUMP_FILE_PATH "/ipstb/share/tvmsg_osd_jump.png"
#define TVMSG_OSD_CLOSE_FILE_PATH "/ipstb/share/tvmsg_osd_close.png"

#define TVMSG_RES_DESC_PATH "/var/res_desc"
#define TVMSG_RES_FILE_PATH "/var/res.zip"
#define TVMSG_RES_BASE_PATH "/var/tvmsg_res"
#define TVMSG_RES_CONNECT_TIMEOUT 15
#define TVMSG_RES_DOWNLOAD_TIMEOUT 300

#define TVMSG_ROLL_FONT_DEFAULT_COLORR 0
#define TVMSG_ROLL_FONT_DEFAULT_COLORG 0
#define TVMSG_ROLL_FONT_DEFAULT_COLORB 0
#define TVMSG_ROLL_BG_DEFAULT_COLORR 200
#define TVMSG_ROLL_BG_DEFAULT_COLORG 200
#define TVMSG_ROLL_BG_DEFAULT_COLORB 200

#define TVMSG_OSD_BG_DEFAULT_COLORR 108
#define TVMSG_OSD_BG_DEFAULT_COLORG 158
#define TVMSG_OSD_BG_DEFAULT_COLORB 175

#define TVMSG_SCREEN_BORDER 0

#define TVMSG_ROLL_INTERVAL 500

/* osd templates */
typedef enum
{
	/* standard ctc template */
	TVMSG_OSD_TEMP1 = 1,
	TVMSG_OSD_TEMP2,
	TVMSG_OSD_TEMP3,
	TVMSG_OSD_TEMP_MAX,
	/* for yueqing hotel boot hello */
	TVMSG_OSD_FV_TEMP1 = 10000,
	TVMSG_OSD_FV_TEMP_MAX
} tvmsg_osd_template_e;

/* roll places */
typedef enum {
	TVMSG_ROLL_PLACE_UP = 2,
	TVMSG_ROLL_PLACE_LEFT = 4,
	TVMSG_ROLL_PLACE_RIGHT = 6,
	TVMSG_ROLL_PLACE_DOWN = 8
} tvmsg_roll_place_type_e;

/* roll direction */
typedef enum {
	TVMSG_ROLL_DIRECTION_LR = 1,
	TVMSG_ROLL_DIRECTION_RL = 2,
	TVMSG_ROLL_DIRECTION_UD = 3,
	TVMSG_ROLL_DIRECTION_DU = 4
} tvmsg_roll_direction_type_e;

/* roll speeds */
typedef enum {
	TVMSG_ROLL_TIME_SLOW = 16,
	TVMSG_ROLL_TIME_NORMAL = 8,
	TVMSG_ROLL_TIME_FAST = 4
} tvmsg_roll_time_type_e;

/* resource return code */
typedef enum {
	TVMSG_RES_RETCODE_HAVE = 0,
	TVMSG_RES_RETCODE_HAVNT = 1,
	TVMSG_RES_RETCODE_ERROR = 2
} tvmsg_res_retcode_e;

int tvmsg_start();
int tvmsg_stop();
int n_send_ir_to_tvmsg(int key, int status);
fboolean n_is_osd_msg_wind_hide();
fboolean n_is_roll_msg_wind_hide();
int tvmsg_res_check_and_get(char *res_path);
int  brows_wgt_tvmsg_start(void *data);
int  brows_wgt_tvmsg_stop(void *data);
fboolean  brows_wgt_tvmsg_isshown(void *data);
fboolean  brows_wgt_tvmsg_onkey(void *data, int type, int key, int status, int param3, int param4);

#ifdef __cplusplus
}
#endif

#endif



