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

#include <kni.h>
#include <midp_logging.h>

#include <gxutl_graphics.h>

#include "gxj_intern_graphics.h"
#include "gxj_intern_putpixel.h"
#include "gxj_intern_font_bitmap.h"
#ifdef FB_USE_FREETYPE
#include <stdint.h>
#include "ft2build.h"

#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/ftbbox.h"
#include "freetype/ftcache.h"

#ifdef ANDROID
#define FV_FONT_FILE "/system/fonts/DroidSansFallback.ttf"
#else
#define FV_FONT_FILE     "/ipstb/bin/fvbrowser.ttf"
#endif

#define FT_CEIL(X) ((((X) + 63) & -64) / 64)
#define FT_FLOOR(X)	(((X) & -64) / 64)

#define CMANAGER_FACE_NUM_MAX 1
#define CMANAGER_SIZE_NUM_MAX 5
#define CMANAGER_BYTES_MAX 1048576
#define CACHE_SIZE_NUM_MAX 20
#define CACHE_CHAR_NUM_MAX 1024


struct ft_text_ctx {
	const char      *font_file;

	FT_Library       ft_lib;
	FT_Face          face;

	FTC_Manager      ftc_manager;
	FTC_ImageCache   image_cache;

	FT_Glyph         glyph;
	FT_Glyph         bitmap;
	FTC_ScalerRec    scale;
	int              adv_x;
	int              adv_y;

	int              style;
	int              face_type;
	int              size;

	int              ascent;
	int              descent;

	int              width;
	int              height;
	int              yoff;
	
	FT_Matrix        italic_matrix;
};
static struct ft_text_ctx g_text_ctx;
static int    g_ft_lib_init = 0;

void init_ft_text_ctx()
{
	struct ft_text_ctx *p = &g_text_ctx;	

	p->style              = -1;
	p->face_type          = -1;
	p->size               = -1;

	p->width              =  0;
	p->height             =  0;
	p->ascent             =  0;
	p->descent            =  0;

	p->italic_matrix.xx   = 1 << 16;
	p->italic_matrix.xy   = 0x5800;
	p->italic_matrix.yx   = 0;
	p->italic_matrix.yy   = 1 << 16;

	p->font_file          = FV_FONT_FILE;
}

static FT_Error 
ftc_face_requester(FTC_FaceID face_id, FT_Library library, 
                   FT_Pointer req_data, FT_Face *aface)
{
	struct ft_text_ctx *ctx;
	FT_Error err;
	(void)req_data;
	ctx = (struct ft_text_ctx *)face_id;
	
	err = FT_New_Face(library, ctx->font_file, 0, aface);
	if (err) {
		printf("Error %d opening face\n", err);
		exit(1);
	}
	return err;
}


static void init_ft_lib()
{
	FT_Error err;
	
	if (g_ft_lib_init) {
		return;
	}

	err = FT_Init_FreeType(&g_text_ctx.ft_lib);
	if (err) {
		printf("init freetype library failed\n");
		exit(1);
	}

	init_ft_text_ctx();
		
	if (FTC_Manager_New(g_text_ctx.ft_lib, CMANAGER_FACE_NUM_MAX, 
	                    CMANAGER_SIZE_NUM_MAX, CMANAGER_BYTES_MAX,
	                    ftc_face_requester, NULL, 
	                    &g_text_ctx.ftc_manager)) {
		printf("FreeType Cache Manager init failed!\n");
		exit(1);
	}

	if (FTC_ImageCache_New(g_text_ctx.ftc_manager, 
	                       &g_text_ctx.image_cache)) {
		printf("FreeType Image Cache init failed!\n");
		exit(1);
	}

	if (FTC_Manager_LookupFace(g_text_ctx.ftc_manager, 
	                           (FTC_FaceID)(void*)&g_text_ctx, 
	                           &g_text_ctx.face)) {
		printf("request new face failed!\n");
		exit(1);
	}
	g_ft_lib_init = 1; 
}


