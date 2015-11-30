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

#ifndef BLUEZ_STACK_H
#define BLUEZ_STACK_H

#include "GenericBluetoothStack.h"

#include <sys/poll.h>
#include <bluetooth/sdp.h>

/**
 * @brief Implementation based on BlueZ stack.
 *
 * This class inherits CGenericBluetoothStack class which uses HCI interface.
 * It implements sending and retrieving HCI commands and events by means of
 * bluetooth-libs-2.21 library. It also implements access to native SDDB.
 */
class CBlueZStack: public CGenericBluetoothStack {
public:
    CBlueZStack();
    ~CBlueZStack();
    bool IsEnabled();
    bool Enable();
    const char *GetLocalAddress();
    const char *GetLocalName(char *name, int size);
    int GetDeviceClass();
    bool SetServiceClasses(int classes);
    int GetAccessCode();
    bool SetAccessCode(int accessCode);
    bool CheckEvents();
    int ReadData(void *data, unsigned short len);
protected:
    bool SendHCICommand(unsigned short ogf, unsigned short ocf,
        unsigned char len, const void *param);
    bool WaitHCICommandComplete();
    bool WaitHCICommandStatus();
private:
    static int g_devId;
    int m_fd;
    struct pollfd m_poll;
};

#endif
