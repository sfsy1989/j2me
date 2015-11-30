#ifndef __F_STB_CFG__
#define __F_STB_CFG__

#define CFG_SERVER_PORT   	1107

#define F_CMD_CFG_GET_INT	"F_CMD_CFG_GET_INT"
#define F_CMD_CFG_GET_STR	"F_CMD_CFG_GET_STR"

int f_cfg_get_int(int id, int *val);
int f_cfg_get_str(int id, char *val, int size);

#endif
