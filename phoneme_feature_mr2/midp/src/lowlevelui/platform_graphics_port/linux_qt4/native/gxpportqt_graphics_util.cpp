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

#include <qpixmap.h>
#include <qimage.h>
#include <string.h>
#include <midp_logging.h>
#include <midp_constants_data.h>

#include <gxutl_graphics.h>
#include "gxpportqt_intern_graphics_util.h"

void
get_transformed_pixmap(QPixmap* originalPixmap,
		       QPixmap* destPixmap,
		       int src_x, int src_y, 
		       int src_width, int src_height,
		       int transform,
		       bool hasAlpha) {

			   REPORT_INFO(LC_LOWUI, "gxpportqt_graphics_util:get_transformed_pixmap\n");

#ifdef FV_USE_QT4
    QImage originalImage = originalPixmap->toImage();
#else
    QImage originalImage = originalPixmap->convertToImage();
#endif
    if ( hasAlpha ) {
	// Qt's handling of the alpha channel in the conversion
	// process between QPixmap and QImage is buggy.
	// If the pixmap's pixels only have alpha values 0x00 and 0xFF
	// then the resulting QImage from conversion will return
	// false for hasAlphaBuffer().
	// so we set our own flag instead of depending on Qt to 
	// maintain alpha information.
	#ifdef FV_USE_QT4
	#else
	originalImage.setAlphaBuffer(TRUE);
    #endif
    }

    /*Qt gives us this useful API that returns a section of a QImage*/
    QImage sectionImage  = originalImage.copy(src_x, src_y, 
					      src_width, src_height);
    /* Skip this pixel-by-pixel copy if there is no transform */
    if (0 != transform) {
    	#ifdef FV_USE_QT4
    	QImage sectionImage32bpp = sectionImage;
    	#else
    	QImage sectionImage32bpp = sectionImage.convertDepth(32);
    	#endif
	
	QImage processedImage;
	
	
	int nXOriginSrc = 0;
	int nYOriginSrc = 0;
	int nWidth      = src_width;
	int nHeight     = src_height;
	
	/*scan length of the source image*/
	int imageWidth  = src_width;
	/*number of rows of the source image*/
	int imageHeight = src_height;
	
	int imgLen;
	int srcImgLen;
	
	int t_width;
	int t_height;
	
	int srcX;
	int srcY;
	int xStart;
	int yStart;
	int xIncr;
	int yIncr;
	int destX;
	int destY;
	int yCounter;
	int xCounter;
	
	int srcIndex;
	int destIndex;
	
	uchar* srcBits     = NULL;
	uchar* destBits    = NULL;
	
	uchar* srcBitsPtr  = NULL;
	uchar* destBitsPtr = NULL;
	
	
	/* set dimensions of image being created,
	   depending on transform */
	if (transform & TRANSFORM_INVERTED_AXES) {
	    t_width  = src_height;
	    t_height = src_width;
	} else {
	    t_width  = src_width;
	    t_height = src_height;
	}
	
	/* width * height * 4 gives us the size of a 32 bpp image */
	imgLen = nWidth * nHeight << 2;
	srcImgLen = imageWidth  * imageHeight << 2;
	
	/* Qt specific */
	#ifdef FV_USE_QT4
	processedImage = QImage(t_width, t_height, QImage::Format_ARGB32);
	#else
	processedImage.create(t_width, t_height, 32);
	#endif
	srcBits  = sectionImage32bpp.bits();
	destBits = processedImage.bits();
	/* ----------- */
	
	if (transform & TRANSFORM_Y_FLIP) {
	    yStart = nHeight-1;
	    yIncr = -1;
	} else {
	    yStart = 0;
	    yIncr = +1;
	}
	
	if (transform & TRANSFORM_X_FLIP) {
	    xStart = nWidth-1;
	    xIncr = -1;
	} else {
	    xStart = 0;
	    xIncr = +1;
	}
	
	srcBitsPtr  = srcBits;
	destBitsPtr = destBits;
	
	
	/* increment srcX,Y regular. increment destX,Y according to transform.
	   this makes handling of mask and alpha values easier */
	
	for (srcY = nYOriginSrc, destY = yStart, yCounter = 0; 
	     yCounter < nHeight; 
	     srcY++, destY+=yIncr, yCounter++) {
	    
	    /* in the current implementation we have source bitmap
	       dimension as the width of the image and the height of the region
	       destination bitmap is of the dimensions of the region */
	    
	    for (srcX = nXOriginSrc, destX = xStart, xCounter = 0; 
		 xCounter < nWidth; 
		 srcX++, destX+=xIncr, xCounter++) {
		
		if ( transform & TRANSFORM_INVERTED_AXES ) {
		    destIndex =  ( ( (destX) * t_width) + (destY) );
		} else {
		    destIndex =  ( ( (destY) * t_width) + (destX) );
		}
		
		destBitsPtr =  destBits + (destIndex * 4) ;
		
		srcIndex = (((srcY) * imageWidth) + (srcX));
		srcBitsPtr = srcBits + (srcIndex * 4);
		
		
		/* copy the pixel that is pointed to */
		*((int *)destBitsPtr) = *((int *)srcBitsPtr);
		
	    } /*for x*/
	    
	} /* for y */
	
	
	/* ---------- */
#ifdef FV_USE_QT4
    destPixmap->fromImage(processedImage);
#else
	if(TRUE == sectionImage.hasAlphaBuffer() ) {
	    processedImage.setAlphaBuffer(TRUE);
	} else {
	    processedImage.setAlphaBuffer(FALSE);
	}
	
	destPixmap->convertFromImage(processedImage);
#endif
    } else {
	/* No transform, just copy the image sub-section */
	#ifdef FV_USE_QT4
	destPixmap->fromImage(sectionImage);
	#else
	destPixmap->convertFromImage(sectionImage);
	#endif
    }
}

