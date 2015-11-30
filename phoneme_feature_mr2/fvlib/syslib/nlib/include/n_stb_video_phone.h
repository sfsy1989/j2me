#ifndef __N_STB_VIDEO_PHONE_H__
#define __N_STB_VIDEO_PHONE_H__

#ifdef __cplusplus
extern "C" {
#endif

int video_phone_open();
int send_key_to_qt(int key);
fboolean is_video_phone_ui_open();

#ifdef __cplusplus
}
#endif

#endif
