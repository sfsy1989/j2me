#ifndef __FV_IR_H__
#define __FV_IR_H__

#if FVT_SDK(Hi3560E_V100R001C01SPC050) || FVT_SDK(Hi3560E_V100R001C01SPC080)
#include "hi_unf_ecs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define O_DEV_INVALID_IR_VALUE  0XFFFFFFFF
#define O_DEV_INVALID_FIP_VALUE  0XFFFFFFFF

typedef struct {
	fboolean check_up;
	int check_timeout;
	int check_count;
	int start_repeat_time;
	int repeat_time;
	int repeat_area;
} o_dev_ir_init_param_s;

typedef enum 
{
	O_DEV_IR_FIRST_KEY,
	O_DEV_IR_REPEAT_KEY,
	O_DEV_IR_RELEASE_KEY
} o_dev_ir_key_status;

int o_dev_ir_init(o_dev_ir_init_param_s *init_param);
int o_reg_repeat_key(unsigned int num_keys, unsigned long *keys);
void o_dev_ir_uninit();
int o_dev_ir_start();
int o_dev_ir_stop();
unsigned long long o_dev_ir_receive(o_dev_ir_key_status *press_status);

#ifdef __cplusplus
}
#endif

#endif