static void find_font(int face, int style, int size)
{
	int change = 0;
	FT_Size ftsize;

	init_ft_lib();

	if (g_text_ctx.face_type != face) {
		g_text_ctx.face_type = face;
		change = 1;
	}
	if (g_text_ctx.style != style) {
		g_text_ctx.style = style;
		change = 1;
	}
	if (g_text_ctx.size != size) {
		int pointsize;
	
		switch (size) {
		default:
		case SIZE_SMALL:
			pointsize = face == FACE_MONOSPACE ? 7: 11;
			break;
		case SIZE_MEDIUM:
			pointsize = 13;
			break;
		case SIZE_LARGE:
			pointsize = 17;
			break;
		}

		g_text_ctx.scale.face_id = (FTC_FaceID)(void*)&g_text_ctx;
		g_text_ctx.scale.width   = pointsize << 6;
		g_text_ctx.scale.height  = pointsize << 6;
		g_text_ctx.scale.pixel   = 0;
		g_text_ctx.scale.x_res   = 72;
		g_text_ctx.scale.y_res   = 72;
		FTC_Manager_LookupSize(g_text_ctx.ftc_manager, 
		                       &g_text_ctx.scale, 
		                       &ftsize);

		g_text_ctx.ascent = ftsize->metrics.ascender >> 6;
		g_text_ctx.descent = ftsize->metrics.descender >> 6;
		g_text_ctx.height = ftsize->metrics.height >> 6;
		g_text_ctx.width = ftsize->metrics.max_advance >> 6;

		g_text_ctx.size = size;
		change = 1;
	}
}

/**TODO: STYLE_UNDERLINED*/
static int load_char(jchar ch)
{
	FT_UInt    index;
	FT_Error   err;

	index = FT_Get_Char_Index(g_text_ctx.face, ch);
	if (!index) {
		return -1;
	}

	
	err = FTC_ImageCache_LookupScaler(g_text_ctx.image_cache,
	                            &g_text_ctx.scale,
	                            FT_LOAD_DEFAULT, 
	                            index, 
	                            &g_text_ctx.glyph, 
	                            NULL);
	if (err) {
		return -1;
	}
	g_text_ctx.adv_x = g_text_ctx.glyph->advance.x >> 16;
	g_text_ctx.adv_y = g_text_ctx.glyph->advance.y >> 16;
	return 0;
}


static int render_char()
{
	FT_Error   err;
#if 0
	if (g_text_ctx.glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
		FT_Outline outline;
		FT_OutlineGlyph outline_glyph;
		FT_Glyph glyph;

		FT_Glyph_Copy(g_text_ctx.glyph, &glyph);
		g_text_ctx.glyph = glyph;
		outline_glyph = ((FT_OutlineGlyph)g_text_ctx.glyph);
		outline = outline_glyph->outline;
		
		if (g_text_ctx.style & STYLE_ITALIC) {
			FT_Outline_Transform(&outline, 
				             &g_text_ctx.italic_matrix);
		}
		if (g_text_ctx.style & STYLE_BOLD) {
			int strength = 1 << 6;
			FT_Outline_Embolden(&outline, strength);
		}
	}
#endif


	if (g_text_ctx.glyph->format != FT_GLYPH_FORMAT_BITMAP) {
		g_text_ctx.bitmap = g_text_ctx.glyph;
		err = FT_Glyph_To_Bitmap(&g_text_ctx.bitmap, 
		                         FT_RENDER_MODE_NORMAL,
		                         NULL, 0);
		if (err) {
			//FT_Done_Glyph(g_text_ctx.bitmap);
			g_text_ctx.bitmap = 0;
			return -1;
		}
	}

	return 0;
}

