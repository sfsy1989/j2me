#ifndef __N_IPTV_LOG_SQM_H__
#define __N_IPTV_LOG_SQM_H__
#include <stdint.h>

typedef enum {
	N_IPTV_SQM_DISABLE = 0,
	N_IPTV_SQM_ENABLE = 1
} n_iptv_sqm_op;

typedef enum {
	N_IPTV_SQM_EPG_PORTAL = 0,
	N_IPTV_SQM_EPG_PAGE_SWITCH,
	N_IPTV_SQM_MP_STATE_CHANGE,
	N_IPTV_SQM_MP_QUALITY_STATUS,
	N_IPTV_SQM_MP_QUALITY_STATUS_BEC,
	N_IPTV_SQM_WARNING,
} n_iptv_sqm_type;

typedef enum {
	N_IPTV_SQM_WARN_RTSP_FAILED = 100,
	N_IPTV_SQM_WARN_JOIN_CHAN_FAILED,
	N_IPTV_SQM_WARN_STREAM_OFF,
	N_IPTV_SQM_WARN_EPG_CANNOT_ACCESS = 200,
	N_IPTV_SQM_WARN_EPG_SWITCH_MORE_THAN_4S
} n_iptv_sqm_warn_code;

typedef enum {
	N_IPTV_SQM_MP_STOP = 0,
	N_IPTV_SQM_MP_PLAY,
	N_IPTV_SQM_MP_PAUSE,
	N_IPTV_SQM_MP_SEEK = 4
} n_iptv_sqm_mp_state;

/**SQM*/
typedef struct {
	char portal[F_MAX_URL_SIZE];
	char prev_url[F_MAX_URL_SIZE];
	char cur_url[F_MAX_URL_SIZE];
	int http_code;
	int switch_time;
} n_iptv_sqm_epg_msg;

typedef struct {
	int state;
	char start_time[32];
	int play_duration;
	int interval;
	char url[F_MAX_URL_SIZE];
	int type;
	int multicast_flag;
	int pack_type;
	char aud_codec[F_MAX_TYPE_SIZE];
	char vid_codec[F_MAX_TYPE_SIZE];
	uint64_t mr;                     /**bps*/
	float mos;
	float mos_min;
	int mos_num;
	float df;
	float mlr;
	float iplr;
	int tr_ts_sync_loss;
	int tr_sync_byte_loss;
	int tr_cont_count_err;
	int tr_pts_err;
} n_iptv_sqm_media_msg;

int n_iptv_log_sqm_ctrl(int op);
int n_iptv_log_sqm_repo(n_iptv_sqm_type type, n_iptv_sqm_warn_code code);

#endif
