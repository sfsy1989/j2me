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

package com.sun.midp.lcdui;

/**
 * Event type IDs, this should match midpEvents.h.
 */
public final class EventConstants {
    /**
     * MINOR EVENT TYPES
     */
    
    /**
     * Minor ID indicating a press, either on a key or a pointer.
     */
    public static final int PRESSED  = 1;  // key, pen

    /**
     * Minor ID indicating a release, either of a key or a pointer.
     */
    public static final int RELEASED = 2;  // key, pen

    /**
     * Minor ID indicating a key repeat.
     */
    public static final int REPEATED = 3;  // key

    /**
     * Minor ID indicating a IME string typed (internal).
     */
    public static final int IME      = 4;

    /**
     * Minor ID indicating a pointer drag.
     */
    public static final int DRAGGED  = 3;  // pen

    /**
     * Minor ID indicating that command event requires posting a menu.
     */
    public static final int MENU_REQUESTED   = -1;

    /**
     * Minor ID indicating that command event is dismissing a menu.
     */
    public static final int MENU_DISMISSED   = -2;

    /**
     * Minor ID indicating that command event is requesting about box.
     * This value should be a positive id since some platforms (like Qt)
     * do not accept negative menu element id.
     */
    public static final int ABOUT_REQUESTED  = 0xFFFF;

    /**
     * The value returned from getSystemKey if the keyCode is the POWER key.
     */
    public static final int SYSTEM_KEY_POWER = 1;

    /**
     * The value returned from getSystemKey if the keyCode is SEND.
     */
    public static final int SYSTEM_KEY_SEND  = 2;

    /**
     * The value returned from getSystemKey if the keyCode is END.
     */
    public static final int SYSTEM_KEY_END   = 3;

    /**
     * The value returned from getSystemKey if the keyCode is either 
     * CLEAR or BACKSPACE. 
     */
    public static final int SYSTEM_KEY_CLEAR = 4;

    /**
     * An identifier for the first soft button, typically the left one.
     * NOTE: This value must be kept in sync with the value defined in
     * the "midpInput.h" file
     */
    public static final int SOFT_BUTTON1     = -6;

    /**
     * An identifier for the second soft button, typically the right one.
     * NOTE: This value must be kept in sync with the value defined in
     * the "midpInput.h" file
     */
    public static final int SOFT_BUTTON2     = -7;

    /**
     * An identifier for the special debug tracing key. This key is
     * generated by native code only when (Logging.REPORT_LEVEL &lt;
     * Logging.DISABLED).
     *
     * The exact key mapping is platform dependent.
     *
     * NOTE: This value must be kept in sync with the value KEY_DEBUGTRACE1
     * defined in the "midpInput.h" file
     */
    public static final int DEBUG_TRACE1     = -22;
};