#ifdef FB_32BPP
/**32bpp version*/
static void draw_char(gxj_screen_buffer *sbuf, gxj_pixel_type color,
                      int dstx, int dsty, const jshort *clip)
{
	FT_BitmapGlyph bmp_glyph;
	FT_Bitmap *bitmap;
	uint8_t *dst;
	uint8_t *src;
	int orgx;
	int orgy;
	int src_inc;
	int dst_inc;
	int clip_w;
	int clip_h;
	int clip_x1;
	int clip_y1;
	int clip_x2;
	int clip_y2;
	int alpha;
	int x;
	int y;
	int r;
	int g;
	int b;
	
	r = (color & 0x00ff0000) >> 16;
	g = (color & 0x0000ff00) >> 8;
	b = color & 0x000000ff;

	bmp_glyph = (FT_BitmapGlyph)g_text_ctx.bitmap;
	bitmap = (FT_Bitmap*)&(bmp_glyph->bitmap);

	if (!bitmap->width)
		return;

	orgx = dstx + bmp_glyph->left;
	orgy = dsty + g_text_ctx.ascent - bmp_glyph->top;

	/**intersect with clip and (orgx, orgy)+width*rows*/
	clip_x1 = FV_MAX(clip[0], orgx);
	clip_y1 = FV_MAX(clip[1], orgy);
	clip_x2 = FV_MIN(clip[2], orgx+bitmap->width);
	clip_y2 = FV_MIN(clip[3], orgy+bitmap->rows);

	clip_w  = clip_x2 - clip_x1;
	clip_h  = clip_y2 - clip_y1;

	if (clip_w <= 0 || clip_h <= 0)
		return;
	
	dst = (uint8_t *)(sbuf->pixelData + clip_y1 * sbuf->width + clip_x1);
	src = bitmap->buffer+(clip_y1-orgy)*bitmap->pitch+(clip_x1-orgx);

	/**1Byte alpha*/
	src_inc = bitmap->pitch - clip_w;
	dst_inc = (sbuf->width - clip_w) << 2;
		
	for (y = 0; y < clip_h; y++, src+=src_inc, dst+=dst_inc) {
		for (x = 0; x < clip_w; x++, dst+=4, src++) {
			alpha = *src;
			dst[0] = (alpha*g+(256-alpha)*dst[0])>>8;  
			dst[1] = (alpha*b+(256-alpha)*dst[1])>>8;  
			dst[2] = (alpha*r+(256-alpha)*dst[2])>>8;  
			dst[3] = 0xff;
		}
	}
}
#else
/**TODO: 16bpp*/
static void draw_char(gxj_screen_buffer *sbuf, gxj_pixel_type color,
                      int dstx, int dsty, const jshort *clip)
{
	FT_BitmapGlyph bmp_glyph;
	FT_Bitmap *bitmap;
	uint16_t *dst;
	uint8_t *src;
	int orgx;
	int orgy;
	int src_inc;
	int dst_inc;
	int clip_w;
	int clip_h;
	int clip_x1;
	int clip_y1;
	int clip_x2;
	int clip_y2;
	int alpha;
	int x;
	int y;
        int r1, g1, b1, a2, a3, r2, b2, g2;
	
	r1 = (color >> 11);
	g1 = ((color >> 5) & 0x3F);
	b1 = (color & 0x1F);

	bmp_glyph = (FT_BitmapGlyph)g_text_ctx.bitmap;
	bitmap = (FT_Bitmap*)&(bmp_glyph->bitmap);

	if (!bitmap->width)
		return;

	orgx = dstx + bmp_glyph->left;
	orgy = dsty + g_text_ctx.ascent - bmp_glyph->top;

	/**intersect with clip and (orgx, orgy)+width*rows*/
	clip_x1 = FV_MAX(clip[0], orgx);
	clip_y1 = FV_MAX(clip[1], orgy);
	clip_x2 = FV_MIN(clip[2], orgx+bitmap->width);
	clip_y2 = FV_MIN(clip[3], orgy+bitmap->rows);

	clip_w  = clip_x2 - clip_x1;
	clip_h  = clip_y2 - clip_y1;

	if (clip_w <= 0 || clip_h <= 0)
		return;
	
	dst = (uint16_t *)(sbuf->pixelData + clip_y1 * sbuf->width + clip_x1);
	src = bitmap->buffer+(clip_y1-orgy)*bitmap->pitch+(clip_x1-orgx);

	/**1Byte alpha*/
	src_inc = bitmap->pitch - clip_w;
	dst_inc = sbuf->width - clip_w;
		
	for (y = 0; y < clip_h; y++, src+=src_inc, dst+=dst_inc) {
		for (x = 0; x < clip_w; x++, dst++, src++) {
			unsigned short c = *dst;
			alpha = *src;

                        r2 = (c >> 11);
                        g2 = ((c >> 5) & 0x3F);
                        b2 = (c & 0x1F);

                        a2 = alpha >> 2;
                        a3 = alpha >> 3;

                        r2 = (r1 * a3 + r2 * (31 - a3)) >> 5;
                        g2 = (g1 * a2 + g2 * (63 - a2)) >> 6;
                        b2 = (b1 * a3 + b2 * (31 - a3)) >> 5;

                        *dst = (gxj_pixel_type)((r2 << 11) | (g2 << 5) | (b2));
		}
	}
}
#endif

