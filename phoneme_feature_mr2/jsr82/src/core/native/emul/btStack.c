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

#include <btStack.h>
#include <btStackEvent.h>
#include <emul.h>
#include <midp_thread.h>
#include <sni.h>

#define APPEND_DEV_REQ(req) \
    APPEND_BYTE(DEVICE); \
    APPEND_BYTE(req);

static char *deviceName = "emulName";

bt_result_t bt_stack_initialize()
{
    LOG("bt_stack_initialize()");
    ensureInitialized();
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_finalize()
{
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_is_initialized(bt_bool_t *retval)
{
    ensureInitialized();
    *retval = BT_BOOL_TRUE;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_is_enabled(bt_bool_t *retval)
{
    ensureInitialized();
    *retval = state & BLUETOOTH_ON ? BT_BOOL_TRUE : BT_BOOL_FALSE;
    return BT_RESULT_SUCCESS;
}

void initDevice() {
    if (!(state & DEVICE_INITED)) {
        START_REQUEST
            APPEND_DEV_REQ(INIT_DEVICE);
        END_REQUEST
        if (NULL == SNI_GetReentryData(NULL)) {
            // this condition preserves from calling midp_thread_wait
            // twice without unblocking current thread.
            midp_thread_wait(JSR82_SIGNAL, BTE_SIGNAL_HANDLE, NULL);
        }
    }
}

bt_result_t bt_stack_enable()
{
    ensureInitialized();
    initDevice();
    state |= BLUETOOTH_ON;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_get_local_address(bt_bdaddr_t *retval)
{
    initDevice();
    memcpy(retval, emul_data.local_addr, BT_ADDRESS_SIZE);
    return BT_RESULT_SUCCESS;
}
    
bt_result_t bt_stack_get_local_name(bt_string_t name, int size)
{
    int len = strlen(deviceName) + 1;
    (void)size;
    
    memcpy(name, deviceName, len);
    
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_get_device_class(int *retval)
{
    initDevice();
    *retval = emul_data.device_class;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_set_service_classes(int classes)
{
    initDevice();

    emul_data.device_class = emul_data.device_class_base | classes;
    START_REQUEST
        APPEND_DEV_REQ(UPDATE_CLASS);
        APPEND_INT(classes);
    END_REQUEST
    
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_get_access_code(int *retval)
{
    *retval = emul_data.access_code;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_set_access_code(int accessCode)
{
    emul_data.access_code = accessCode;
    START_REQUEST
        APPEND_DEV_REQ(UPDATE_ACCESS);
        APPEND_INT(emul_data.access_code);
    END_REQUEST
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_start_inquiry(int accessCode)
{
    LOG("bt_stack_start_inquiry()");
    START_REQUEST
        APPEND_DEV_REQ(START_INQUIRY);
        APPEND_INT(accessCode);
    END_REQUEST
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_cancel_inquiry()
{
    LOG("bt_stack_cancel_inquiry()");
    START_REQUEST
        APPEND_DEV_REQ(CANCEL_INQUIRY);
    END_REQUEST
    return BT_RESULT_SUCCESS;
}
    
bt_result_t bt_stack_ask_friendly_name(const bt_bdaddr_t addr)
{
    LOG("bt_stack_ask_friendly_name()");
    bt_stack_on_remote_name_complete(addr, deviceName);
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_authenticate(int handle)
{
    LOG("bt_stack_authenticate()");
    bt_stack_on_authentication_complete(handle, BT_BOOL_TRUE);
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_stack_encrypt(int handle, bt_bool_t enable)
{
    LOG("bt_stack_encrypt()");
    if (enable) {
        bt_stack_on_encryption_change(handle, BT_BOOL_TRUE, BT_BOOL_TRUE);
    } else {
        bt_stack_on_encryption_change(handle, BT_BOOL_FALSE, BT_BOOL_TRUE);
    }
    return BT_RESULT_SUCCESS;
}

#ifndef BT_USE_EVENT_API

bt_result_t bt_stack_check_events(bt_bool_t *retval)
{
    (void)retval;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_read_data(void *data, int len, int *retval)
{
    (void)data;
    (void)len;
    (void)retval;
    return BT_RESULT_FAILURE;
}

#endif

bt_result_t bt_stack_update_record(unsigned long handle, const void *data,
        int len, unsigned long *retval)
{
    (void)handle;
    (void)data;
    (void)len;
    (void)retval;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_remove_record(unsigned long handle)
{
    (void)handle;
    return BT_RESULT_FAILURE;
}
