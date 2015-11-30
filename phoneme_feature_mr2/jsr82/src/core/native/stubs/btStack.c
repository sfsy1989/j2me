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

bt_result_t bt_stack_initialize()
{
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_finalize()
{
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_is_initialized(bt_bool_t *retval)
{
    (void)retval;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_is_enabled(bt_bool_t *retval)
{
    (void)retval;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_enable()
{
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_get_local_address(bt_bdaddr_t *retval)
{
    (void)retval;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_get_local_name(bt_string_t name, int size)
{
    (void)name;
    (void)size;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_get_device_class(int *retval)
{
    (void)retval;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_set_service_classes(int classes)
{
    (void)classes;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_get_access_code(int *retval)
{
    (void)retval;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_set_access_code(int accessCode)
{
    (void)accessCode;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_start_inquiry(int accessCode)
{
    (void)accessCode;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_cancel_inquiry()
{
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_ask_friendly_name(const bt_bdaddr_t addr)
{
    (void)addr;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_authenticate(int handle)
{
    (void)handle;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_stack_encrypt(int handle, bt_bool_t enable)
{
    (void)handle;
    (void)enable;
    return BT_RESULT_FAILURE;
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
