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
 * 
 * This source file is specific for Qt-based configurations.
 */
#include <stddef.h>

#include <qpixmap.h>
#include <qimage.h>

#include <qteapp_export.h>
#include <midpMalloc.h>

#include <midpResourceLimit.h>

#include <gxutl_image.h>
#include <gxpport_immutableimage.h>
#include <gxpportqt_image.h>
#include "gxpportqt_intern_graphics_util.h"

/**
 * Structure for ImmutableImage data.
 */
typedef struct {

    /** Which routine allocated this image. */
    int marker;

    /** The platform version of Immutable Image */
    QImage* qimage;

    /** Cached pixmap used by UI */
    QPixmap* qpixmap;

} _Platform_ImmutableImage ;

static
bool load_raw(QImage** qimage,
	     gxutl_image_buffer_raw* rawImageBuffer, unsigned int length,
             jboolean isStatic, int* ret_Width, int* ret_Height);

/**
 * Calculate image resource size that should be used for resource limit
 * checking. Image color depth is hard-coded to 32 as it was found that 
 * color depth of resulting image was changed to 32-bit(from 16 or lower size) 
 * in most of the cases. 
 */ 
#define ImgRscSize(img) (((img)->width()*(img)->height()*32)>>3)

/**
 * Calculate image region resource size that should be used for resource limit
 * checking. Image color depth is hard-coded to 32 as it was found that 
 * color depth of resulting image was changed to 32-bit(from 16 or lower size) 
 * in most of the cases. 
 */ 
#define ImgRegionRscSize(img, width, height) ((width*height*32)>>3)

