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

#include <lfjport_export.h>
#include <qteapp_export.h>
#include "lfjport_qte_mainwindow.h"
#include <midp_logging.h>
#include <midp_constants_data.h>

/**
 * @file
 * Additional porting API for Java Widgets based port of abstract
 * command manager.
 */

static jboolean inFullScreenMode;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the LCDUI native resources.
 *
 * @return <tt>0</tt> upon successful initialization, or
 *         <tt>other value</tt> otherwise
 */
int lfjport_ui_init() {
  inFullScreenMode = KNI_FALSE;
  qteapp_init(lfjport_create_main_window);
  return 0;
}

/**
 * Finalize the LCDUI native resources.
 */
void lfjport_ui_finalize() {
  qteapp_finalize();
}

/**
 * Bridge function to request a repaint 
 * of the area specified.
 *
 * @param x1 top-left x coordinate of the area to refresh
 * @param y1 top-left y coordinate of the area to refresh
 * @param x2 bottom-right x coordinate of the area to refresh
 * @param y2 bottom-right y coordinate of the area to refresh
 */
void lfjport_refresh(int x1, int y1, int x2, int y2)
{
  qteapp_get_mscreen()->refresh(x1, y1, x2, y2);
}

/**
 * Porting API function to update scroll bar.
 *
 * @param scrollPosition current scroll position
 * @param scrollProportion maximum scroll position
 * @return status of this call
 */
int lfjport_set_vertical_scroll(
  int scrollPosition, int scrollProportion) {
  REPORT_CALL_TRACE2(LC_HIGHUI, "LF:STUB:LCDUIsetVerticalScroll(%3d, %3d)\n",
		     scrollPosition, scrollProportion);

  /* Suppress unused parameter warnings */
  (void)scrollPosition;
  (void)scrollProportion;

  return 0;
}


/**
 * Turn on or off the full screen mode
 *
 * @param mode true for full screen mode
 *             false for normal
 */
void lfjport_set_fullscreen_mode(jboolean mode) {
  ChameleonMIDPMainWindow * mainWindow = 
    ChameleonMIDPMainWindow::getMainWindow();
  mainWindow->setFullScreen(mode);
  inFullScreenMode = mode;
}

jboolean lfjport_reverse_orientation()
{
   ChameleonMIDPMainWindow * mainWindow =
    ChameleonMIDPMainWindow::getMainWindow();
   jboolean r = qteapp_get_mscreen()->reverse_orientation();
   mainWindow->resizeScreen();
   return r;
}

jboolean lfjport_get_reverse_orientation()
{
   return qteapp_get_mscreen()->get_reverse_orientation();
}

int lfjport_get_screen_width() {
    if (inFullScreenMode) {
        return qteapp_get_mscreen()->getDisplayFullWidth();
    } else {
        return qteapp_get_mscreen()->getDisplayWidth();
    }
}

int lfjport_get_screen_height() {
    if (inFullScreenMode) {
        return qteapp_get_mscreen()->getDisplayFullHeight();
    } else {
        return qteapp_get_mscreen()->getDisplayHeight();
    }
}

/**
 * Bridge function to ask MainWindow object for the full screen mode
 * status.
 */
jboolean lfjport_is_fullscreen_mode() {
  return inFullScreenMode;
}

/**
 * Resets native resources when foreground is gained by a new display.
 */
void lfjport_gained_foreground() {
  qteapp_get_mscreen()->gainedForeground();
}

#ifdef __cplusplus
}
#endif