/*
 * Helper function to transform QImages
 *
 * @param srcQImage the QImage to be transformed
 * @param transform the transform to perform on the image
 */
void transform_image(QImage* srcQImage, int transform) {
    REPORT_INFO(LC_LOWUI, "gxpportqt_graphics_util:transform_image\n");
    /* make sure the source image depth is 32 */
    #ifdef FV_USE_QT4
    #else
    if (32 != srcQImage->depth()) {
        *srcQImage = srcQImage->convertDepth(32);
    }
    #endif

    int nXOriginSrc = 0;
    int nYOriginSrc = 0;
    int nWidth      = srcQImage->width();
    int nHeight     = srcQImage->height();
        
    /*scan length of the source image*/
    int imageWidth  = nWidth;
    /*number of rows of the source image*/
    int imageHeight = nHeight;
        
    int imgLen;
    int srcImgLen;
        
    int t_width;
    int t_height;
        
    int srcX;
    int srcY;
    int xStart;
    int yStart;
    int xIncr;
    int yIncr;
    int destX;
    int destY;
    int yCounter;
    int xCounter;
        
    int srcIndex;
    int destIndex;
        
    uchar* srcBits     = NULL;
    uchar* destBits    = NULL;
    
    uchar* srcBitsPtr  = NULL;
    uchar* destBitsPtr = NULL;
        
        
    /* set dimensions of image being created,
       depending on transform */
    if (transform & TRANSFORM_INVERTED_AXES) {
      t_width  = nHeight;
      t_height = nWidth;
    } else {
      t_width  = nWidth;
      t_height = nHeight;
    }
        
    /* width * height * 4 gives us the size of a 32 bpp image */
    imgLen = nWidth * nHeight << 2;
    srcImgLen = imageWidth  * imageHeight << 2;
        
    /* Qt specific */
    #ifdef FV_USE_QT4
    QImage processedQImage = QImage(t_width, t_height, QImage::Format_ARGB32);
    #else
    QImage processedQImage = QImage(t_width, t_height, 32);
    #endif

    srcBits  = srcQImage->bits();
    destBits = processedQImage.bits();
    /* ----------- */
    
    if (transform & TRANSFORM_Y_FLIP) {
      yStart = nHeight-1;
      yIncr = -1;
    } else {
      yStart = 0;
      yIncr = +1;
    }
    
    if (transform & TRANSFORM_X_FLIP) {
      xStart = nWidth-1;
      xIncr = -1;
    } else {
      xStart = 0;
      xIncr = +1;
    }
        
    srcBitsPtr  = srcBits;
    destBitsPtr = destBits;
        
        
    /* increment srcX,Y regular. increment destX,Y according to transform.
       this makes handling of mask and alpha values easier */
    
    for (srcY = nYOriginSrc, destY = yStart, yCounter = 0; 
         yCounter < nHeight; 
         srcY++, destY+=yIncr, yCounter++) {
      
      /* in the current implementation we have source bitmap
         dimension as the width of the image and the height of the region
         destination bitmap is of the dimensions of the region */
      
      for (srcX = nXOriginSrc, destX = xStart, xCounter = 0; 
           xCounter < nWidth; 
           srcX++, destX+=xIncr, xCounter++) {
        
        if ( transform & TRANSFORM_INVERTED_AXES ) {
          destIndex =  ( ( (destX) * t_width) + (destY) );
        } else {
          destIndex =  ( ( (destY) * t_width) + (destX) );
        }
        
        destBitsPtr =  destBits + (destIndex * 4) ;
        
        srcIndex = (((srcY) * imageWidth) + (srcX));
        srcBitsPtr = srcBits + (srcIndex * 4);
        
        
        /* copy the pixel that is pointed to */
        *((int *)destBitsPtr) = *((int *)srcBitsPtr);
        
      } /*for x*/
      
    } /* for y */
    
    
    /* ---------- */
    #ifdef FV_USE_QT4
    #else
    processedQImage.setAlphaBuffer(srcQImage->hasAlphaBuffer());
    #endif

    *srcQImage = processedQImage;
}
