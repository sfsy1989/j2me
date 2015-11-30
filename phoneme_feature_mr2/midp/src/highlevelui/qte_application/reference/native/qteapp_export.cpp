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

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include <jvmconfig.h>
#include <kni.h>
#include <jvm.h>
#include <jvmspi.h>

#include <qapplication.h>

#include <midpEvents.h>
#include <pcsl_memory.h>

#include <qteapp_export.h>

static QApplication*   qapp = NULL;
static MIDPMainWindow* MainWindow = NULL;
static MScreen*        mscreen = NULL;
jboolean               shuttingDown = KNI_FALSE;

// see Need revisit at the end of initialize
// the following extern should be moved once initMenus call is moved
extern "C" void initMenus();
extern "C" void qteapp_init(
  MIDPMainWindowCreateProc mainWindowCreateProc) {
  //edit by sookie
    //char display_buf[64];
    int qtArgc = 1;
#ifdef FV_USE_QT4
	char* argv[7];
#else
	char* argv[5];
#endif

    // The argv[0] which will go into Ui::initialize
    // *MUST* match the one in Qtopia env. (i.e. the name
    // inside .desktop file), otherwise the icon will
    // NOT show up in task bar when run. [QPE just
    // match the name of this and append .png. that's why]
    argv[0] = "usertest";

//#if ENABLE_QWS
    argv[1] = "-qws";
    qtArgc++;
//#endif /* ENABLE_QWS */
        argv[2] = "-nomouse";
    qtArgc++;
	
#ifdef FV_USE_QT4
	argv[3] = "-geometry";
#else
	argv[3] = "-bg";
#endif

    qtArgc++;

#ifdef FV_USE_QT4
	argv[4] = "640x530";
#else
	argv[4] = "white";
#endif

	
    qtArgc++;

#ifdef FV_USE_QT4
	argv[5] = "-display";
    qtArgc++;

	if((argv[6]=getenv("jvm_display"))==NULL){
		argv[6] = "linuxfb:/dev/fb0";
	}
	printf("======jvm_display is %s\n",argv[6]);
	//argv[6] = "linuxfb:/dev/fb1";
    qtArgc++;
#else
#endif

	qapp = new QApplication(qtArgc, argv);

#ifdef FV_USE_QT4
	QWSServer::setBackground(QBrush(Qt::transparent));
#else
#endif
    // The constructor of MIDPMainWindow sets mscreen.
    MainWindow = mainWindowCreateProc(NULL, NULL);

    // IMPL_NOTE:this should move to MainWindow or MScreen
    initMenus();
#ifdef FV_USE_QT4
#else
    qapp->setMainWidget(MainWindow);
#endif
    MainWindow->showMaximized();
    mscreen = MainWindow->getMScreen();
    mscreen->init();

//edit by sookie
    //MainWindow->setGeometry(MainWindow->x(), MainWindow->y(),MainWindow->width(),MainWindow->height());
    MainWindow->setGeometry(40,23,640,530);
   //MainWindow->setBackgroundColor( Qt::white );
    //MainWindow->setWFlags(MainWindow->getWFlags()|Qt::WStyle_Customize|Qt::WStyle_NoBorder);
}

extern "C" void qteapp_finalize() {
    shuttingDown = KNI_TRUE;
#ifdef FV_USE_QT4
    MainWindow->close();
#else
    MainWindow->close(true);
#endif
    // IMPL_NOTE: check return value from close()

    qapp->exit(0);

    /**
     * \Need revisit mscreen should be deleted by
     * MainWindow.
     */
    if (mscreen != NULL) {
      // deleted by MainWindow->close(true)
      // delete mscreen;
      mscreen = NULL;
    }

    if (MainWindow != NULL) {
      // deleted by MainWindow->close(true)
      // delete MainWindow;
      MainWindow = NULL;
    }

    delete qapp;
    qapp = NULL;

    shuttingDown = KNI_FALSE;
}

/**
 * Returns a pointer to the MScreen instance of MIDP application.
 */
extern "C" QApplication * qteapp_get_application() {
  return qapp;
}

/**
 * Returns a pointer to the MScreen instance of MIDP application.
 */
extern "C" QWSServer * qteapp_get_server() {
  return qwsServer;
}

/**
 * Returns a pointer to the main window of MIDP application.
 */
extern "C" MIDPMainWindow * qteapp_get_main_window() {
  return MainWindow;
}

/**
 * Returns a pointer to the MScreen instance of MIDP application.
 */
extern "C" MScreen * qteapp_get_mscreen() {
  return mscreen;
}

/**
 * Requests that the slave mode VM control code schedule a time slice
 * as soon as possible, since Java threads are waiting to be run.
 */
extern "C" void qteapp_schedule_time_slice() {
  mscreen->setNextVMTimeSlice(0);
}

/**
 * Runs the Qte event loop.
 */
extern "C" void qteapp_event_loop() {
  mscreen->startVM();
  #ifdef FV_USE_QT4
  qapp->exec();
  #else
  qapp->enter_loop();
  #endif
  mscreen->stopVM();
}

#if ENABLE_MIDP_MALLOC_FOR_NEW

/**
 * The libraries shipped with some C++ compilers on some slow platforms
 * have poor performance in the built-in new/delete operators. The
 * performance can be improved by re-implementing of the operators using
 * simple malloc/free functions. Please define ENABLE_MIDP_MALLOC_FOR_NEW
 * for the purpose.
 */

void* operator new[](size_t size) {
    return pcsl_mem_malloc(size);
}

void* operator new(size_t size) {
    return pcsl_mem_malloc(size);
}

void operator delete[](void* p) {
    pcsl_mem_free(p);
}

void operator delete[](void* p, size_t /*size*/) {
    pcsl_mem_free(p);
}

void operator delete(void* p) {
    pcsl_mem_free(p);
}

#endif
