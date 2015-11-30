#ifndef __F_STB_OSD__
#define __F_STB_OSD__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	OSD_OP_UNSHOW = 0,
	OSD_OP_SHOW = 1
} n_osd_op_e;

typedef enum {
	N_OSD_PT_NULL = 0,
	N_OSD_PT_UPGRADE_DOWNLOAD = 1,
	N_OSD_PT_UPGRADE_UNTAR    = 2,
	N_OSD_PT_UPGRADE_ERASE    = 3,
	N_OSD_PT_UPGRADE_WRITE    = 4,
	N_OSD_PT_UPGRADE_FINISH   = 5,
	N_OSD_PT_UPGRADE_NEWEST   = 6,
	N_OSD_PT_UPGRADE_FAIL     = 7,
	N_OSD_PT_REBOOT           = 8,
	N_OSD_PT_RESET            = 9,
	N_OSD_PT_MODIFY_PARAS     = 10,
	N_OSD_PT_UPGRADE_CHECK    = 11,
	N_OSD_PT_MAX
} n_osd_prompt_e;


#define cmd_osd_Control	    "osd.Control"
#define cmd_osd_Playstatus  "osd.Playstatus"
#define cmd_osd_Channel     "osd.Channel"
#define cmd_osd_Playproc    "osd.Playproc"
#define cmd_osd_Volume       "osd.Volume"
#define cmd_osd_Net          "osd.Net"
#define cmd_osd_Stream       "osd.Stream"
#define cmd_osd_Startbar     "osd.Startbar"
#define cmd_osd_Prompt       "osd.Prompt"
#define cmd_osd_Chanlogo     "osd.Chanlogo"
#define cmd_osd_Pageloading  "osd.Pageloading"
#define cmd_osd_Upgtimer     "osd.Upgtimer"
#define cmd_osd_Splashscreen "osd.Splashscreen"

int n_osd_show_playstatus(unsigned int op, const char *status);
int n_osd_show_channel(unsigned int op, unsigned int chno);
int n_osd_show_playproc(unsigned int op,
                 const char *start, const char *cur, const char *end,
                 unsigned int pos, unsigned int sel);
int n_osd_show_vol(unsigned int op, unsigned int val);
int n_osd_show_stream_stat(unsigned int op, unsigned int status);
int n_osd_show_net_stat(unsigned int op, unsigned int status);
int n_osd_show_startbar(unsigned int op, unsigned int pos, const char *msg);
int n_osd_show_prompt(unsigned int op, int status);
int n_osd_show_chan_logo(unsigned int op, const char* logo_url);
int n_osd_show_loading(n_osd_op_e op, int delay, int interval);
int n_osd_show_timer(n_osd_op_e op);
int n_osd_show_splash_screen(n_osd_op_e op);

#ifdef __cplusplus
}
#endif

#endif
