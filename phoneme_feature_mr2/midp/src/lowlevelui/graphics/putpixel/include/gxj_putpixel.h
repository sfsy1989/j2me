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

#ifndef _GXJ_PORT_H
#define _GXJ_PORT_H

/**
 * @file
 * @ingroup lowui_port
 *
 * @brief Porting Interface of putpixel based graphics system
 */

#include <string.h>
#include <gx_image.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enable verbose clip checks of screen buffer vs clip[XY][12] and
 * X,Y coordinates (or pointers) against screen buffer and clip[XY][12].
 * This is useful for debugging clipping bugs.
 */
#ifndef ENABLE_BOUNDS_CHECKS
#define ENABLE_BOUNDS_CHECKS 0
#endif

#ifdef FB_32BPP
typedef unsigned int gxj_pixel_type;
#else
/**
 * 16-bit pixel.
 * The color encoding used in pixels is 565, that is,
 * 5+6+5=16 bits for red, green, blue.
 */
typedef unsigned short gxj_pixel_type;
#endif

/** 8-bit alpha */
typedef unsigned char gxj_alpha_type;

/**dont include (x2,y2)*/
struct fv_rect {
    int x1;
    int y1;
    int x2;
    int y2;
};

#define FV_SCREEN_WIDTH          640
#define FV_SCREEN_HEIGHT         528
#define FV_BLOCK_WIDTH           32
#define FV_BLOCK_HEIGHT          16
#define FV_BLOCK_X_NUM           (FV_SCREEN_WIDTH / FV_BLOCK_WIDTH)
#define FV_BLOCK_Y_NUM           (FV_SCREEN_HEIGHT / FV_BLOCK_HEIGHT)
#define FV_BLOCK_X(x)            ((x)>>5)
#define FV_BLOCK_Y(y)            ((y)>>4)
#define FV_BLOCK2PIXEL_X(x)      ((x)<<5)
#define FV_BLOCK2PIXEL_Y(y)      ((y)<<4)

#define FV_REFRESH_BLOCK          0
#define FV_REFRESH_DIRTYRECT      1
#define FV_REFRESH_FULLSCREEN     2

/** Screen buffer definition */
typedef struct _gxj_screen_buffer {
    int width;	/**< width in pixel */
    int height;	/**< height in pixel */
    gxj_pixel_type *pixelData; /**< pointer to array of pixel data */
    gxj_alpha_type *alphaData; /**< pointer to array of alpha data */
    
    unsigned char dirtyBlock[FV_BLOCK_X_NUM*FV_BLOCK_Y_NUM];
    struct fv_rect dirtyRect;
    int refreshType;

#if ENABLE_BOUNDS_CHECKS
    java_graphics *g; /**< Associated Graphics object */
#endif
} gxj_screen_buffer;


#define FV_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define FV_MIN(x, y) (((x) < (y)) ? (x) : (y))

static inline void 
fv_intersect_rect(struct fv_rect *r1, struct fv_rect *r2)
{
    r1->x1 = FV_MAX(r1->x1, r2->x1);
    r1->x2 = FV_MIN(r1->x2, r2->x2);
    r1->y1 = FV_MAX(r1->y1, r2->y1);
    r1->y2 = FV_MIN(r1->y2, r2->y2);
}

static inline void
fv_union_rect(struct fv_rect *r1, struct fv_rect *r2)
{
    r1->x1 = FV_MIN(r1->x1, r2->x1);
    r1->x2 = FV_MAX(r1->x2, r2->x2);
    r1->y1 = FV_MIN(r1->y1, r2->y1);
    r1->y2 = FV_MAX(r1->y2, r2->y2);
}

static inline void
fv_init_dirty_block(gxj_screen_buffer *sbuf)
{
    memset(sbuf->dirtyBlock, 0, sizeof(sbuf->dirtyBlock));
    sbuf->dirtyRect.x1 = 0;
    sbuf->dirtyRect.y1 = 0;
    sbuf->dirtyRect.x2 = 0;
    sbuf->dirtyRect.y2 = 0;
    sbuf->refreshType  = FV_REFRESH_BLOCK;
}


void
fv_add_dirty_block(gxj_screen_buffer *sbuf, struct fv_rect *r);


static inline void
fv_add_dirty_rect_xy(gxj_screen_buffer *sbuf,
                     int x1, int y1, int x2, int y2)
{
    struct fv_rect r;

    r.x1  = x1;
    r.y1  = y1;
    r.x2  = x2;
    r.y2  = y2;
    
    fv_add_dirty_block(sbuf, &r);
}                     

static inline void
fv_add_clip_dirty_rect_xy(gxj_screen_buffer *sbuf, const jshort *clip,
                          int x1, int y1, int x2, int y2)
{
    struct fv_rect r;
    struct fv_rect cr;

    cr.x1 = clip[0];
    cr.y1 = clip[1];
    cr.x2 = clip[2];
    cr.y2 = clip[3];

    r.x1  = x1;
    r.y1  = y1;
    r.x2  = x2;
    r.y2  = y2;
    
    fv_intersect_rect(&r, &cr);
    fv_add_dirty_block(sbuf, &r);
}                     
                     
