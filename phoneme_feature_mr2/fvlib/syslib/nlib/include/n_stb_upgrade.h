#ifndef __N_STB_UPGRADE_H__
#define __N_STB_UPGRADE_H__

typedef enum {
    UPGRADE_MODE_3A = 0,
    UPGRADE_MODE_LOCAL,
    UPGRADE_MODE_HTTPD,
    UPGRADE_MODE_TR069,
    UPGRADE_MODE_USB,
    UPGRADE_MODE_Urgent,
} n_lib_upgrade_mode;

typedef enum {
    UPGRADE_STAT_SUCCESS = 0,
    UPGRADE_STAT_FAIL,
    UPGRADE_STAT_NEWSKIP,
} n_lib_upgrade_stat;

void n_upgrade_mount_tmpfs();
void n_upgrade_umount_tmpfs();
int n_upgrade_redirect(char * domain);
int n_upgrade_for_tr069(char *upg_url, char *upg_filename);
int n_upgrade_direct(char *upg_url, char *upg_filename);

#endif

