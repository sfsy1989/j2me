/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

/**
 * @file
 * Implementation of the porting layer for generic fb application
 */

#include <errno.h>
#include <sys/ipc.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* The following is needed for accessing /dev/fb */
#include <linux/fb.h>
/* The following is needed for accessing /dev/tty? in RAW mode */
#include <linux/kd.h>
#include <termios.h>
#include <sys/vt.h>
#include <signal.h>
#ifdef VESA_NO_BLANKING
#include <linux/input.h>
#endif
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <gxj_putpixel.h>
#include <gxj_screen_buffer.h>
#include <midp_logging.h>
#include <midpMalloc.h>
#include <midp_constants_data.h>
#include <fbapp_device_type.h>
#include <fbapp_export.h>
#include "fb_port.h"
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

static struct android_app * user_android_app = NULL;
static char * display_buffer = NULL;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


//struct engine g_engine;

/* Hisi  ir Ioctl definitions */
#define IR_IOC_SET_BUF               0x01
#define IR_IOC_SET_ENABLE_KEYUP      0x02// 1:check keyup
#define IR_IOC_SET_ENABLE_REPKEY     0x03// 1:check repkey, 0:hardware behave
#define IR_IOC_SET_REPKEY_TIMEOUTVAL 0x04
#define IR_IOC_ENABLE                0x05// 1:enable ir, 0:disable ir
#define IR_IOC_RESET                 0x06
#define IR_IOC_SET_BLOCKTIME         0x07
#define IR_IOC_SET_FORMAT            0x10
#define IR_IOC_SET_CODELEN           0x11
#define IR_IOC_SET_FREQ              0x12
#define IR_IOC_SET_LEADS             0x13
#define IR_IOC_SET_LEADE             0x14
#define IR_IOC_SET_SLEADE            0x15
#define IR_IOC_SET_CNT0_B            0x16
#define IR_IOC_SET_CNT1_B            0x17
#define IR_IOC_GET_CONFIG            0x20
#define IR_IOC_ENDBG                 0x31
#define IR_IOC_DISDBG                0x32

#define QUEUE_KEY_PATH_NAME  "/ipstb/etc/device.ini"
#define QUEUE_KEY_PROJ_ID  32

//#define FIFOMODE (O_CREAT | O_RDWR | O_NONBLOCK)
//#define OPENMODE (O_RDONLY | O_NONBLOCK)
#define FIFO_SERVER "/var/jvm_ir"

#ifdef ANDROID
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "android_native_app_glue.h"
#include <android/log.h>
#define  LOG_TAG    "midp"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGI(...)
#endif


/**
 * By default use fast copying of rotated pixel data.
 * The assumptions required by fast copying implementation are supposed.
 */
#ifndef ENABLE_FAST_COPY_ROTATED
#define ENABLE_FAST_COPY_ROTATED    1
#endif

/** @def PERROR Prints diagnostic message. */
#define PERROR(msg) REPORT_ERROR2(0, "%s: %s", msg, strerror(errno))

/** The file descriptor for reading the mouse */
static int mouseFd = -1;
/** The file descriptor for reading the keyboard */
static int keyboardPipeFd[2] = {0};

/** Cached Linux/FrameBuffer device type retrieved from fb application */
static LinuxFbDeviceType linuxFbDeviceType =
    LINUX_FB_VERSATILE_INTEGRATOR; 

/** Return file descriptor of keyboard device, or -1 in none */
int getKeyboardFd() {
	return keyboardPipeFd[0];
}
/** Return file descriptor of mouse device, or -1 in none */
int getMouseFd() {
    return mouseFd;
}

/** System offscreen buffer */
gxj_screen_buffer gxj_system_screen_buffer;

static struct termios origTermData;
static struct termios termdata;

/** Allocate system screen buffer according to the screen geometry */
void initScreenBuffer(int width, int height) {
    gxj_system_screen_buffer.width = width;
    gxj_system_screen_buffer.height = height;
    gxj_system_screen_buffer.pixelData = NULL;
    gxj_system_screen_buffer.alphaData = NULL;
}

/**
  * Retrieve Linux/Fb device type from the fb application.
  * Note: The methods introduces cyclic dependencies between fb_port and
  * fb_application libraries to not replicate the method checkDeviceType()
  * for each fb_port library implementation.
  */
static void initLinuxFbDeviceType() {
    linuxFbDeviceType = fbapp_get_fb_device_type();
}

