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

#ifndef BT_KNI_H
#define BT_KNI_H

#include <kni.h>
#include <midpError.h>

/**
 * @def KNI_BOOL
 * Gets appropriate boolean KNI value.
 */
#define KNI_BOOL(b) ((b) ? KNI_TRUE : KNI_FALSE)


/**
 * @def KNI_TEST
 * Throws runtime exception when input expression is false.
 */
#define KNI_TEST(expr) \
        if (!(expr)) { \
            REPORT_WARN2(0, "Assertion failed: file %s line %d", \
                    __FILE__, __LINE__); \
            KNI_ThrowNew(midpRuntimeException, NULL); \
        }

#endif
