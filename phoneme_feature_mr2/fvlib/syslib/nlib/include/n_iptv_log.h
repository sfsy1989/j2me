#ifndef __F_IPTV_LOG_H__
#define __F_IPTV_LOG_H__

typedef enum {
	N_IPTVLOG_MOD_BROWS = 0,
	N_IPTVLOG_MOD_MP
} n_iptv_log_mod;

typedef enum {
	N_IPTVLOG_ERR_AUTH = 0,
	N_IPTVLOG_ERR_WEB,
	N_IPTVLOG_ERR_RTSP,
	N_IPTVLOG_ERR_IGMP,
} n_iptv_log_err_type;

char* n_iptv_log_timestamp();
void n_iptv_log(f_iptv_event_type event, f_iptv_fault_level level,
                char *info, f_stb_error_code errorcode);
void n_iptv_reset_statis_upload();
int n_iptv_upload_statis_now(char *upload_url);
int n_iptv_log_add_info(n_iptv_log_mod mod, n_iptv_log_err_type err_type,
                        char *url, int errcode);
int n_iptv_upload_prod_csv_now(char *filename);
void n_iptv_reset_logmsg_upload(int op);
#endif