/** Inits keyboard device */
static void initKeyboard() {
	int ret = pipe(keyboardPipeFd);
	if(ret < 0){
		LOGI("create pipe for keyboard failed!");
		return;
	}
}

/** Frees and close grabbed keyboard device */
static void restoreConsole() {

}

#define WIDTH    640
#define HEIGHT   528
#define XOFF     40
#define YOFF     23 

#ifdef FB_32BPP
#define BPP             32
//#define TDE_FORMAT      TDE2_COLOR_FMT_ARGB8888
static struct fb_bitfield stA32 = { 24, 8, 0 };
static struct fb_bitfield stR32 = { 16, 8, 0 };
static struct fb_bitfield stG32 = { 8, 8, 0 };
static struct fb_bitfield stB32 = { 0, 8, 0 };
#else 
#define BPP             16
//#define TDE_FORMAT      TDE2_COLOR_FMT_RGB565
static struct fb_bitfield stA32 = { 0, 0, 0 };
static struct fb_bitfield stR32 = { 11, 5, 0 };
static struct fb_bitfield stG32 = { 5, 6, 0 };
static struct fb_bitfield stB32 = { 0, 5, 0 };
#endif

static int g_mapped_memlen;
static unsigned char *g_mapped_mem;
static unsigned char *g_mem0;
static unsigned char *g_mem1;

static int g_fd = -1;

static struct fb_var_screeninfo hifb_st_def_vinfo;

static int g_scale_block_width  = FV_BLOCK_WIDTH;
static int g_scale_width        = WIDTH;
static int g_scale_block_height = FV_BLOCK_HEIGHT;
static int g_scale_height       = HEIGHT;

pthread_mutex_t * 
get_pthread_mutex()
{
	return &lock;
}

char * get_display_buffer_addr()
{
	return display_buffer;
}

void set_android_app_param(struct android_app* state)
{
	user_android_app = 	state;
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
 	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
    	unsigned char keyEvent[2] = {0};
		keyEvent[0] = AKeyEvent_getKeyCode(event);
		if (AKeyEvent_getAction(event) == 1){
			keyEvent[1] = 0;
		}else{
			keyEvent[1]= 1;
		}
		int ret = write(keyboardPipeFd[1], keyEvent, sizeof(keyEvent));
        if(ret < 0){
                LOGI("jvm key event fifo send error\n");
                return -1;
        }
        LOGI("Key event: action=%d keyCode=%d metaState=0x%x",
                AKeyEvent_getAction(event),
                AKeyEvent_getKeyCode(event),
                AKeyEvent_getMetaState(event));
    }

    return 0;
}

/** Inits frame buffer device */
void initFrameBuffer() {
	display_buffer = (void *)malloc(WIDTH*HEIGHT*BPP/8);
	gxj_system_screen_buffer.pixelData = (void *)malloc(WIDTH*HEIGHT*BPP/8);
    gxj_system_screen_buffer.alphaData = NULL;
    gxj_system_screen_buffer.width = WIDTH;
    gxj_system_screen_buffer.height = HEIGHT;

	user_android_app->onInputEvent = engine_handle_input;

	fv_init_dirty_block(&gxj_system_screen_buffer);	
}
/**
 * Change screen orientation to landscape or portrait,
 * depending on the current screen mode
 */
void reverseScreenOrientation() {

    // Whether current Displayable won't repaint the entire screen on
    // resize event, the artefacts from the old screen content can appear.
    // That's why the buffer content is not preserved.
#if 0
    gxj_rotate_screen_buffer(KNI_FALSE);
#else
    printf("TODO: reverseScreenOrientation\n");
#endif
}

/** Initialize frame buffer video device */
void connectFrameBuffer() {
    initLinuxFbDeviceType();
    initKeyboard();
    initFrameBuffer();
}

/** Clear screen content */
void clearScreen() {
    int n;
    gxj_pixel_type *p = (gxj_pixel_type *)fb.data;
    gxj_pixel_type color = (gxj_pixel_type)GXJ_RGB2PIXEL(0, 0, 0);
    for (n = fb.width * fb.height; n > 0; n--) {
	    *p ++ = color;
    }
}

/**
 * Resizes system screen buffer to fit new screen geometry
 * and set system screen dimensions accordingly.
 * Call after frame buffer is initialized.
 */
void resizeScreenBuffer(int width, int height) {
#if 0
    if (gxj_resize_screen_buffer(width, height) != ALL_OK) {
        fprintf(stderr, "Failed to reallocate screen buffer\n");
        exit(1);
    }
#endif
    (void)width;
    (void)height;
}