static inline void 
adj_clip(gxj_screen_buffer *dest, const jshort *clip, jshort *nclip)
{
	memcpy(nclip, clip, 4 * sizeof(clip[0]));
	/*
	* Don't let a bad clip origin into the clip code or the may be
	* over or under writes of the destination buffer.
	*
	* Don't change the clip array that was passed in.
	*/
	if (clip[0] < 0) {
		nclip[0] = 0;
	} 
       
	if (clip[1] < 0) {
		nclip[1] = 0;
	}

	if (clip[2] > dest->width) {
		nclip[2] =  dest->width;
	}

	if (clip[3] > dest->height) {
		nclip[3] = dest->height;
	}
}

void
gx_draw_chars(jint pixel, const jshort *clip, 
	      const java_imagedata *dst, int dotted, 
	      int face, int style, int size,
	      int x, int y, int anchor, const jchar *charArray, int n)
{
	int i;
	int dstx;
	int dsty;
	gxj_screen_buffer screen_buffer;
	gxj_screen_buffer *dest = gxj_get_image_screen_buffer_impl(dst, 
	                                  &screen_buffer, NULL);
	dest = (gxj_screen_buffer *)getScreenBuffer(dest);
	jshort nclip[4];

	find_font(face, style, size);

	if (anchor & RIGHT) {
		x -= gx_get_charswidth(face, style, size, charArray, n);
	}
	if (anchor & HCENTER) {
		x -= gx_get_charswidth(face, style, size, charArray, n) >> 1;
	}
	if (anchor & BOTTOM) {
        	y -= g_text_ctx.height;
	}
	if (anchor & BASELINE) {
        	y -= g_text_ctx.height + g_text_ctx.descent;
	}
	dstx = x;
	dsty = y;
	
	adj_clip(dest, clip, nclip);
	if (nclip[0] >= nclip[2] || nclip[1] >= nclip[3]) {
		/* Nothing to do. */
		return;
	}

	/* Surpress unused parameter warnings */
	(void)dotted;

	for (i = 0; i < n; i++) {
		if (load_char(charArray[i]) < 0) {
			continue;
		}
		if (render_char() < 0) {
			continue;
		}
	        draw_char(dest, GXJ_RGB24TOPIXEL(pixel), dstx, dsty, nclip);
		dstx += g_text_ctx.adv_x;
	
		FT_Done_Glyph(g_text_ctx.bitmap);
		g_text_ctx.bitmap = 0;
	}
	fv_add_dirty_rect_xy(dest, x, y, dstx, y+g_text_ctx.height);
}


