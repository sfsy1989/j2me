#ifndef __PRODUCT_MEDIA_H_
#define __PRODUCT_MEDIA_H_

#ifdef __cplusplus
extern "C" {
#endif 

int product_media_initialize(void);
int product_media_finalize(void);
int product_media_start(char* media_file);
int product_media_stop(void);


#ifdef __cplusplus
}
#endif

#endif

