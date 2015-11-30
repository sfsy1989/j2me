#ifndef __F_LIB_H__
#define __F_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "iniparser.h"

typedef enum {
    FV_FALSE = 0,
    FV_TRUE  = 1
} fboolean;

#define FV_OK   0

#if 0
#ifdef FVT_BUILD_RELEASE
#define f_assert(expr)  do { (void) 0; } while (0)
#else
#define f_assert(expr)  \
do { \
    if (!(expr)) { \
        printf("error at line %d in %s!\n", __LINE__, __FUNCTION__); \
        exit(-1); \
    } \
} while(0)
#endif
#endif

#define f_assert(expr)  do { (void) 0; } while (0)

#define f_sizeof_array(x) (sizeof(x) / sizeof(x[0]))

#include "f_platform.h"
#include "f_log.h"
#include "f_dump.h"
#include "f_daemon.h"
#include "f_timer.h"
#include "f_flash.h"
#include "f_utils.h"
#include "f_utest.h" 

#if FTM_USE(FVIPC)
#include "f_ipc.h"
#endif

typedef void (*f_net_download_cb)(int result);

int f_flash_nanddump_startpage(char *mtddev,char *readbuf);
int f_net_get_download_size(char *src);
double f_net_get_last_download_speed();
void f_net_stop_multi();
int f_net_download_multi(char *dest, char *src, unsigned char fake,
	int connect_timeout, int download_timeout, f_net_download_cb callback);
int f_net_download(char *dest, char *src, int connect_timeout, int download_timeout);
int f_net_upload(char *url, char *localfile, char *remotename);

#ifdef __cplusplus
}
#endif

#endif