void
gx_get_fontinfo(int face, int style, int size, 
		int *ascent, int *descent, int *leading)
{
	find_font(face, style, size);

	if (!g_text_ctx.width) {
		*ascent  = 0; 
		/**FIXME: +-???*/
		*descent = 0;
		*leading = 1; 
		return;
	}
	*ascent = g_text_ctx.ascent;
	*descent = -g_text_ctx.descent;
	*leading = 1; 
}

int
gx_get_charswidth(int face, int style, int size, 
		  const jchar *charArray, int n)
{
	//FT_Error error;
	int width = 0;
	int i;

	find_font(face, style, size);

	for (i = 0; i < n; i++) {
		if (load_char(charArray[i]) < 0) 
			continue;
		width += g_text_ctx.adv_x;
	}
	return width;
}

#else
/**
 * @file
 *
 * given character code c, return the bitmap table where
 * the encoding for this character is stored.
 */
static pfontbitmap selectFontBitmap(jchar c, pfontbitmap* pfonts) {
    int i=1;
    unsigned char c_hi = (c>>8) & 0xff;
    unsigned char c_lo = c & 0xff;
    do {
        if ( c_hi == pfonts[i][FONT_CODE_RANGE_HIGH]
          && c_lo >= pfonts[i][FONT_CODE_FIRST_LOW]
          && c_lo <= pfonts[i][FONT_CODE_LAST_LOW]
        ) {
            return pfonts[i];
        }
        i++;
    } while (i <= (int) pfonts[0]);
    /* the first table must cover the range 0-nn */
    return pfonts[1];
}


/**
 * @file
 *
 * putpixel primitive character drawing. 
 */
unsigned char BitMask[8] = {0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};
static void drawChar(gxj_screen_buffer *sbuf, jchar c0,
		     gxj_pixel_type pixelColor, int x, int y,
		     int xSource, int ySource, int xLimit, int yLimit,
		     pfontbitmap* pfonts,
		     int fontWidth, int fontHeight) {
    int xDest;
    int yDest;
    int xDestLimit;
    int yDestLimit;
    unsigned long byteIndex;
    int bitOffset;
    unsigned long pixelIndex;
    unsigned long pixelIndexLineInc;
    unsigned char bitmapByte;
    unsigned char const * fontbitmap =
        selectFontBitmap(c0,pfonts) + FONT_DATA;
    jchar const c = (c0 & 0xff) -
        fontbitmap[FONT_CODE_FIRST_LOW-FONT_DATA];
    unsigned long mapLen =
        ((fontbitmap[FONT_CODE_LAST_LOW-FONT_DATA]
        - fontbitmap[FONT_CODE_FIRST_LOW-FONT_DATA]
        + 1) * fontWidth * fontHeight + 7) >> 3;
    unsigned long const firstPixelIndex = c * fontHeight * fontWidth;
    unsigned char const * const mapend = fontbitmap + mapLen;

    int destWidth = sbuf->width;
    gxj_pixel_type *dest = sbuf->pixelData + y*destWidth + x;
    int destInc = destWidth - xLimit + xSource;

    pixelIndex = firstPixelIndex + (ySource * fontWidth) + xSource;
    pixelIndexLineInc = fontWidth - (xLimit - xSource);
    byteIndex = pixelIndex / 8;
    fontbitmap += byteIndex;
    bitOffset = pixelIndex % 8;
    bitmapByte = *fontbitmap;
    yDestLimit = y + yLimit - ySource;
    xDestLimit = x + xLimit - xSource;

    // The clipping should be applied here already, so
    // we use optimal access to destination buffer with
    // no extra checks

    if (fontbitmap < mapend) {
        for (yDest = y; yDest < yDestLimit;
                yDest++, bitOffset+=pixelIndexLineInc, dest += destInc) {
            for (xDest = x; xDest < xDestLimit;
                    xDest++, bitOffset++, dest++) {
                if (bitOffset >= 8) {
                    fontbitmap += bitOffset / 8;
                    if (fontbitmap >= mapend) {
                        break;
                    }
                    bitOffset %= 8;
                    bitmapByte = *fontbitmap;
                }

                /* we don't draw "background" pixels, only foreground */
                if ((bitmapByte & BitMask[bitOffset]) != 0) {
                    *dest = pixelColor;
                }
            }
        }
    }
    fv_add_dirty_rect_xy(sbuf, x, y, xDestLimit, yDestLimit);
}