/** Get x-coordinate of screen origin */
int getScreenX(int screenRotated) {
    // System screen buffer geometry
    int bufWidth = gxj_system_screen_buffer.width;
    int x = 0;
    int LCDwidth = screenRotated ? fb.height : fb.width;
    if (LCDwidth > bufWidth) {
        x = (LCDwidth - bufWidth) / 2;
    }
    return x;
}

/** Get y-coordinate of screen origin */
int getScreenY(int screenRotated) {
    int bufHeight = gxj_system_screen_buffer.height;
    int y = 0;
    int LCDheight = screenRotated ? fb.width : fb.height;
    if (LCDheight > bufHeight) {
        y = (LCDheight - bufHeight) / 2;
    }
    return y;
}


void refreshScreenNormal(int x1, int y1, int x2, int y2)
{
	char * temp = (char *)gxj_system_screen_buffer.pixelData;
	pthread_mutex_lock(&lock);
	memcpy(display_buffer,temp,WIDTH*HEIGHT*BPP/8);
	pthread_mutex_unlock(&lock);
}


#ifdef FB_32BPP
#else
#if ENABLE_FAST_COPY_ROTATED
/**
 * Fast rotated copying of screen buffer area to the screen memory.
 * The copying is optimized for 32bit architecture with read caching
 * to copy 16bit pixel data with 90 CCW rotation.
 *
 * The data is copied by 2x2 pixel blocks to operate with machine words.
 * Source data is traversed by lines to benefit from read caching.
 *
 * IMPL_NOTE ASSUMPTIONS:
 * 1) Source and target buffers are word aligned
 * 2) Source and target widths are even, so column pixels are equally aligned
 *
 * @param src pointer to source pixel data to start copying from
 * @param dst pointer to destination pixel data to start copying to
 * @param x1 x-coordinate of the left upper corner of the copied area
 * @param y1 y-coordinate of the left upper corner of the copied area
 * @param x2 x-coordinate of the right lower corner of the copied area
 * @param y2 y-coordinate of the right lower corner of the copied area
 * @param bufWidth width of the source screen buffer
 * @param dstWidth width of the screen
 * @param srcInc source pointer increment at the end of source row
 * @param dstInc dest pointer increment at the end of source row
 */
static void fast_copy_rotated(short *src, short *dst, int x1, int y1, int x2, int y2,
        int bufWidth, int dstWidth, int srcInc, int dstInc) {

  int x;
  unsigned a, b;
  while(y1 < y2) {
    y1 += 2;
    for (x = x1; x < x2; x += 2) {
      a = *(unsigned*)src;
      b = *(unsigned*)(src + bufWidth);
      src += 2;
      
      *(unsigned*)dst = (b << 16) | (a & 0x0000ffff);
      dst -= dstWidth;
      *(unsigned*)dst = (b & 0xffff0000) | (a >> 16);
      dst -= dstWidth;
    }
    dst += dstInc + 1;
    src += srcInc + bufWidth;
  }
}

#else /* ENABLE_FAST_COPY_ROTATED */
/**
 * Simple rotated copying of screen buffer area to the screen memory.
 * Source data is traversed by lines to benefit from read caching,
 * the target data is rotated by 90 CCW.
 *
 * IMPL_NOTE:
 *   The method can be used if IMPL_NOTE ASSUMPTIONS for fast copy
 *   routine can not be satisfied. On ARM 201MHz fast copying is about
 *   2x better than the simple one.
 *
 * @param src pointer to source pixel data to start copying from
 * @param dst pointer to destination pixel data to start copying to
 * @param x1 x-coordinate of the left upper corner of the copied area
 * @param y1 y-coordinate of the left upper corner of the copied area
 * @param x2 x-coordinate of the right lower corner of the copied area
 * @param y2 y-coordinate of the right lower corner of the copied area
 * @param bufWidth width of the source screen buffer
 * @param dstWidth width of the screen
 * @param srcInc source pointer increment at the end of source row
 * @param dstInc dest pointer increment at the end of source row
 */
static void simple_copy_rotated(short *src, short *dst, int x1, int y1, int x2, int y2,
		int bufWidth, int dstWidth, int srcInc, int dstInc) {

    int x;
    (void)bufWidth;
    while(y1++ < y2) {
        for (x = x1; x < x2; x++) {
            *dst = *src++;
            dst -= dstWidth;
         }
         dst += dstInc;
         src += srcInc;
    }
}
#endif /* ENABLE_FAST_COPY_ROTATED */
#endif

