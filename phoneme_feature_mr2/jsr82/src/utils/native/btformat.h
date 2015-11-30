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

#ifndef __BT_FORMAT_H__
#define __BT_FORMAT_H__

#include <midpUtilKni.h>
#include <midpMalloc.h>
#include <btCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Converts hex value from character to int value.
 *
 * @param c input character contains ('0'-'9','A'-'F')
 * @return integer value of input character
 */
int hex2int(char c);

/**
 * Converts int value (0-0xF) to character.
 *
 * @param i input integer value (0-0xF)
 * @return appropriate character ('0'-'9','A'-'F')
 */
char int2hex(int i);

/**
 * Extracts Bluetooth address from Java string.
 *
 * @param addressHandle handle to Java string containing Bluetooth address
 * @param addr receives Bluetooth address value
 */
void get_addr(jstring addressHandle, bt_bdaddr_t addr);

#ifdef __cplusplus
}
#endif

#endif /* __BT_COMMON_H__ */
