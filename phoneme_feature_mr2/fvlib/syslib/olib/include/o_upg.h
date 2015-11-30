#ifndef __O_UPG_H__
#define __O_UPG_H__

#define O_UPG_MAX_NAME_SIZE 256

typedef int (*get_upg_partition_func)(void *param1, void *param2);
typedef fboolean (*check_partition_func)(void *param1, void *param2);
typedef fboolean (*if_need_upgrade_func)(void *param);
typedef int (*flash_erase_func)(void *param1, void *param2);
typedef int (*flash_write_func)(void *pram1, void *param2, void *param3, void *param4);

typedef struct _o_upg_part_tab_s {
	char name[O_UPG_MAX_NAME_SIZE];
	get_upg_partition_func get_upg_partition;
	check_partition_func check_partition;
	if_need_upgrade_func if_need_upgrade;
	flash_erase_func erase_func;
	flash_write_func write_func;
} o_upg_part_tab_s;

int o_upg_bootlogo(char *path, int file_size);
int o_upg_init_part_table(o_upg_part_tab_s *part_tab);
int o_upg_update_logo_flag();

#endif