#ifdef FB_32BPP
void refreshScreenRotated(int x1, int y1, int x2, int y2) {
    printf("TODO: refreshScreenRotated in 32bpp\n");
    (void)x1;
    (void)y1;
    (void)x2;
    (void)y2;
}
#else
/** Refresh rotated screen with offscreen buffer content */
void refreshScreenRotated(int x1, int y1, int x2, int y2) {
    gxj_pixel_type *src = gxj_system_screen_buffer.pixelData;
    gxj_pixel_type *dst = (gxj_pixel_type*)fb.data;
    int srcWidth, srcHeight;
    int dstWidth = fb.width;
    int dstHeight = fb.height;

    int srcInc;
    int dstInc;

    // System screen buffer geometry
    int bufWidth = gxj_system_screen_buffer.width;
    int bufHeight = gxj_system_screen_buffer.height;

    // Check if frame buffer is big enough
    checkScreenBufferSize(bufHeight, bufWidth);

    if (linuxFbDeviceType == LINUX_FB_OMAP730) {
        // Needed by the P2 board
        // Max screen size is 176x220 but can only display 176x208
        dstHeight = bufWidth;
    }

    // Make sure the copied lines are 4-byte aligned for faster memcpy
    if ((x1 & 1) == 1) x1 -= 1;
    if ((x2 & 1) == 1) x2 += 1;
    if ((y1 & 1) == 1) y1 -= 1;
    if ((y2 & 1) == 1) y2 += 1;

    srcWidth = x2 - x1;
    srcHeight = y2 - y1;

    if (bufWidth < dstHeight || bufHeight < dstWidth) {
            // We are drawing into a frame buffer that's larger than what MIDP
            // needs. Center it.
            dst += (dstHeight - bufWidth) / 2 * dstWidth;
            dst += ((dstWidth - bufHeight) / 2);
        }

    src += x1 + y1 * bufWidth;
    dst += y1 + (bufWidth - x1 - 1) * dstWidth;

    srcInc = bufWidth - srcWidth;      // increment for src pointer at the end of row
    dstInc = srcWidth * dstWidth + 1;  // increment for dst pointer at the end of column

#if ENABLE_FAST_COPY_ROTATED
    fast_copy_rotated(src, dst, x1, y1, x2, y2,
        bufWidth, dstWidth, srcInc, dstInc);
#else
    simple_copy_rotated(src, dst, x1, y1, x2, y2,
        bufWidth, dstWidth, srcInc, dstInc);
#endif
}
#endif
/** Frees allocated resources and restore system state */
void finalizeFrameBuffer() {
#if 0
    gxj_free_screen_buffer();
#endif
    if (g_mapped_mem) {
        memset(g_mapped_mem, 0, g_mapped_memlen);
	g_mapped_mem    = 0;
        g_mapped_memlen = 0;
    }
    restoreConsole();
}

void
fv_add_dirty_block(gxj_screen_buffer *sbuf, struct fv_rect *r)
{
    int b1_x;
    int b1_y;
    int b2_x;
    int b2_y;
    int x;
    int y;

    if (r->x1 >= r->x2 || r->y1 >= r->y2) 
        return;
    if (r->x1 < 0) {
	r->x1 = 0;
    } 
    if (r->y1 < 0) {
        r->y1 = 0;
    }

    if (r->x2 > sbuf->width) {
        r->x2 = sbuf->width;
    }
    if (r->y2 > sbuf->height) {
        r->y2 = sbuf->height;
    }
    fv_union_rect(&sbuf->dirtyRect, r);
    if (sbuf->refreshType == FV_REFRESH_FULLSCREEN || 
        sbuf->refreshType == FV_REFRESH_DIRTYRECT) {
        return;
    } 
    
    if (r->x2 - r->x1 >= FV_SCREEN_WIDTH * 2 / 3 &&
        r->y2 - r->y1 >= FV_SCREEN_HEIGHT * 2 / 3) {
        sbuf->refreshType = FV_REFRESH_DIRTYRECT;
        return;
    }

    b1_x = FV_BLOCK_X(r->x1);
    b1_y = FV_BLOCK_Y(r->y1); 
    b2_x = FV_BLOCK_X(r->x2 - 1);
    b2_y = FV_BLOCK_Y(r->y2 - 1);

    for (y = b1_y; y <= b2_y; y++) {
        for (x = b1_x; x <= b2_x; x++) {
            sbuf->dirtyBlock[y * FV_BLOCK_X_NUM + x] = 1;
        }
    }
}