/*
 * Draws the first n characters specified using the current font,
 * color, and anchor point.
 *
 * <p>
 * <b>Reference:</b>
 * Related Java declaration:
 * <pre>
 *     drawString(Ljava/lang/String;III)V
 * </pre>
 *
 * @param pixel Device-dependent pixel value
 * @param clip Clipping information
 * @param dst Platform dependent destination information
 * @param dotted The stroke style to be used
 * @param face The font face to be used (Defined in <B>Font.java</B>)
 * @param style The font style to be used (Defined in
 * <B>Font.java</B>)
 * @param size The font size to be used. (Defined in <B>Font.java</B>)
 * @param x The x coordinate of the anchor point
 * @param y The y coordinate of the anchor point
 * @param anchor The anchor point for positioning the text
 * @param chararray Pointer to the characters to be drawn
 * @param n The number of characters to be drawn
 */
void
gx_draw_chars(jint pixel, const jshort *clip, 
	      const java_imagedata *dst, int dotted, 
	      int face, int style, int size,
	      int x, int y, int anchor, const jchar *charArray, int n) {
    int i;
    int xStart;
    int xDest;
    int yDest;
    int width;
    int yLimit;
    int nCharsToSkip = 0;
    int widthRemaining;
    int yCharSource;
    int fontWidth;
    int fontHeight;
    int fontDescent;
    int clipX1 = clip[0];
    int clipY1 = clip[1];
    int clipX2 = clip[2];
    int clipY2 = clip[3];
    int diff;
    gxj_screen_buffer screen_buffer;
    gxj_screen_buffer *dest = 
      gxj_get_image_screen_buffer_impl(dst, &screen_buffer, NULL);
    dest = (gxj_screen_buffer *)getScreenBuffer(dest);

    REPORT_CALL_TRACE(LC_LOWUI, "LCDUIdrawChars()\n");

    /* Surpress unused parameter warnings */
    (void)dotted;
    (void)face;
    (void)size;
    (void)style;

    if (n <= 0) {
        /* nothing to do */
        return;
    }

    fontWidth = FontBitmaps[1][FONT_WIDTH];
    fontHeight = FontBitmaps[1][FONT_HEIGHT];
    fontDescent = FontBitmaps[1][FONT_DESCENT];

    xDest = x;
    if (anchor & RIGHT) {
        xDest -= fontWidth * n;
    }

    if (anchor & HCENTER) {
        xDest -= ((fontWidth * n) / 2);
    }

    yDest = y;  
    if (anchor & BOTTOM) {
        yDest -= fontHeight;
    }

    if (anchor & BASELINE) {
        yDest -= fontHeight - fontDescent;
    }

    width = fontWidth * n;
    yLimit = fontHeight;

    xStart = 0;
    yCharSource = 0;

    /*
     * Don't let a bad clip origin into the clip code or the may be
     * over or under writes of the destination buffer.
     *
     * Don't change the clip array that was passed in.
     */
    if (clipX1 < 0) {
        clipX1 = 0;
    }
       
    if (clipY1 < 0) {
        clipY1 = 0;
    }

    diff = clipX2 - dest->width;
    if (diff > 0) {
        clipX2 -= diff;
    }

    diff = clipY2 - dest->height;
    if (diff > 0) {
        clipY2 -= diff;
    }

    if (clipX1 >= clipX2 || clipY1 >= clipY2) {
        /* Nothing to do. */
        return;
    }

    /* Apply the clip region to the destination region */
    diff = clipX1 - xDest;
    if (diff > 0) {
        xStart += diff % fontWidth;
        width -= diff;
        xDest += diff;
        nCharsToSkip = diff / fontWidth;
    }

    diff = (xDest + width) - clipX2;
    if (diff > 0) {
        width -= diff;
        n -= diff/fontWidth;
    }

    diff = (yDest + fontHeight) - clipY2;
    if (diff > 0) {
        yLimit -= diff;
    }

    diff = clipY1 - yDest;
    if (diff > 0) {
        yCharSource += diff;
        yDest += diff;
    }

    if (width <= 0 || yCharSource >= yLimit || nCharsToSkip >= n) {
        /* Nothing to do. */
        return;
    }

    widthRemaining = width;

    if (xStart != 0) {
        int xLimit;
        int startWidth;
        if (width > fontWidth) {
            startWidth = fontWidth - xStart;
            xLimit = fontWidth;
        } else {
            startWidth = width;
            xLimit = xStart + width;
        }

        /* Clipped, draw the right part of the first char. */
        drawChar(dest, charArray[nCharsToSkip], GXJ_RGB24TOPIXEL(pixel), xDest, yDest,
                 xStart, yCharSource, xLimit, yLimit,
                 FontBitmaps, fontWidth, fontHeight);
        nCharsToSkip++;
        xDest += startWidth;
        widthRemaining -= startWidth;
    }

    /* Draw all the fully wide chars. */
    for (i = nCharsToSkip; i < n && widthRemaining >= fontWidth;
         i++, xDest += fontWidth, widthRemaining -= fontWidth) {

        drawChar(dest, charArray[i], GXJ_RGB24TOPIXEL(pixel), xDest, yDest,
                 0, yCharSource, fontWidth, yLimit,
                 FontBitmaps, fontWidth, fontHeight);
    }

    if (i < n && widthRemaining > 0) {
        /* Clipped, draw the left part of the last char. */
        drawChar(dest, charArray[i], GXJ_RGB24TOPIXEL(pixel), xDest, yDest,
                 0, yCharSource, widthRemaining, yLimit,
                 FontBitmaps, fontWidth, fontHeight);
    }
}

