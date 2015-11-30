/*
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

#include <stdlib.h>
#include <string.h>
#include <btCommon.h>

#include "GenericBluetoothStack.h"

#define OGF_LINK_CONTROL    0x01

#define OCF_INQUIRY         0x0001
#define OCF_INQUIRY_CANCEL  0x0002
#define OCF_AUTH_REQUEST    0x0011
#define OCF_ENCRYPT_REQUEST 0x0013
#define OCF_NAME_REQUEST    0x0019

bool CGenericBluetoothStack::StartInquiry(int accessCode)
{
    struct {
        unsigned char lap[3];
        unsigned char length;
        unsigned char num;
    } __PPACKED__ request;
    request.lap[0] = accessCode;
    request.lap[1] = accessCode >> 8;
    request.lap[2] = accessCode >> 16;
    request.length = 0x10;
    request.num = 0;
    if (!SendHCICommand(OGF_LINK_CONTROL, OCF_INQUIRY, sizeof(request),
            &request)) {
        return false;
    }
    return WaitHCICommandStatus();
}

bool CGenericBluetoothStack::CancelInquiry()
{
    if (!SendHCICommand(OGF_LINK_CONTROL, OCF_INQUIRY_CANCEL, 0, NULL)) {
        return false;
    }
    return WaitHCICommandComplete();
}

bool CGenericBluetoothStack::AskFriendlyName(bt_bdaddr_t addr)
{
    struct {
        unsigned char addr[6];
        unsigned char scanRepMode;
        unsigned char reserved;
        unsigned short clock;
    } __PPACKED__ request;
    
    memcpy(request.addr, addr, BT_ADDRESS_SIZE);
    request.scanRepMode = 1;
    request.reserved = 0;
    request.clock = 0;
    if (!SendHCICommand(OGF_LINK_CONTROL, OCF_NAME_REQUEST, sizeof(request),
            &request)) {
        return false;
    }
    return WaitHCICommandStatus();
}

bool CGenericBluetoothStack::Authenticate(int handle)
{
    if (!SendHCICommand(OGF_LINK_CONTROL, OCF_AUTH_REQUEST, sizeof(handle),
            &handle)) {
        return false;
    }
    return WaitHCICommandStatus();
}

bool CGenericBluetoothStack::Encrypt(int handle, bool enable)
{
    struct {
        unsigned short handle;
        unsigned char enable;
    } __PPACKED__ request;
    request.handle = handle;
    request.enable = enable ? 1 : 0;
    if (!SendHCICommand(OGF_LINK_CONTROL, OCF_ENCRYPT_REQUEST, sizeof(request),
            &request)) {
        return false;
    }
    return WaitHCICommandStatus();
}
