#ifndef __N_STB_DISPLAY_H__
#define __N_STB_DISPLAY_H__

#include "f_lib.h"

typedef struct {
	fboolean fullscreen_refresh;
	fboolean sync_output;
	int deflicker_level;
	fboolean enable_color_key;
	fboolean enable_color_mask;
	unsigned int color_key;
	unsigned int color_mask_red;
	unsigned int color_mask_green;
	unsigned int color_mask_blue;
} n_stb_disp_config_s;

n_stb_disp_config_s *n_stb_get_disp_conf();

#endif

