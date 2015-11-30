#ifndef __N_STB_SIGNAL__
#define __N_STB_SIGNAL__

#include "n_stb_mod.h"

int n_sig_init();
int n_sig_reg(char *signal_name);
int n_sig_add_listener(n_mod_e mod_id, char *sig_name);
int n_sig_reg_listener(n_mod_e mod_id, char *sig_name, ipc_func if_callback, int *ret);
int n_sig_send(int sig_id, ipc_valueset *arg);

#endif

