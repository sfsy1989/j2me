#ifndef __O_DEV_STANDBY_H__
#define __O_DEV_STANDBY_H__

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
	O_BOARD_STANDBY,
	O_BOARD_WAKEUP
};

typedef enum {
	O_STANDBY_TRUE,
	O_STANDBY_FALSE,
}o_standby_mode;

int o_board_set_standby_key(unsigned long long standby_key_value);
int o_board_standby(o_standby_mode mode);
int o_board_wakeup();
int o_board_get_standby_status();
int o_board_kill_ir_thread(pthread_t thread_id);

#ifdef __cplusplus
}
#endif

#endif