extern "C" void gxpport_createimmutable_from_mutable
(gxpport_mutableimage_native_handle srcMutableImagePtr,
 gxpport_image_native_handle *newImmutableImagePtr,
 gxutl_native_image_error_codes* creationErrorPtr) {
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_createimmutable_from_mutable\n");

    /* Convert from source QPixmap to destination QImage */
    QPixmap* srcPixmap = gxpportqt_get_mutableimage_pixmap(srcMutableImagePtr);

    int rscSize = ImgRscSize(srcPixmap); /* new img is the same size */

    /* Check resource limit before copying */
    if (midpCheckResourceLimit(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        /* Exceeds resource limit */
        *creationErrorPtr  = GXUTL_NATIVE_IMAGE_RESOURCE_LIMIT;
        return;
    }

    _Platform_ImmutableImage* immutableImage =
        (_Platform_ImmutableImage*)midpMalloc(sizeof(_Platform_ImmutableImage));
    if (immutableImage == NULL) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }
#ifdef FV_USE_QT4
    immutableImage->qimage = new QImage(srcPixmap->toImage());
#else
    immutableImage->qimage = new QImage(srcPixmap->convertToImage());
#endif
    if (NULL == immutableImage->qimage) {
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    if (immutableImage->qimage->isNull()) {
        delete immutableImage->qimage;
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;    
    }

    /* Copying succeeds */
    if (midpIncResourceCount(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        REPORT_ERROR(LC_LOWUI, "Error in updating resource limit"
                     " for Immutable image");
    }

    immutableImage->marker = 1;
    immutableImage->qpixmap = NULL;

    *newImmutableImagePtr = immutableImage;
    *creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
}

extern "C" void gxpport_createimmutable_from_immutableregion
(gxpport_image_native_handle srcImmutableImagePtr,
 int src_x, int src_y, 
 int src_width, int src_height,
 int transform,
 gxpport_image_native_handle *newImmutableImagePtr,
 gxutl_native_image_error_codes* creationErrorPtr) {
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_createimmutable_from_immutableregion\n");

    _Platform_ImmutableImage* srcImmutableImage = 
      (_Platform_ImmutableImage*)srcImmutableImagePtr;

    int rscSize = ImgRegionRscSize(srcImmutableImage->qimage,
                   src_width, src_height);

    /* Check with resource limit */
    if (midpCheckResourceLimit(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        /* Exceed Resource limit */
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_RESOURCE_LIMIT;
        return;
    }

    _Platform_ImmutableImage* immutableImage =
      (_Platform_ImmutableImage*)midpMalloc(sizeof(_Platform_ImmutableImage));
    if (immutableImage == NULL) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    immutableImage->qimage =
        new QImage(srcImmutableImage->qimage->copy(src_x, 
                                                       src_y, 
                                                       src_width, 
                                                       src_height));
    if (NULL == immutableImage->qimage) {
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    if (immutableImage->qimage->isNull()) {
        delete immutableImage->qimage;
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;    
    }

    if (transform != 0) {
        transform_image(immutableImage->qimage, transform);
    }

    /* Copy succeeds */
    if (midpIncResourceCount(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        REPORT_ERROR(LC_LOWUI, "Error in updating resource limit"
                     " for Immutable image");
    }

    immutableImage->marker =2;
    immutableImage->qpixmap = NULL;

    *newImmutableImagePtr = immutableImage;
    *creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
}

extern "C" void gxpport_createimmutable_from_mutableregion
(gxpport_mutableimage_native_handle srcMutableImagePtr,
 int src_x, int src_y, 
 int src_width, int src_height,
 int transform,
 gxpport_image_native_handle* newImmutableImagePtr,
 gxutl_native_image_error_codes* creationErrorPtr) {
	REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_createimmutable_from_mutableregion\n");


    QPixmap *srcqpixmap =
    gxpportqt_get_mutableimage_pixmap(srcMutableImagePtr);

    int rscSize = ImgRegionRscSize(srcqpixmap, src_width, src_height);

    /* Check resource limit */
    if (midpCheckResourceLimit(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        /* Exceed Resource limit */
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_RESOURCE_LIMIT;
        return;
    }

#ifdef FV_USE_QT4
    QImage srcQImage = srcqpixmap->toImage();
#else
    QImage srcQImage = srcqpixmap->convertToImage();
#endif
    if (srcQImage.isNull()) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    _Platform_ImmutableImage* immutableImage = 
      (_Platform_ImmutableImage*)midpMalloc(sizeof(_Platform_ImmutableImage));
    if (immutableImage == NULL) {
	*creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
	return;
    }

    immutableImage->qimage = new QImage(srcQImage.copy(src_x,
                                                       src_y, 
                                                       src_width, 
                                                       src_height));
    if (NULL == immutableImage->qimage) {
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    if (immutableImage->qimage->isNull()) {
        delete immutableImage->qimage;
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    if (transform != 0) {
        transform_image(immutableImage->qimage, transform);
    }

    /* Copy succeeds */
    if (midpIncResourceCount(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        REPORT_ERROR(LC_LOWUI, "Error in updating resource limit"
                     " for Immutable image");
    }

    immutableImage->marker = 3;
    immutableImage->qpixmap = NULL;
    
    *newImmutableImagePtr = immutableImage;
    *creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
}

extern "C" void gxpport_decodeimmutable_from_selfidentifying
(unsigned char* srcBuffer, int length, 
 int* imgWidth, int* imgHeight,
 gxpport_image_native_handle *newImmutableImagePtr,
 gxutl_native_image_error_codes* creationErrorPtr) {
 
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_decodeimmutable_from_selfidentifying\n");
    MIDP_ERROR err;
    gxutl_image_format format;
    unsigned int w, h;

    err = gxutl_image_get_info(srcBuffer, (unsigned int)length,
			       &format, &w, &h);

    if (err != MIDP_ERROR_NONE || format == GXUTL_IMAGE_FORMAT_UNSUPPORTED) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_UNSUPPORTED_FORMAT_ERROR;
        return;
    }

    /* Check resource limit */
    int rscSize = ImgRegionRscSize(NULL, w, h);

    if (midpCheckResourceLimit(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        /* Exceed Resource limit */
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_RESOURCE_LIMIT;
        return;
    }

    bool loadResult = FALSE;
    
    _Platform_ImmutableImage* immutableImage =
        (_Platform_ImmutableImage*)midpMalloc(sizeof(_Platform_ImmutableImage));
    if (immutableImage == NULL) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    switch(format) {

    case GXUTL_IMAGE_FORMAT_JPEG:
    case GXUTL_IMAGE_FORMAT_PNG:
	case GXUTL_IMAGE_FORMAT_GIF:
        immutableImage->qimage = new QImage();
        if (NULL == immutableImage->qimage) {
            *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
            return;
        }

        if (immutableImage->qimage->loadFromData((uchar*)srcBuffer, 
                                                 (unsigned int)length)) {

            *imgWidth  = immutableImage->qimage->width();
            *imgHeight = immutableImage->qimage->height();

            if ((0 == *imgWidth) || (0 == *imgHeight)) {
                delete immutableImage->qimage;
            } else {
		immutableImage->marker = 4;
                loadResult = TRUE;
            }
        } else {
            delete immutableImage->qimage;
            immutableImage->qimage = NULL;
        }

        break;

    case GXUTL_IMAGE_FORMAT_RAW:
        loadResult = load_raw(&immutableImage->qimage,
                             (gxutl_image_buffer_raw*)srcBuffer, 
                             (unsigned int)length, KNI_FALSE,
                             imgWidth, imgHeight);
	immutableImage->marker = 5;
        break;

    default:
	/* Shouldn't be here */
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_UNSUPPORTED_FORMAT_ERROR;
        return;
    }

    if (!loadResult) {
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_DECODING_ERROR;
        return;
    }
        
    /* Image creation succeeds */
    if (midpIncResourceCount(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
            REPORT_ERROR(LC_LOWUI, "Error in updating resource limit"
                         " for Immutable image");
    }

    immutableImage->qpixmap = NULL;

    *newImmutableImagePtr = immutableImage;
    *creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
}

extern "C" void gxpport_decodeimmutable_from_argb
(jint* srcBuffer,
 int width, int height,
 jboolean processAlpha,
 gxpport_image_native_handle* newImmutableImagePtr,
 gxutl_native_image_error_codes* creationErrorPtr) {

 
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_decodeimmutable_from_argb\n");

    // use bytes as is, QT images have the same binary data layout
#ifdef FV_USE_QT4
    (void)processAlpha;
    QImage qimage = QImage((unsigned char*)srcBuffer, 
                           width, height, QImage::Format_ARGB32);
#else
    QImage qimage = QImage((unsigned char*)srcBuffer, 
                           width, height, 
                           IMAGE_DEPTH,
                           NULL, 0, 
                           QImage::IgnoreEndian);
#endif
    if (qimage.isNull()) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_DECODING_ERROR;
        return;
    }

    /* Check Resource limit */
    int rscSize = ImgRscSize(&qimage);

    if (midpCheckResourceLimit(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        /* Exceed Resource limit */
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_RESOURCE_LIMIT;
        return;
    }
#ifdef FV_USE_QT4
#else
    qimage.setAlphaBuffer(processAlpha == KNI_TRUE?TRUE:FALSE);
#endif
    _Platform_ImmutableImage* immutableImage =
        (_Platform_ImmutableImage*)midpMalloc(sizeof(_Platform_ImmutableImage));
    if (immutableImage == NULL) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    // make a deep copy of the image
    immutableImage->qimage = new QImage(qimage.copy());
    if (NULL == immutableImage->qimage) {
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    if (immutableImage->qimage->isNull()) {
        delete immutableImage->qimage;
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    /* Image creation succeeds */
    if (midpIncResourceCount(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        REPORT_ERROR(LC_LOWUI, "Error in updating "
                     "resource limit for Immutable image");
    }

    immutableImage->marker = 6;
    immutableImage->qpixmap = NULL;

    *newImmutableImagePtr = immutableImage;
    *creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
}

extern "C" void gxpport_render_immutableimage
(gxpport_image_native_handle immutableImagePtr,
 gxpport_mutableimage_native_handle dstMutableImagePtr,
 const jshort *clip,
 jint x_dest, jint y_dest) {

 
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_render_immutableimage\n");

    _Platform_ImmutableImage* immutableImage = 
        (_Platform_ImmutableImage*)immutableImagePtr;
    QPixmap *qpixmapDst =
      gxpportqt_get_mutableimage_pixmap(dstMutableImagePtr);

    if (NULL == immutableImage || immutableImage->qimage->isNull()) {
        /* not a valid image should not happen, log this */
		REPORT_INFO(LC_LOWUI, "==========NULL == immutableImage || immutableImage->qimage->isNull(\n");
        return;
    }

    REPORT_INFO(LC_LOWUI, "==========start render...\n");
    MScreen * mscreen = qteapp_get_mscreen();
	#if 1
    QPainter *gc = mscreen->setupGC(-1, -1, clip, 
                                    (QPaintDevice *)qpixmapDst, 
                                    0);
    #ifdef FV_USE_QT4
            gc->drawImage(x_dest, y_dest, *immutableImage->qimage);
        return;
    #else
    if (immutableImage->qimage->hasAlphaBuffer()) {
        gc->drawImage(x_dest, y_dest, *immutableImage->qimage);
        return;
    }

    gc->drawPixmap(x_dest, y_dest,
        *(gxpportqt_get_immutableimage_pixmap(immutableImagePtr)));
    #endif
	#else
    mscreen->setupGCParas(-1,-1,clip,(QPaintDevice*)qpixmapDst,0,x_dest, y_dest, 0, 0);
    mscreen->setGCType(1);
	mscreen->setupImage(*immutableImage->qimage);
    mscreen->gcUpdate();

	immutableImage->qimage->save("test.png");	

	#endif
}

extern "C" void gxpport_render_immutableregion
(gxpport_image_native_handle immutableImagePtr,
 gxpport_mutableimage_native_handle dstMutableImagePtr,
 const jshort *clip,
 jint x_dest, jint y_dest, 
 jint width, jint height,
 jint x_src, jint y_src,
 jint transform) {
 
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_render_immutableregion\n");

    _Platform_ImmutableImage* immutableImage = 
        (_Platform_ImmutableImage*)immutableImagePtr;
    QPixmap *qpixmapDst =
    gxpportqt_get_mutableimage_pixmap(dstMutableImagePtr);

    if (NULL == immutableImagePtr || immutableImage->qimage->isNull()) {
      /* not a valid image should not happen, log this */
        return;
    }

    MScreen * mscreen = qteapp_get_mscreen();
    QPainter *gc = mscreen->setupGC(-1, -1, clip, 
                                    (QPaintDevice *)qpixmapDst, 
                                    0);
    if (0 == transform) {
        gc->drawImage(x_dest, y_dest,
                      *immutableImage->qimage,
                      x_src, y_src,
                      width, height,
                      0);
        return;
    }

    QImage image = immutableImage->qimage->copy(x_src, y_src, 
                                               width, height);

    transform_image(&image, transform);
    
    if (!image.isNull()) {
        gc->drawImage(x_dest, y_dest, image);
    }
}


extern "C" void gxpport_get_immutable_argb
(gxpport_image_native_handle immutableImagePtr,
 jint* rgbBuffer, int offset, int scanLength,
 int x, int y, int width, int height,
 gxutl_native_image_error_codes* errorPtr) {
 
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_get_immutable_argb\n");

    _Platform_ImmutableImage* immutableImage = 
        (_Platform_ImmutableImage*)immutableImagePtr;

    if (NULL == immutableImage || immutableImage->qimage->isNull()) {
      /* not a valid image should not happen, log this */
      return ;
    }

    QImage   image;
    int      curOffset = offset;      /* current offset in output array */
    int      curX, curY;
    unsigned int r,g,b;
    QRgb     pixel;

#ifdef FV_USE_QT4
    image = *immutableImage->qimage;
    if (TRUE) {
#else
    if (IMAGE_DEPTH != immutableImage->qimage->depth()) {
        image = immutableImage->qimage->convertDepth(IMAGE_DEPTH);
    } else {
        image = *immutableImage->qimage;
    }

    if (image.hasAlphaBuffer() == TRUE) {
#endif
        for (curY = y; curY < y + height; curY++) {
            for (curX = x; curX < x + width; curX++) {
                
                // Obtain the R,G,B
                pixel = image.pixel(curX, curY);
                
                r = qRed(pixel) ;
                g = qGreen(pixel);
                b = qBlue(pixel);
            
                rgbBuffer[curOffset] = (qAlpha(pixel) << 24)
                    | ((r & 0xff) << 16) 
                    | ((g & 0xff) << 8) 
                    |  (b & 0xff);
                
                curOffset++;
            }

            curOffset += (scanLength - width);
        }
        
    } else {
        
        for (curY = y; curY < y + height; curY++) {
            for (curX = x; curX < x + width; curX++) {
                
                // Obtain the R,G,B
                pixel = image.pixel(curX, curY);
                
                r = qRed(pixel) ;
                g = qGreen(pixel);
                b = qBlue(pixel);
                
                rgbBuffer[curOffset] = (0xff << 24)
                    | ((r & 0xff) << 16) 
                    | ((g & 0xff) << 8) 
                    |  (b & 0xff);
                
                curOffset++;
            }
            
            curOffset += (scanLength - width);
        }
        
    }

    *errorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
}


extern "C"
void gxpport_destroy_immutable(gxpport_image_native_handle imagePtr) {

	REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_destroy_immutable\n");


    _Platform_ImmutableImage* immutableImage = 
        (_Platform_ImmutableImage*)imagePtr;

    if (NULL != immutableImage) {
        if (NULL != immutableImage->qimage) {
            int rscSize = ImgRscSize(immutableImage->qimage);

            /* Update the resource count */
            if (midpDecResourceCount(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
                REPORT_ERROR(LC_LOWUI,"Error in updating resource limit for"
                             " Immutable image");
            }

            delete immutableImage->qimage;
        }

        if (NULL != immutableImage->qpixmap) {
            delete immutableImage->qpixmap;
        }

	midpFree(immutableImage);
    }
}

extern "C" void gxpport_decodeimmutable_to_platformbuffer
(unsigned char* srcBuffer, long length, 
 unsigned char** ret_dataBuffer, long* ret_length,
 gxutl_native_image_error_codes* creationErrorPtr) {

 
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_decodeimmutable_to_platformbuffer\n");

    gxutl_image_format format;
    MIDP_ERROR err;
    unsigned int w, h;

    err = gxutl_image_get_info(srcBuffer, (unsigned int)length,
			       &format, &w, &h);
    
    switch (err) {

    case MIDP_ERROR_NONE:
	break; /* continue */

    case MIDP_ERROR_IMAGE_CORRUPTED:
	*creationErrorPtr = GXUTL_NATIVE_IMAGE_DECODING_ERROR;
	return;
    
    default:
	*creationErrorPtr = GXUTL_NATIVE_IMAGE_UNSUPPORTED_FORMAT_ERROR;
	return;
    }
		     
    switch (format) {

    case GXUTL_IMAGE_FORMAT_RAW:
        /* already in RAW format. make a copy */
	{
	    unsigned char* dataBuffer = (unsigned char*) midpMalloc(length);
            
	    if (NULL == dataBuffer) {
		*creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
	    } else {
                
		memcpy(dataBuffer, srcBuffer, length);
                
		*ret_dataBuffer = dataBuffer;
		*ret_length = length;

		*creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
	    }
	}
        break;

    case GXUTL_IMAGE_FORMAT_JPEG:
    case GXUTL_IMAGE_FORMAT_PNG:
        {
            QImage qimage;
            
            if (qimage.loadFromData((uchar*)srcBuffer, 
                                    (unsigned int)length)) {
                int imgWidth  = qimage.width();
                int imgHeight = qimage.height();
                
                if ((0 == imgWidth) || (0 == imgHeight)) {
                    *creationErrorPtr = GXUTL_NATIVE_IMAGE_DECODING_ERROR;
                } else {
                    QImage image;
                    #ifdef FV_USE_QT4
					    REPORT_INFO1(LC_LOWUI, "=====qimage.depth() is %d\n",qimage.depth());
					    image = qimage;
                    #else
                    if (IMAGE_DEPTH != qimage.depth()) {
                        image = qimage.convertDepth(IMAGE_DEPTH);
                    } else {
                        image = qimage;
                    }
                    #endif
                    *ret_length = offsetof(gxutl_image_buffer_raw, data)
				+ image.numBytes();

                    gxutl_image_buffer_raw *dataBuffer =
			(gxutl_image_buffer_raw *) midpMalloc(*ret_length);
                    
                    if (NULL == dataBuffer) {
                        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
                    } else {
                        dataBuffer->width    = (unsigned int)imgWidth;
                        dataBuffer->height   = (unsigned int)imgHeight;
                        #ifdef FV_USE_QT4
                        dataBuffer->hasAlpha = true;
                        #else
                        dataBuffer->hasAlpha = (unsigned int)image.hasAlphaBuffer();
                        #endif
                        memcpy(dataBuffer->header, gxutl_raw_header, 4);
                        memcpy(dataBuffer->data, image.bits(), image.numBytes());
                        
                        *ret_dataBuffer = (unsigned char *)dataBuffer;
                        *creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
                    }
                }
            } else {
                *creationErrorPtr = GXUTL_NATIVE_IMAGE_DECODING_ERROR;
            }
        }
        break;
    
    default:
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_UNSUPPORTED_FORMAT_ERROR;
        break;

    } /* switch (format) */
}

extern "C" void gxpport_loadimmutable_from_platformbuffer
(unsigned char* srcBuffer, int length, jboolean isStatic,
 int* ret_imgWidth, int* ret_imgHeight,
 gxpport_image_native_handle *newImmutableImagePtr,
 gxutl_native_image_error_codes* creationErrorPtr) {

 
 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpport_loadimmutable_from_platformbuffer\n");

    int rscSize;
    gxutl_image_buffer_raw* dataBuffer = (gxutl_image_buffer_raw*)srcBuffer;

    /* Check resource limit */
    rscSize = ImgRegionRscSize(NULL, dataBuffer->width, dataBuffer->height);
    if (midpCheckResourceLimit(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        /* Exceed Resource limit */
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_RESOURCE_LIMIT;
        return;
    }

    _Platform_ImmutableImage* immutableImage =
        (_Platform_ImmutableImage*)midpMalloc(sizeof(_Platform_ImmutableImage));
    if (immutableImage == NULL) {
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_OUT_OF_MEMORY_ERROR;
        return;
    }

    if (!load_raw(&immutableImage->qimage,
                dataBuffer,
                length, isStatic,
                ret_imgWidth, ret_imgHeight)) {
	midpFree(immutableImage);
        *creationErrorPtr = GXUTL_NATIVE_IMAGE_DECODING_ERROR;
        return;
    }

    /* Image creation succeeds */
    if (midpIncResourceCount(RSC_TYPE_IMAGE_IMMUT, rscSize) == 0) {
        REPORT_ERROR(LC_LOWUI, "Error in updating resource limit"
                     " for Immutable image");
    }
            
    immutableImage->marker = 7;
    immutableImage->qpixmap = NULL;

    *newImmutableImagePtr = immutableImage;
    *creationErrorPtr = GXUTL_NATIVE_IMAGE_NO_ERROR;
}


extern "C" QPixmap* gxpportqt_get_immutableimage_pixmap
(gxpport_image_native_handle immutableImagePtr) {

	REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:gxpportqt_get_immutableimage_pixmap\n");


    _Platform_ImmutableImage* immutableImage = 
        (_Platform_ImmutableImage*)immutableImagePtr;

    if (NULL == immutableImage || NULL == immutableImage->qimage ||
        immutableImage->qimage->isNull()) {
        return NULL;
    }

    if (NULL != immutableImage->qpixmap) {
        return immutableImage->qpixmap;
    }

    immutableImage->qpixmap = new QPixmap();
    if (NULL == immutableImage->qpixmap) {
        return NULL;
    }
            
    immutableImage->qpixmap->convertFromImage(*immutableImage->qimage);
    if (!immutableImage->qpixmap->isNull()) {
        return immutableImage->qpixmap;
    }

    delete immutableImage->qpixmap;
    immutableImage->qpixmap = NULL;
    return NULL;
}

/*
 * loads a RAW image into a QImage 
 *
 * @param qimage pointer to destination QImage
 * @param rawImageBuffer source RAW image
 * @param length in bytes of the input image
 * @param isStatic true if the rawImageBuffer is static
 * @return ret_Width the width of the decoded image
 * @return ret_Height the height of the decoded image
 */
static 
bool load_raw(QImage** qimage, gxutl_image_buffer_raw* rawImageBuffer, 
	      unsigned int length,
             jboolean isStatic, int* ret_Width, int* ret_Height) {

			 
			 REPORT_INFO(LC_LOWUI, "gxpportqt_immutableimage:load_raw\n");

    bool retVal = FALSE;
    
    /* Check buffer size */
    if (offsetof(gxutl_image_buffer_raw, data)
	+ rawImageBuffer->width * rawImageBuffer->height * 4
	== length) {
#ifdef FV_USE_QT4
        QImage* localQimage = new QImage(rawImageBuffer->data,
                                         rawImageBuffer->width,
                                         rawImageBuffer->height,
                                         QImage::Format_ARGB32);
#else
	QImage* localQimage = new QImage(rawImageBuffer->data,
                                    rawImageBuffer->width,
				    rawImageBuffer->height, 
                                    IMAGE_DEPTH,
                                    NULL, 0, 
                                    QImage::IgnoreEndian);
    #endif
	if (localQimage != NULL) {
            if (!localQimage->isNull()) {
            	#ifdef FV_USE_QT4
            	#else
                localQimage->setAlphaBuffer(rawImageBuffer->hasAlpha);
                #endif
                if (isStatic) {
                    *qimage = localQimage;
                } else {
                    *qimage = new QImage(localQimage->copy());
                    delete localQimage;
                }

                if (*qimage != NULL) {
                    *ret_Width  = (*qimage)->width();
                    *ret_Height = (*qimage)->height();
                    if ((0 != *ret_Width) && (0 != *ret_Height)) {
                        retVal = TRUE;
                    } else {
                        delete *qimage;
                    }
                }
            }
	}
    }
     
    return retVal;
}
