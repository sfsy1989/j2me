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

#include <kni.h>
#include <jvmspi.h>
#include <sni.h>

#include <QtGui>

#include "lfpport_qte_util.h"

#include <qapplication.h>
#include <qmainwindow.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#ifdef FV_USE_QT4
#include <qevent.h>
#else
#include <qpaintdevicemetrics.h>
#endif
#include <qlayout.h>
#include <qtoolbar.h>
#include <qpalette.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qsizepolicy.h>

#include <midp_constants_data.h>
#include <keymap_input.h>
#include <midpServices.h>
#include <midpMalloc.h>
#include <midpString.h>
#include <midpEventUtil.h>
#include <lfpport_font.h>
#include <lfp_registry.h>

#include <qteapp_key.h>
#include "lfpport_qte_mscreen.h"
#include "lfpport_qte_mainwindow.h"
#include <moc_lfpport_qte_mscreen.cpp>

/**
 * @file  lfpport_qte_mscreen.cpp
 *
 * A frame-less widget that all Displayables are rendered on.
 */


jboolean PlatformMScreen::r_orientation = false;

/**
 * A frame-less widget that all Displayables are rendered on.
 */
 #ifdef FV_USE_QT4
 PlatformMScreen::PlatformMScreen(QWidget *parent, const char* name):QFrame(parent){
 (void)name;
 #else
PlatformMScreen::PlatformMScreen(QWidget *parent, const char* name) :QScrollView(parent, name) {
#endif
  vm_stopped = false;

  // MainWindow already has frame, no frame for MScreen
  //setFrameStyle(QFrame::NoFrame);
  //setWindowFlags(Qt::Window);

  // Graphics context
  gc = new QPainter(this);
  mscreen_pixmap = new QPixmap(640,530);
  //mscreen_image = NULL;
  //old_mscreen_image = NULL;
  qpixmap = QPixmap(640,530);

  force_refresh = true;
  last_pen = -1;
  last_brush = -1;
  last_dotted = 0;

  gc_flag = 0;

#if 1
  QPalette pal;
  setGeometry(QRect(0, 0, 640, 530));
  pal.setColor(QPalette::Background, QColor(0,0,0,0));
  setPalette(pal);

  setAutoFillBackground(TRUE);
#endif
  setAttribute(Qt::WA_OpaquePaintEvent);
  setAttribute(Qt::WA_NoSystemBackground);

  TRACE_MSC(  PlatformMScreen::MScreen..);
  connect(&vm_slicer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
  TRACE_MSC(..PlatformMScreen::MScreen);
}


/**
 * full screen mode in Qt does not work nicely with menu bar
 * so we have to fall back to the design where full screen
 * in Qt platform widget means only ticker is gone.
 */
void PlatformMScreen::init() {
    TRACE_MSC(  PlatformMScreen::init..);
	#if 1
#ifdef FV_USE_QT4
        setFocusPolicy(Qt::ClickFocus);
#else
    setFocusPolicy(QWidget::ClickFocus);
    
    // Always ...
    setHScrollBarMode(QScrollView::AlwaysOff);
#endif

#if 1
    DISPLAY_WIDTH       = NORMALWIDTH;
    DISPLAY_HEIGHT      = NORMALHEIGHT;

    SCREEN_WIDTH        = NORMALWIDTH - VERT_SCROLLBAR_WIDTH;
    SCREEN_HEIGHT       = NORMALHEIGHT;

    DISPLAY_FULLWIDTH   = FULLWIDTH;
    DISPLAY_FULLHEIGHT  = FULLHEIGHT;

    /*
     * Use the values from constants.xml. No more dynamic querying.
     *
    // Measure without scrollbar
    setVScrollBarMode(QScrollView::AlwaysOff);
    DISPLAY_WIDTH  = visibleWidth();  // width for full and normal screen mode
    DISPLAY_HEIGHT = visibleHeight(); // height for normal mode

    // Measure with scrollbar
    setVScrollBarMode(QScrollView::AlwaysOn);
    SCREEN_WIDTH  = visibleWidth();  // width that should be used for layout
    SCREEN_HEIGHT = visibleHeight(); // height that form should return

    // Special case, see above for explanation;
    // for canvas: no scroll, no ticker
    DISPLAY_FULLWIDTH  = DISPLAY_WIDTH;
    DISPLAY_FULLHEIGHT = DISPLAY_HEIGHT +
                         MainWindow->getTicker()->sizeHint().height();

    // for canvas: no border in layout and no spacing between mscreen and ticker
    QLayout *layout = parentWidget() == NULL ? NULL : parentWidget()->layout();
    if (layout != NULL) {
      DISPLAY_FULLWIDTH  += 2*layout->margin();
      DISPLAY_FULLHEIGHT += 2*layout->margin() + layout->spacing();
    }
    */

#ifdef FV_USE_QT4
#else
    setVScrollBarMode(QScrollView::Auto);
#endif
    // Set the size of the midlet suites app area
    //setFixedSize(getDisplayWidth(), getDisplayHeight());
    setFixedSize(640,530);
#ifdef FV_USE_QT4
    //qpixmap = qpixmap.copy(0,0,getDisplayWidth(), getDisplayHeight());
    qpixmap = qpixmap.copy(0,0,640, 530);
#else
    //qpixmap.resize(getDisplayWidth(), getDisplayHeight());
    qpixmap.resize(640, 530);
#endif
    qpixmap.fill(Qt::black); // Qt::white is default
#endif

#endif

    TRACE_MSC(..PlatformMScreen::init);
}


/**
 * Resize the buffer size (either normal or fullscreen)
 *
 * @param newSize Specify the size of the screen
 */
void PlatformMScreen::setBufferSize(BufferSize newSize)
{
    if (newSize == fullScreenSize) {
        if (gc->isActive()) {
            gc->end();
        }
        #ifdef FV_USE_QT4
        qpixmap = qpixmap.copy(0,0,getDisplayFullWidth(), getDisplayFullHeight());
        #else
        qpixmap.resize(getDisplayFullWidth(), getDisplayFullHeight());
        #endif
    } else {
        #ifdef FV_USE_QT4
        qpixmap = qpixmap.copy(0,0,getDisplayWidth(), getDisplayHeight());
        #else
        qpixmap.resize(getDisplayWidth(), getDisplayHeight());
        #endif
    }

    // Whether current Displayable won't repaint the entire screen on
    // resize event, the artefacts from the old screen content can appear.
    // That's why the buffer content is not preserved.
    qpixmap.fill(); // Qt::white is default
}

/**
 * Start VM by starting a time share request for it.
 */
void PlatformMScreen::startVM() {
    vm_stopped = false;
    key_press_count = 0;

    // Setup next VM time slice to happen immediately
    setNextVMTimeSlice(0);
}

/**
 * Stop VM by stopping requests for VM time slice.
 * Any leftover UI resource will be freed also.
 */
void PlatformMScreen::stopVM() {
    // Stop any further VM time slice
    setNextVMTimeSlice(-1);

    // Clean up any leftover native UI resources since VM is exiting
    MidpDeleteAllComponents();

    // Clean up any leftovers of native font resources
    lfpport_font_finalize();
}

PlatformMScreen::~PlatformMScreen()
{
#ifdef FV_USE_QT4
#else
    killTimers();
#endif
    delete gc;
    gc = NULL;

    delete mscreen_pixmap;
    mscreen_pixmap = NULL;
}

/**
 * Handle mouse press event in the VIEWPORT
 *
 * @param mouse QT's QMouseEvent
 */
 #ifdef FV_USE_QT4
 #else
void PlatformMScreen::viewportMousePressEvent(QMouseEvent *mouse)
{

    MidpEvent evt;

    MIDP_EVENT_INITIALIZE(evt);

    evt.type = MIDP_PEN_EVENT;
    evt.ACTION = KEYMAP_STATE_PRESSED;
    evt.X_POS = mouse->x();
    evt.Y_POS = mouse->y();

    midpStoreEventAndSignalForeground(evt);

}


/**
 * Handle mouse move event in the VIEWPORT
 *
 * @param mouse QT's QMouseEvent
 */
void PlatformMScreen::viewportMouseMoveEvent(QMouseEvent *mouse)
{
    MidpEvent evt;

    MIDP_EVENT_INITIALIZE(evt);

    evt.type = MIDP_PEN_EVENT;
    evt.ACTION = KEYMAP_STATE_DRAGGED;
    evt.X_POS = mouse->x();
    evt.Y_POS = mouse->y();

    midpStoreEventAndSignalForeground(evt);
}

/**
 * Handle mouse move release in the VIEWPORT
 *
 * @param mouse QT's QMouseEvent
 */
void PlatformMScreen::viewportMouseReleaseEvent(QMouseEvent *mouse)
{
    MidpEvent evt;

    MIDP_EVENT_INITIALIZE(evt);


    evt.type = MIDP_PEN_EVENT;
    evt.ACTION = KEYMAP_STATE_RELEASED;
    evt.X_POS = mouse->x();
    evt.Y_POS = mouse->y();

    midpStoreEventAndSignalForeground(evt);
}
#endif

/**
 * Handle key press event
 *
 * @param key QT's QKeyEvent
 * @see keyReleaseEvent or <a href="http://doc.trolltech.com/qtopia/html/qwidget.html#0a4482">online menu.</a>
 */
void PlatformMScreen::keyPressEvent(QKeyEvent *key)
{
    key_press_count += 1;
#if ENABLE_MULTIPLE_ISOLATES
    if (key->key() == Qt::Key_F12 ||
        key->key() == Qt::Key_Home) {
        // F12 to display the foreground selector
        if (!key->isAutoRepeat()) {
            MidpEvent evt;
            MIDP_EVENT_INITIALIZE(evt);
            evt.intParam1 = 0;
            evt.type = SELECT_FOREGROUND_EVENT;
            midpStoreEventAndSignalAms(evt);
        }
#ifdef QT_KEYPAD_MODE
    } else if (key->key() == Qt::Key_Flip) {
#else
    } else if (key->key() == Qt::Key_F4) {
#endif
        if (!key->isAutoRepeat()) {
            MidpEvent evt;
            MIDP_EVENT_INITIALIZE(evt);
            evt.type = SELECT_FOREGROUND_EVENT;
            evt.intParam1 = 1;
            midpStoreEventAndSignalAms(evt);
        }
    }
#else
    // F12 pause or activate all Java apps
    if ((key->key() == Qt::Key_F12 || key->key() == Qt::Key_Home) &&
        !key->isAutoRepeat()) {
        pauseAll();
    }
#endif
    else {
        MidpEvent evt;
        MIDP_EVENT_INITIALIZE(evt);
        if ((evt.CHR = mapKey(key)) != KEYMAP_KEY_INVALID) {
            if (evt.CHR == KEYMAP_KEY_SCREEN_ROT) {
                evt.type = ROTATION_EVENT;
            } else {
                evt.type = MIDP_KEY_EVENT;
            }
            evt.ACTION = key->isAutoRepeat() ? 
                KEYMAP_STATE_REPEATED : KEYMAP_STATE_PRESSED;
            handleKeyEvent(evt);
        }
    }
}

/**
 * Handle key release event
 *
 * @param key QT's QKeyEvent
 * @see keyPressEvent or <a href="http://doc.trolltech.com/qtopia/html/qwidget.html#4b620f">online menu.</a>
 */
void PlatformMScreen::keyReleaseEvent(QKeyEvent *key)
{
    if (key_press_count == 0 || key->isAutoRepeat()) {
        // We may have a left-over keyReleaseEvent from a previous
        // invocation of the VM! Also, Zaurus SL-5500 hardware/OS
        // has defect that some hard buttons only generate key release
        // events without key press event.
        return;
    }

    MidpEvent evt;

    MIDP_EVENT_INITIALIZE(evt);

    if ((evt.CHR = mapKey(key)) != KEYMAP_KEY_INVALID) {
        evt.type = MIDP_KEY_EVENT;
        evt.ACTION = KEYMAP_STATE_RELEASED;
        handleKeyEvent(evt);
    }

    if (key_press_count > 0) {
        key_press_count -= 1;
    }
}

/**
 * Handle (all) the key events.
 *
 * @param evt MIDP event
 */
void PlatformMScreen::handleKeyEvent(MidpEvent evt)
{
  if (evt.type != MIDP_INVALID_EVENT) {
    midpStoreEventAndSignalForeground(evt);
  }
}

/**
 * Repaint current screen upon system notification
 *
 * @param e Qt paint event
 */
void PlatformMScreen::viewportPaintEvent(QPaintEvent *e)
{
    TRACE_MSC(PlatformMScreen::viewportPaintEvent);
    const QRect& r = e->rect();
    refresh(r.left(), r.top(), r.right(), r.bottom());
}

/**
 * Create a Color from the packed pixel value.
 *
 * @param pixel the packed pixel integer
 */
QColor PlatformMScreen::getColor(int pixel) {
    int r = (pixel >> 16) & 0xff;
    int g = (pixel >> 8)  & 0xff;
    int b = (pixel >> 0)  & 0xff;
    return QColor(r, g, b);
}

/**
 * Returns 'true' if dst is the current painting device. Otherwise,
 * return 'false'.
 *
 * @param dst Pointer to QPaintDevice structure
 */
bool
PlatformMScreen::isCurrentPaintDevice(QPaintDevice *dst) {
    return (gc->device() == dst) ? true : false;
}

/**
 * Set the drawing parameters in the QPainter
 */
QPainter *
PlatformMScreen::setupGC(int pixel_pen, int pixel_brush, const jshort *clip,
                  QPaintDevice *dst, int dotted) {

  TRACE_MSC(PlatformMScreen::setupGC);

    QPaintDevice* dev = gc->device();

    // default destination is the back buffer
    if (dst == NULL) {
	  TRACE_MSC(PlatformMScreen::setupGC-dstIsNull);
      //dst = (QPaintDevice*)&qpixmap;
      dst = &qpixmap;
    }

    if(dst == NULL){
        TRACE_MSC(PlatformMScreen::setupGC-dstIsStillNull);
    }

    // finish operation on old device, if changed
    if ((dev != dst) || force_refresh) {
        if (gc->isActive()) {
            gc->end();
        }
    }

    // start operation on new device, if needed
    if (!gc->isActive()) {
        gc->begin(dst);
    }

    if(gc->isActive()){
       TRACE_MSC(PlatformMScreen::setupGC-gcActive);
	}else{
       TRACE_MSC(PlatformMScreen::setupGC-gcNoActive);
	}
    // check if pen parameters changed
    if (((dev != dst)            ||
         (last_pen != pixel_pen) ||
         (last_dotted != dotted) ||
         force_refresh)) {

        if (pixel_pen != -1) {
            QColor color = getColor(pixel_pen);
            QPen pen = QPen(color, 0,
                            (dotted ? Qt::DotLine : Qt::SolidLine));
            gc->setPen(pen);
        } else {
            gc->setPen(Qt::NoPen);
        }
        last_pen = pixel_pen;
        last_dotted = dotted;
    }

    // check if pen parameters changed
    if (((dev != dst)            ||
         (last_brush != pixel_brush)   ||
         force_refresh)) {
        if (pixel_brush != -1) {
            gc->setBrush(getColor(pixel_brush));
        } else {
            gc->setBrush(Qt::NoBrush);
        }
        last_brush = pixel_brush;

    }


    // check if clipping region changed
    if (clip != NULL &&
        ((dev != dst)            ||
         force_refresh           ||
         (clip[0] != last_clip.left())  ||
         (clip[1] != last_clip.top())   ||
         ((clip[2] - clip[0]) != last_clip.width()) ||
         ((clip[3] - clip[1]) != last_clip.height()))) {
        QRect uclip(clip[0], clip[1],
                    clip[2] - clip[0], clip[3] - clip[1]);
        last_clip = uclip;
        gc->setClipRect(uclip);
    }

    // drop force_refresh flag after all
    force_refresh = false;

    return gc;
}

void PlatformMScreen::setNextVMTimeSlice(int millis) {
    if (millis < 0) {
        // A negative time means we should stop VM from getting time slice
        if (vm_slicer.isActive()) {
            vm_slicer.stop();
        }
    } else {
        if (vm_slicer.isActive()) {
        	#ifdef FV_USE_QT4
        	vm_slicer.start(millis);
        	#else
            vm_slicer.changeInterval(millis);
        	#endif
        } else {
            vm_slicer.start(millis);
        }
    }
}

void PlatformMScreen::slotTimeout() {
    slotTimeoutImpl();
}

/**
 * Refresh certain part of the screen.
 *
 * @param x1 absolute X coordinate of top left conner
 * @param y1 absolute Y coordinate of top left conner
 * @param x2 absolute X coordinate of lower right conner
 * @param y2 absolute Y coordinate of lower right conner
 */
void PlatformMScreen::refresh(int x1, int y1, int x2, int y2) {
    TRACE_MSC(PlatformMScreen::refresh);

    /* Only draw for canvas */
    if ((MidpCurrentScreen != NULL)
        && (MidpCurrentScreen->component.type == MIDP_CANVAS_TYPE)) {

		TRACE_MSC(PlatformMScreen::refresh111);

      /* Finish last activity */
      //if (gc->isActive())
      //    gc->end();

#ifdef FV_USE_QT4
    //QPainter painter;
    //painter.drawPixmap(x1, y1,
    #if 0
      gc->drawPixmap(x1, y1,
             qpixmap,
             x1, y1,
             x2 - x1 + 1,
             y2 - y1 + 1);
	#else
	//(void)x1;
	//(void)y1;
	//(void)x2;
	//(void)y2;
	//this->viewport()->repaint(x1, y1,x2 - x1 + 1,y2 - y1 + 1);
	gc_x=x1;
	gc_y=y1;
	gc_width=x2 - x1 + 1;
	gc_height=y2 - y1 + 1 ;
	 repaint( x1, y1,x2 - x1 + 1,y2 - y1 + 1 );
	#endif
	
#else
      bitBlt((QPaintDevice*)viewport(), x1, y1,
             &qpixmap,
             x1, y1,
             x2 - x1 + 1,
             y2 - y1 + 1);
#endif
    }


    force_refresh = true;
    last_pen = last_brush = -1;
}

#ifdef FV_USE_QT4
int
PlatformMScreen::getScrollPosition()
{
  return 0;
}

void PlatformMScreen::setScrollPosition(int pos)
{
(void)pos;
}
#else
int
PlatformMScreen::getScrollPosition()
{
  return (contentsY());
}

void PlatformMScreen::setScrollPosition(int pos)
{
    setContentsPos(0, pos);
}
#endif
/**
 * Resets native resources of the device when foreground is gained
 * by a new Display.
 */
void PlatformMScreen::gainedForeground() {
  force_refresh  = KNI_TRUE;
  key_press_count = 0;
}

/**
 * Width of a normal screen.
 */
 int PlatformMScreen::getDisplayWidth() const { 
    if (r_orientation) {
        return DISPLAY_HEIGHT;
    } else {
        return DISPLAY_WIDTH; 
    }
}
/**
 * Height of a normal screen.
 */
int PlatformMScreen::getDisplayHeight() const {
    if (r_orientation) {
        return DISPLAY_WIDTH;
    } else {
        return DISPLAY_HEIGHT; 
    }
}

/**
 * Width of a full screen canvas.
 */
int PlatformMScreen::getDisplayFullWidth() const {
    if (r_orientation) {
        return DISPLAY_FULLHEIGHT;
    } else {
        return DISPLAY_FULLWIDTH; 
    }
}

/**
 * Height of a full screen canvas.
 */
int PlatformMScreen::getDisplayFullHeight() const {
    if (r_orientation) {
        return DISPLAY_FULLWIDTH;
    } else {
        return DISPLAY_FULLHEIGHT;
    }
}

/**
 * Width available for laying out items in a Form.
 */
int PlatformMScreen::getScreenWidth() const {
    if (r_orientation) {
        return SCREEN_HEIGHT;
    } else {
        return SCREEN_WIDTH; 
    }
}

/**
 * Height available for laying out items in a Form.
 */
int PlatformMScreen::getScreenHeight() const {
    if (r_orientation) {
        return SCREEN_WIDTH;
    } else {
        return SCREEN_HEIGHT; 
    }
}


/**
 * Width available for Alert.
 */
int PlatformMScreen::getAlertWidth() const {
    if (r_orientation) {
        return ALERT_HEIGHT;
    } else {
        return ALERT_WIDTH;
    }

}

/**
 * Height available for Alert.
 */
int PlatformMScreen::getAlertHeight() const {
    if (r_orientation) {
        return ALERT_WIDTH;
    } else {
        return ALERT_HEIGHT; 
    }
}

void PlatformMScreen::paintEvent(QPaintEvent *pEvent){
        TRACE_MSC(PlatformMScreen::paintEvent);
        #if 0
if(gc_flag == 1){ 
#if 0
          QPaintDevice* dev = gc->device();
        
          // default destination is the back buffer
          if (gc_dst == NULL) {
                TRACE_MSC(PlatformMScreen::setupGC-dstIsNull);
            //gc_dst = (QPaintDevice*)&qpixmap;
            gc_dst = mscreen_pixmap;
          }
        
          // finish operation on old device, if changed
          if (dev != gc_dst) {
              if (gc->isActive()) {
                  gc->end();
              }
          }
        
          // start operation on new device, if needed
          if (!gc->isActive()) {
              gc->begin(gc_dst);
              //gc->begin(this);
          }
        
          if(gc->isActive()){
             TRACE_MSC(PlatformMScreen::setupGC-gcActive);
              }else{
             TRACE_MSC(PlatformMScreen::setupGC-gcNoActive);
              }
          // check if pen parameters changed
          if (((dev != gc_dst)            ||
               (last_pen != gc_pixel_pen) ||
               (last_dotted != gc_dotted))) {
        
              if (gc_pixel_pen != -1) {
                  QColor color = getColor(gc_pixel_pen);
                  QPen pen = QPen(color, 0,
                                  (gc_dotted ? Qt::DotLine : Qt::SolidLine));
                  gc->setPen(pen);
              } else {
                  gc->setPen(Qt::NoPen);
              }
              last_pen = gc_pixel_pen;
              last_dotted = gc_dotted;
          }
        
          // check if pen parameters changed
          if (((dev != gc_dst)            ||
               (last_brush != gc_pixel_brush))) {
              if (gc_pixel_brush != -1) {
                  gc->setBrush(getColor(gc_pixel_brush));
              } else {
                  gc->setBrush(Qt::NoBrush);
              }
              last_brush = gc_pixel_brush;
        
          }
        
        
          // check if clipping region changed
          if (gc_clip != NULL &&
              ((dev != gc_dst)            ||
               (gc_clip[0] != last_clip.left())  ||
               (gc_clip[1] != last_clip.top())   ||
               ((gc_clip[2] - gc_clip[0]) != last_clip.width()) ||
               ((gc_clip[3] - gc_clip[1]) != last_clip.height()))) {
              QRect uclip(gc_clip[0], gc_clip[1],
                          gc_clip[2] - gc_clip[0], gc_clip[3] - gc_clip[1]);
              last_clip = uclip;
              gc->setClipRect(uclip);
          }
#endif
		(void)pEvent;
		//QFrame::paintEvent(pEvent);
    	//QPainter p(this->viewport());
    	QPainter p(this);
          switch(gc_type){
                case 0:
					    printf("=====================paintevent 0\n");
						//p.begin(this);
                        //p.fillRect(0, 0, 300, 300, QColor(0,255,0));
                        p.fillRect(gc_x,gc_y,gc_width,gc_height,getColor(gc_pixel_brush));
						//p.end();
                        break;

				case 1:
					    printf("=====================paintevent 1\n");
					    //p.begin(this);
						//mscreen_image.save("test222.png");
						//if(!old_mscreen_image.isNull()){
						//p.drawImage(old_gc_x,old_gc_y,old_mscreen_image);
						//}
						p.drawImage(gc_x,gc_y,mscreen_image);
						//p.end();
					    //gc->drawImage(gc_x, gc_y, *mscreen_image);
						break;
                default:
                        break;
          }

          gc_flag = 0;
}
 #endif  
   (void)pEvent;
   QPainter p(this);
   //p.drawPixmap(0,0,640,530,qpixmap);
   p.drawPixmap(gc_x,gc_y,qpixmap,gc_x,gc_y,gc_width,gc_height);
   
}

int PlatformMScreen::setupGCParas(int pixel_pen, int pixel_brush, const jshort *clip,
		      QPaintDevice *dst, int dotted,int x, int y, int width, int height){
		old_gc_x = gc_x;
		old_gc_y = gc_y;
		
        gc_pixel_pen = pixel_pen;
        gc_pixel_brush = pixel_brush;
        gc_clip = (short *)clip;
        gc_dst = dst;
        gc_dotted = dotted;

        gc_x = x;
        gc_y = y;
        gc_width = width;
        gc_height = height;

        gc_flag = 1;
        return 0;
}

int PlatformMScreen::setGCType(int gc_t){
        gc_type = gc_t;

        return 0;
}

void PlatformMScreen::gcUpdate() {
       switch(gc_type){
                case 0:
					    printf("=====================PlatformMScreen::gcUpdate 0\n");
                        //this->viewport()->repaint(0,0,640,530);
                        //repaint(gc_x,gc_y,gc_width,gc_height);
                        update(gc_x,gc_y,gc_width,gc_height);
                        break;
				case 1:
					    //update(gc_x,gc_y,640,530);
					    //repaint(gc_x,gc_y,640,530);
					    //printf("=====================gcUpdate\n");
					    //gc->begin(this);
						//gc->drawImage(QPointF(gc_x, gc_y), *mscreen_image,
						//		QRectF(gc_x, gc_y, 640, 530),
						//		Qt::AutoColor);
						//gc->end();
					    printf("=====================PlatformMScreen::gcUpdate 1\n");
                        //this->viewport()->repaint(0,0,640,530);
                        //repaint(gc_x,gc_y,mscreen_image.width(),mscreen_image.height());
                        update(gc_x,gc_y,mscreen_image.width(),mscreen_image.height());
                default:
                        break;
       }
}

void PlatformMScreen::setupImage(QImage imagePtr){
	//mscreen_image = imagePtr;
	//if(!mscreen_image.isNull()){
	//old_mscreen_image = QImage(mscreen_image);
	//}
	mscreen_image = QImage(imagePtr);
	//mscreen_image.save("test111.png");
}

