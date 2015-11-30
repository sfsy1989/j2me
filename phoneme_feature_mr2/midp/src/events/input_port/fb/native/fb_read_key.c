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
 * Functions to read key events from platform devices
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#ifdef ANDROID
#include <linux/msg.h>
#else
#include <sys/msg.h>
#endif

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifdef DIRECTFB
#include <directfb.h>
#include <directfbapp_export.h>
#endif

#include <kni.h>
#include <midp_logging.h>
#include <fbapp_export.h>

#include "fb_handle_input.h"
#include "fb_read_key.h"

jboolean read_fh2_key_event() {
#if 0
        // Platform structure to read key event to
        struct FbKeyEvent {
	    unsigned long irkey_datah;
	    unsigned long irkey_datal;
	    unsigned long irkey_state_code;
        } fbKeyEvent;

	unsigned long long key_value;
	int key_state;

	ioctl(fbapp_get_keyboard_fd(), 0x07, 200);


        int res = read(fbapp_get_keyboard_fd(), &fbKeyEvent, sizeof(fbKeyEvent));

        if ((res<0) || (res>sizeof(fbKeyEvent))) {
            REPORT_ERROR2(LC_CORE,
                "Invalid key input event, received %d bytes instead of %d",
                res, sizeof(fbKeyEvent));
            printf("=====read_fh2_key_event Invalid key input\n");
                    return KNI_FALSE;
        }
        
	 if((res>0)&&(res<=sizeof(fbKeyEvent)))
	    {
	                key_state = fbKeyEvent.irkey_state_code;
	                key_value = fbKeyEvent.irkey_datah & 0xFFFF;
	                key_value = key_value << 32;
	                key_value = key_value | fbKeyEvent.irkey_datal;
	                printf("=====key value is 0x%llx     key state is %d \n",key_value,key_state);
	    }


        if((key_state == 0) || (key_state == 1)){
        	printf("=====read_fh2_key_event set struct keyState\n");
	        keyState.km = mapping;
	        //keyState.changedBits = fbKeyEvent.value ^ keyState.key;
	        keyState.key = (unsigned int)key_value;
	        keyState.down = 0;
	         return KNI_TRUE;
        }
        
    keyState.down = -1;
    return KNI_FALSE;
#endif
        char keyBuffer[8];
        int    readBytes;
        bzero(keyBuffer,sizeof(keyBuffer));

        if((readBytes=read(fbapp_get_keyboard_fd(),keyBuffer,sizeof(keyBuffer)))<0)
        {
          printf("========  read_fh2_key_event() pipe error!\n");
          keyState.down = -1;
          return KNI_FALSE;
        }

        if(readBytes!=0){
                if(readBytes>(int)sizeof(keyBuffer)){
                        printf("========  read_fh2_key_event() pipe error!\n");
                        keyState.down = -1;
                        return KNI_FALSE;
                }
                printf("========  read_fh2_key_event()  Get  key %d\n",keyBuffer[0]);
                keyState.km = mapping;
                keyState.key = (unsigned int)keyBuffer[0];
                keyState.down = (unsigned int)keyBuffer[1];
        }
 
        //keyState.km = mapping;
        //keyState.key = (unsigned int)keyBuffer[0];
        //keyState.down = 0;
        return KNI_TRUE;
        
}



/** Update input keys state from OMAP730 keypad device */
jboolean read_omap730_key_event() {
    if (!keyState.hasPendingKeySignal) {
        // Platform structure to read key event to
        struct FbKeyEvent {
            struct timeval time;
            unsigned short type;
            unsigned short code;
            unsigned value;
        } fbKeyEvent;
        size_t fbKeyEventSize =
            sizeof(fbKeyEvent);

        int readBytes = read(
            fbapp_get_keyboard_fd(), &fbKeyEvent, fbKeyEventSize);

        if (readBytes < (int)fbKeyEventSize) {
            REPORT_ERROR2(LC_CORE,
                "Invalid key input event, received %d bytes instead of %d",
                readBytes, (int)fbKeyEventSize);
                    return KNI_FALSE;
        }
        keyState.km = mapping;
        keyState.changedBits = fbKeyEvent.value ^ keyState.key;
        keyState.key = fbKeyEvent.value;
    }
    keyState.down = -1;
    return KNI_TRUE;
}

/** Update input keys state reading single char from keyboard device */
jboolean read_char_key_event() {
    unsigned char c;
    int readBytes = read(fbapp_get_keyboard_fd(), &c, sizeof(c));
    keyState.key = c;
    keyState.down = -1;
    return (readBytes > 0) ?
        KNI_TRUE : KNI_FALSE;
}

#ifdef DIRECTFB
/** Update input keys state from DirectFB keyboard device */
jboolean read_directfb_key_event() {
    DFBWindowEvent dfbEvent;
    directfbapp_get_event(&dfbEvent);

    // IMPL_NOTE: DirectFB sends key-up codes via DWET_KEYUP
    //   event with ordinary (like key-down) key code
    keyState.down = (dfbEvent.type != DWET_KEYUP);

    keyState.changedBits = dfbEvent.key_code ^ keyState.key;
    keyState.key = dfbEvent.key_code;
    return KNI_TRUE;
}
#endif
