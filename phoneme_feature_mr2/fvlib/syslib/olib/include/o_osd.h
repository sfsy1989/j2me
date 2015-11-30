#ifndef __O_OSD_H__
#define __O_OSD_H__

#include "f_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	O_OSD_DEFLICKER_AUTO = 0,
	O_OSD_DEFLICKER_LOW,
	O_OSD_DEFLICKER_MIDDLE,
	O_OSD_DEFLICKER_HIGH
};

typedef struct {
	int x;
	int y;
	int w;
	int h;
} o_osd_rect_s;

typedef struct {
	o_osd_rect_s src_rect;
	o_osd_rect_s osd_rect;
	fboolean enable_color_key;
	fboolean enable_color_mask;
	unsigned int color_key;
	unsigned int color_mask_red;
	unsigned int color_mask_green;
	unsigned int color_mask_blue;
	int color_format;
	fboolean fullscreen_refresh;
	int deflicker_level;
	fboolean sync_output;
} o_osd_init_param_s;

int o_osd_get_num();
int o_osd_get_primary_id(); // for browser
int o_osd_get_secondary_id(); // for osd
int o_osd_get_video_id();
int o_osd_set_order(int osd_primary, int osd_secondary, int video);

int o_osd_init(int id, o_osd_init_param_s *osd_init_param);
void o_osd_uninit(int id);
int o_osd_start(int id);
void o_osd_stop(int id);
int o_osd_set_enable(int id, fboolean enable);
unsigned char *o_osd_get_buffer(int id);
int o_osd_bitblit(int id, unsigned char *bits, o_osd_rect_s *src_rect, o_osd_rect_s *dst_rect);
/* framebuffer's device space */
int o_osd_get_width(int id);
int o_osd_get_height(int id);
/* framebuffer's logic space */
int o_osd_get_src_width(int id);
int o_osd_get_src_height(int id);
int o_osd_clear_buf(int id);

#ifdef __cplusplus
}
#endif

#endif

