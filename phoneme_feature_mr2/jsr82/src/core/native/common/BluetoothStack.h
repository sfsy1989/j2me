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

#ifndef BLUETOOTH_STACK_H
#define BLUETOOTH_STACK_H

/**
 * @defgroup jsr82 JSR82 Optional Package
 * @ingroup stack
 */

/**
 * @defgroup jsr82bluetooth JSR82 BluetoothStack Porting Interface
 * @ingroup jsr82
 * @brief JSR82 BluetoothStack Porting Interface.
 *
 * Central to the porting interface is a single C++ abstract class,
 * CBluetoothStack, which must be subclassed and have its abstract methods
 * implemented for use with a specific native Bluetooth stack available
 * on the target platform.
 *
 * CGenericBluetoothStack, which is a subclass of
 * CBluetoothStack, can be used when the native Bluetooth stack provides
 * access to HCI interface.
 */

/**
 * @file
 * @ingroup jsr82bluetooth
 * @brief #include <BluetoothStack.h>
 * @{
 */

#include <stdlib.h>
#include <btCommon.h>

/**
 * @brief Abstract class defining core porting interface.
 *
 * This abstract class provides porting interface for all Bluetooth stack
 * implementations. For each isolate, a separate instance of this class
 * (or rather of a subclass of this class) is created, serving as a proxy
 * between the actual stack implementation and the upper-level code.
 */
class CBluetoothStack {
public:
    /** Default constructor counts instances of this interface. */
    CBluetoothStack();

    /** Virtual destructor counts instances of this interface. */
    virtual ~CBluetoothStack();

    /**
     * Indicates whether this instance was successfully initialized.
     *
     * @return true if the initialization was successful, false otherwise
     */
    virtual bool IsInitialized() = 0;

    /**
     * Checks if the Bluetooth radio is enabled.
     *
     * @return true if Bluetooth is enabled, false otherwise
     */
    virtual bool IsEnabled() = 0;

    /**
     * Enables Bluetooth radio.
     *
     * @return true if Bluetooth is enabled, false otherwise
     */
    virtual bool Enable() = 0;

    /**
     * Returns Bluetooth address of the local device.
     *
     * @return Bluetooth address of the local device in ASCII format,
     *         or NULL if the address could not be retrieved
     */
    virtual const char *GetLocalAddress() = 0;

    /**
     * Returns user-friendly name for the local device.
     *
     * @param name pointer to a buffer which will receive user-friendly
     *        name for the local device
     * @param size size of the buffer
     * @return User-friendly name for the local device, or NULL if
     *         the name could not be retrieved
     */
    virtual const char *GetLocalName(char *name, int size) = 0;

    /**
     * Retrieves the class of device value that represents the service classes,
     * major device class, and minor device class of the local device.
     *
     * @return class of device value, or -1 if the information could not
     *         be retrieved
     */
    virtual int GetDeviceClass() = 0;

    /**
     * Sets major service class bits of the device.
     *
     * @param classes an integer whose binary representation indicates the major
     *        service class bits that should be set
     * @return true if the operation succeeded, false otherwise
     */
    virtual bool SetServiceClasses(int classes) = 0;

    /**
     * Retrieves the inquiry access code that the local Bluetooth device is
     * scanning for during inquiry scans.
     *
     * @return inquiry access code, or -1 if the information could not
     *         be retrieved
     */
    virtual int GetAccessCode() = 0;

    /**
     * Sets the inquiry access code that the local Bluetooth device is
     * scanning for during inquiry scans.
     *
     * @param accessCode inquiry access code to be set (valid values are in the
     *        range 0x9e8b00 to 0x9e8b3f), or 0 to take the device out of
     *        discoverable mode
     * @return true if the operation succeeded, false otherwise
     */
    virtual bool SetAccessCode(int accessCode) = 0;

    /**
     * Starts asynchronous device discovery.
     *
     * @param accessCode desired access code
     * @return true if the operation has started, false otherwise
     */
    virtual bool StartInquiry(int accessCode) = 0;

    /**
     * Cancels asynchronous device discovery.
     *
     * @return true if the operation was canceled, false otherwise
     */
    virtual bool CancelInquiry() = 0;

    /**
     * Starts asynchronous name retrieval.
     *
     * @param addr Bluetooth address of the device which name is to be retrieved
     * @return true if the operation has started, false otherwise
     */
    virtual bool AskFriendlyName(bt_bdaddr_t addr) = 0;

    /**
     * Starts asynchronous authentication.
     *
     * @param handle connection handle for an ACL connection
     * @return true if the operation has started, false otherwise
     */
    virtual bool Authenticate(int handle) = 0;

    /**
     * Starts asynchronous encryption mode change.
     *
     * @param handle connection handle for an ACL connection
     * @param enable flag indicating whether the encryption needs to be enabled
     * @return true if the operation has started, false otherwise
     */
    virtual bool Encrypt(int handle, bool enable) = 0;

    /**
     * Checks if Bluetooth events are available.
     *
     * @return true if there are pending Bluetooth events, false otherwise
     */
    virtual bool CheckEvents() = 0;

    /**
     * Reads stack implementation-specific event data.
     *
     * @param data buffer where the data will be written to
     * @param len length of the buffer in bytes
     * @return actual number of bytes read
     */
    virtual int ReadData(void *data, unsigned short len) = 0;

    /** Overloaded memory allocation operator. */
    static void *operator new(size_t size);

    /** Overloaded memory deallocation operator. */
    static void operator delete(void *addr);

protected:
    /**
     * Indicates whether this instance was successfully initialized.
     * Set this variable to true in constructor, otherwise the object
     * will be automatically deleted by the calling code.
     */
    bool m_bInitialized;

    /**
     * Keeps count of subclasses of this class, can be used for global
     * initialization/teardown in constructor/destructor.
     */
    static unsigned int g_nInstanceCount;
};

/** @} */

#endif
