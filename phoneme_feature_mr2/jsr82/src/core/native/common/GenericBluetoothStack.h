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

#ifndef GENERIC_BLUETOOTH_STACK_H
#define GENERIC_BLUETOOTH_STACK_H

/**
 * @file
 * @ingroup jsr82bluetooth
 * @brief #include <GenericBluetoothStack.h>
 *
 * The CGenericBluetoothStack class is useful when porting to the platform
 * which provides access to Bluetooth HCI interface.
 * @{
 */

#include "BluetoothStack.h"

/**
 * @brief Class partly implementing porting interface via HCI.
 *
 * This class provides implementation of StartInquiry(), CancelInquiry() and
 * AskFriendlyName() methods by means of Bluetooth HCI interface.
 */
class CGenericBluetoothStack: public CBluetoothStack {
public:
    bool StartInquiry(int accessCode);
    bool CancelInquiry();
    bool AskFriendlyName(bt_bdaddr_t addr);
    bool Authenticate(int handle);
    bool Encrypt(int handle, bool enable);
protected:
    /**
     * This method is used to send an HCI command to the underlying native stack
     * implementation.
     *
     * @param ogf OpCode Group Field as described in Bluetooth specification
     * @param ocf OpCode Command Field as described in Bluetooth specification
     * @param len length of the parameters in bytes
     * @param param command parameters
     */
    virtual bool SendHCICommand(unsigned short ogf, unsigned short ocf,
        unsigned char len, const void *param) = 0;

    /**
     * This method awaits until the Command Complete Event is received, or
     * a predefined timeout has elapsed.
     *
     * @return <code>true</code> if the event was received,
     *         <code>false</code> otherwise
     */
    virtual bool WaitHCICommandComplete() = 0;

    /**
     * This method awaits until the Command Status Event is received, or
     * a predefined timeout has elapsed.
     *
     * @return <code>true</code> if the event was received with a non-failure
     *         status, <code>false</code> otherwise
     */
    virtual bool WaitHCICommandStatus() = 0;
};

/** @} */

#endif