/**
 * Obtains the ascent, descent and leading info for the font indicated.
 *
 * @param face The face of the font (Defined in <B>Font.java</B>)
 * @param style The style of the font (Defined in <B>Font.java</B>)
 * @param size The size of the font (Defined in <B>Font.java</B>)
 * @param ascent The font's ascent should be returned here.
 * @param descent The font's descent should be returned here.
 * @param leading The font's leading should be returned here.
 */
void
gx_get_fontinfo(int face, int style, int size, 
		int *ascent, int *descent, int *leading) {

    REPORT_CALL_TRACE(LC_LOWUI, "LCDUIgetFontInfo()\n");

    /* Surpress unused parameter warnings */
    (void)face;
    (void)size;
    (void)style;

    *ascent  = FontBitmaps[1][FONT_ASCENT];
    *descent = FontBitmaps[1][FONT_DESCENT];
    *leading = FontBitmaps[1][FONT_LEADING];
}

/**
 * Gets the advance width for the first n characters in charArray if
 * they were to be drawn in the font indicated by the parameters.
 *
 * <p>
 * <b>Reference:</b>
 * Related Java declaration:
 * <pre>
 *     charWidth(C)I
 * </pre>
 *
 * @param face The font face to be used (Defined in <B>Font.java</B>)
 * @param style The font style to be used (Defined in
 * <B>Font.java</B>)
 * @param size The font size to be used. (Defined in <B>Font.java</B>)
 * @param charArray The string to be measured
 * @param n The number of character to be measured
 * @return The total advance width in pixels (a non-negative value)
 */
int
gx_get_charswidth(int face, int style, int size, 
		  const jchar *charArray, int n) {

    REPORT_CALL_TRACE(LC_LOWUI, "LCDUIcharsWidth()\n");

    /* Surpress unused parameter warnings */
    (void)face;
    (void)size;
    (void)style;
    (void)charArray;

    return n * FontBitmaps[1][FONT_WIDTH];
}
#endif
