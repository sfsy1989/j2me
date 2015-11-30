#ifndef __N_STB_MOD_H__
#define __N_STB_MOD_H__

typedef enum {
	N_MOD_MIN = -1,
	N_MOD_BROWS,
	N_MOD_MPLAYER,
	N_MOD_DEVICE,
	N_MOD_OSD,
	N_MOD_NET,
	N_MOD_IPTVLOGD,
	N_MOD_CPE,
	N_MOD_CFG,
	N_MOD_TVMSG,
	N_MOD_UPG,
	N_MOD_MAX
} n_mod_e;

#define F_STB_CTL_MOD N_MOD_BROWS ///< control module

typedef enum {
	N_MOD_NOT_INIT = 0,
	N_MOD_INIT,
	N_MOD_RUNNING,
	N_MOD_EXIT
} n_mod_stat_e;

int n_mod_set_stat(n_mod_stat_e stat);
n_mod_stat_e n_mod_get_stat(int mod_id);
n_mod_stat_e n_mod_stat();

#endif

