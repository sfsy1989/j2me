#ifndef __F_STB_SERVICE_H__
#define __F_STB_SERVICE_H__

#include "f_ipc.h"

#define cmd_set_log_level "FTM_SET_LOG_LEVEL"

typedef enum {
	F_MOD_CMD_MIN = -1,
	F_MOD_CMD_INIT,
	F_MOD_CMD_START,
	F_MOD_CMD_STOP,
	F_MOD_CMD_RESTART,
	F_MOD_CMD_EXIT,
	F_MOD_CMD_MAX
} f_mod_cmd_e;

typedef int (*f_mod_init_func)(void *arg);
typedef int (*f_mod_start_func)(void *arg);
typedef int (*f_mod_stop_func)(void *arg);
typedef int (*f_mod_restart_func)(void *arg);
typedef int (*f_mod_exit_func)(void *arg);

typedef struct {
	f_mod_init_func		mod_init;
	f_mod_start_func	mod_start;
	f_mod_stop_func		mod_stop;
	f_mod_restart_func	mod_restart;
	f_mod_exit_func		mod_exit;
} f_mod_service_callback_s;

typedef struct {
    int  port;
} f_ipc_cfg_s;

int n_svr_get_port(int id);
int n_fvipc_init();
ipc_service* n_init_service0(int id, int local_only);
ipc_service* n_init_service(int id, f_mod_service_callback_s *callback, int local_only);
ipc_service* n_get_service();
int n_get_mod_id();
int n_fvipc_start(int wait_ms);
int n_fvipc_oneshot();
int n_fvipc_block();
int n_fvipc_unblock();
int n_fvipc_stop();
int n_stb_module_control(int mod_id, int status);
int n_module_stop(n_mod_e module_name, int kill_flag);
int n_module_start(n_mod_e module_name);
int n_check_waitms();

#endif

