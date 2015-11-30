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

#include <btformat.h>


/**
 * Converts hex value from character to int value.
 *
 * @param c input character contains ('0'-'9','A'-'F')
 * @return integer value of input character
 */
int hex2int(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return c - 'A' + 10;
}

/**
 * Converts int value (0-0xF) to character.
 *
 * @param i input integer value (0-0xF)
 * @return appropriate character ('0'-'9','A'-'F')
 */
char int2hex(int i)
{
    if (i >= 0 && i <= 9) {
        return '0' + i;
    }
    if (i >= 10 && i <= 15) {
        return 'A' + i - 10;;
    }
    return '\0';
}

/**
 * Extracts Bluetooth address from Java string.
 *
 * @param addressHandle handle to Java string containing Bluetooth address
 * @param addr receives Bluetooth address value
 */
void get_addr(jstring addressHandle, bt_bdaddr_t addr)
{
    int i;
    MidpString str = midpNewString(addressHandle);
    char *address = midpJcharsToChars(str);
    char *ptr = address + (BT_ADDRESS_SIZE * 2 - 1);
    for (i = 0; i < BT_ADDRESS_SIZE; i++) {
        *addr = hex2int(*ptr--);
        *addr++ |= hex2int(*ptr--) << 4;
    }
    midpFree(address);
    MIDP_FREE_STRING(str);
}
