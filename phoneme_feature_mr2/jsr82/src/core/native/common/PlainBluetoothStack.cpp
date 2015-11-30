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

#include <PlainBluetoothStack.h>
#include <btStack.h>
#include <btSDDB.h>

#include <stdio.h>
#include <string.h>


// Helper functions

const char *GetAddressAsString(bt_bdaddr_t bdaddr)
{
    static char address[13];
    sprintf(address, "%02X%02X%02X%02X%02X%02X", bdaddr[5], bdaddr[4],
            bdaddr[3], bdaddr[2], bdaddr[1], bdaddr[0]);
    return address;
}

// CPlainBluetoothStack class member functions

CPlainBluetoothStack::CPlainBluetoothStack()
{
    m_bInitialized = bt_stack_initialize() == BT_RESULT_SUCCESS;
}

CPlainBluetoothStack::~CPlainBluetoothStack()
{
    bt_stack_finalize();
}

bool CPlainBluetoothStack::IsInitialized()
{
    return m_bInitialized;
}

bool CPlainBluetoothStack::IsEnabled()
{
    bt_bool_t retval;
    return bt_stack_is_enabled(&retval) == BT_RESULT_SUCCESS &&
            retval == BT_BOOL_TRUE;
}

bool CPlainBluetoothStack::Enable()
{
    return bt_stack_enable() == BT_RESULT_SUCCESS;
}

const char *CPlainBluetoothStack::GetLocalAddress()
{
    bt_bdaddr_t retval;
    if (bt_stack_get_local_address(&retval) != BT_RESULT_SUCCESS) {
        return NULL;
    }
    return GetAddressAsString(retval);
}

const char *CPlainBluetoothStack::GetLocalName(char *name, int size)
{
    if (bt_stack_get_local_name(name, size) != BT_RESULT_SUCCESS) {
        return NULL;
    }
    return name;
}

int CPlainBluetoothStack::GetDeviceClass()
{
    int cls;
    if (bt_stack_get_device_class(&cls) != BT_RESULT_SUCCESS) {
        return -1;
    }
    return cls;
}

bool CPlainBluetoothStack::SetServiceClasses(int classes)
{
    return bt_stack_set_service_classes(classes) == BT_RESULT_SUCCESS;
}

int CPlainBluetoothStack::GetAccessCode()
{
    int iac;
    if (bt_stack_get_access_code(&iac) != BT_RESULT_SUCCESS) {
        return -1;
    }
    return iac;
}

bool CPlainBluetoothStack::SetAccessCode(int accessCode)
{
    return bt_set_access_code(accessCode) == BT_RESULT_SUCCESS;
}

bool CPlainBluetoothStack::StartInquiry(int accessCode)
{
    return bt_stack_start_inquiry(accessCode) == BT_RESULT_SUCCESS;
}

bool CPlainBluetoothStack::CancelInquiry()
{
    return bt_stack_cancel_inquiry() == BT_RESULT_SUCCESS;
}

bool CPlainBluetoothStack::AskFriendlyName(bt_bdaddr_t addr)
{
    return bt_stack_ask_friendly_name(addr) == BT_RESULT_SUCCESS;
}

bool CPlainBluetoothStack::Authenticate(int handle)
{
    return bt_stack_authenticate(handle) == BT_RESULT_SUCCESS;
}

bool CPlainBluetoothStack::Encrypt(int handle, bool enable)
{
    return bt_stack_encrypt(handle, enable ? BT_BOOL_TRUE : BT_BOOL_FALSE) ==
            BT_RESULT_SUCCESS;
}

bool CPlainBluetoothStack::CheckEvents()
{
    bt_bool_t retval;
    return bt_stack_check_events(&retval) == BT_RESULT_SUCCESS &&
            retval == BT_BOOL_TRUE;
}

int CPlainBluetoothStack::ReadData(void *data, unsigned short len)
{
    int retval;
    if (bt_stack_read_data(data, len, &retval) != BT_RESULT_SUCCESS) {
        return -1;
    }
    return retval;
}
