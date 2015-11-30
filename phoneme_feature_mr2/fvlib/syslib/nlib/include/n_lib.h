#ifndef __N_LIB_H__
#define __N_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "f_lib.h"
#include "o_lib.h"

#include "n_limit.h"
#include "n_stb.h"
#include "n_iptv.h"

typedef int (*n_reload_config_func)();

typedef struct {
	n_reload_config_func	reload_cfg_func;
} n_lib_callback_s;

int n_lib_init(char* name, n_lib_callback_s *callback);
void n_lib_uninit();
int n_lib_thread_init();
int n_lib_thread_uninit();
int n_lib_regist_signal();

#ifdef __cplusplus
}
#endif

#endif
