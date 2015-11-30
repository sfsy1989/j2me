#ifndef __O_BOARD_H__
#define __O_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	O_BOARD_NULL = 0,
	O_BOARD_150,
	O_BOARD_152,
	O_BOARD_154,
	O_BOARD_155
};

typedef struct {
	int board_type;
} o_board_init_param_s;

int o_board_init(o_board_init_param_s *init_param);
void o_board_uninit();
fboolean o_board_is_init();
int o_board_get_type();
int o_board_reboot();

#include "o_board_standby.h"

#ifdef __cplusplus
}
#endif

#endif