static inline void
fv_add_clip_dirty_rect_wh(gxj_screen_buffer *sbuf, const jshort *clip,
                          int x, int y, int w, int h)
{
    struct fv_rect r;
    struct fv_rect cr;

    cr.x1 = clip[0];
    cr.y1 = clip[1];
    cr.x2 = clip[2];
    cr.y2 = clip[3];

    r.x1  = x;
    r.y1  = y;
    r.x2  = x + w;
    r.y2  = y + h;
    
    fv_intersect_rect(&r, &cr);
    fv_add_dirty_block(sbuf, &r);
}                     

/**
 * Each port must define one system screen buffer
 * from where pixels are copied to physical screen.
 */
extern gxj_screen_buffer gxj_system_screen_buffer;

/**
 * @name Accessing pixel colors
 * These macros return separate colors packed as 5- and 6-bit fields
 * in a pixel.
 * The pixel color encoding is 565, that is, 5+6+5=16 bits for red, green,
 * blue.
 * The returned separate colors are 8 bits as in Java RGB.
 * @{
 */
#ifdef FB_32BPP
#define GXJ_GET_RED_FROM_PIXEL(P)   (((P) & 0xff0000) >> 16)
#define GXJ_GET_GREEN_FROM_PIXEL(P) (((P) & 0xff00) >> 8)
#define GXJ_GET_BLUE_FROM_PIXEL(P)   ((P) & 0xff)
#else
#define GXJ_GET_RED_FROM_PIXEL(P)   (((P) >> 8) & 0xF8)
#define GXJ_GET_GREEN_FROM_PIXEL(P) (((P) >> 3) & 0xFC)
#define GXJ_GET_BLUE_FROM_PIXEL(P)  (((P) << 3) & 0xF8)
#endif
/** @} */

/** Convert pre-masked triplet r, g, b to 16 bit pixel. */
#ifdef FB_32BPP
#define GXJ_RGB2PIXEL(r, g, b) ((b) | ((g) << 8) | ((r) << 16) | 0xff000000)
#else
#define GXJ_RGB2PIXEL(r, g, b) ( b +(g << 5)+ (r << 11) )
#endif

#ifdef FB_32BPP
#define GXJ_RGB24TOPIXEL(x) ((x) | 0xFF000000)
#define GXJ_PIXELTORGB24(x) (x)
#else
/** Convert 24-bit RGB color to 16bit (565) color */
#define GXJ_RGB24TORGB16(x) (((( x ) & 0x00F80000) >> 8) + \
                             ((( x ) & 0x0000FC00) >> 5) + \
			     ((( x ) & 0x000000F8) >> 3) )

/** Convert 16-bit (565) color to 24-bit RGB color */
#define GXJ_RGB16TORGB24(x) ( ((x & 0x001F) << 3) | ((x & 0x001C) >> 2) |\
                              ((x & 0x07E0) << 5) | ((x & 0x0600) >> 1) |\
                              ((x & 0xF800) << 8) | ((x & 0xE000) << 3) )
#define GXJ_RGB24TOPIXEL GXJ_RGB24TORGB16
#define GXJ_PIXELTORGB24 GXJ_RGB16TORGB24
#endif



/**
 * Extend the 8-bit Alpha value of an ARGB8888 pixel
 * over 24 bits.
 * Used for alpha blending a RGB888 pixel.
 */
#define GXJ_XAAA8888_FROM_ARGB8888(src) \
(unsigned int)(((src >> 24) & 0x000000FF) | \
               ((src >> 16) & 0x0000FF00) | \
               ((src >> 8 ) & 0x00FF0000) )


/**
 * Convert a Java platform image object to its native representation.
 *
 * @param jimg Java platform Image object to convert from
 * @param sbuf pointer to Screen buffer structure to populate
 * @param g optional Graphics object for debugging purposes only.
 *          Provide NULL if this parameter is irrelevant.
 *
 * @return the given 'sbuf' pointer for convenient usage,
 *         or NULL if the image is null.
 */
gxj_screen_buffer* gxj_get_image_screen_buffer_impl(const java_imagedata *img,
						    gxj_screen_buffer *sbuf,
						    kjobject graphics);

/**
 * Convenient macro for getting screen buffer from a Graphics's target image.
 */
#define GXJ_GET_GRAPHICS_SCREEN_BUFFER(g,sbuf) \
   gxj_get_image_screen_buffer_impl(GXAPI_GET_IMAGEDATA_PTR_FROM_GRAPHICS(g),sbuf,g)

#ifdef __cplusplus
}
#endif

#endif /* _GXJ_PORT_H */
