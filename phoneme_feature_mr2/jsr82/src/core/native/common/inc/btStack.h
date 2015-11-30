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

#ifndef STACK_H
#define STACK_H

#include <btCommon.h>

/** @def BT_USE_EVENT_API define this symbol for high level calback-based API */
#define BT_USE_EVENT_API

/**
 * @defgroup jsr82stack JSR82 BluetoothStack C Language Porting Interface
 * @ingroup jsr82
 * @brief JSR82 BluetoothStack C Language Porting Interface.
 *
 * Group description pending.
 */

/**
 * @file
 * @ingroup jsr82stack
 * @brief #include <btStack.h>
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocates native resources.
 *
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_initialize();

/**
 * Releases native resources.
 *
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_finalize();

/**
 * Checks if the Bluetooth radio is enabled.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the Bluetooth radio is enabled,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_is_enabled(bt_bool_t *retval);

/**
 * Enables Bluetooth radio.
 *
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_enable();

/**
 * Returns Bluetooth address of the local device.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         Bluetooth address of the local device
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_get_local_address(bt_bdaddr_t *retval);

/**
 * Returns user-friendly name for the local device.
 *
 * @param addr Bluetooth address of a remote device
 * @param name buffer to receive the user-friendly name for the local device
 *         (null-terminated in UTF-8 encoding)
 * @param size size of the buffer in bytes
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_get_local_name(bt_string_t name, int size);

/**
 * Retrieves the class of device value that represents the service classes,
 * major device class, and minor device class of the local device.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         class of device value, or -1 if the information could not
 *         be retrieved
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_get_device_class(int *retval);

/**
 * Sets major service class bits of the device.
 *
 * @param classes an integer whose binary representation indicates the major
 *        service class bits that should be set
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_set_service_classes(int classes);

/**
 * Retrieves the inquiry access code that the local Bluetooth device is
 * scanning for during inquiry scans.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         inquiry access code, or -1 if the information could not
 *         be retrieved
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_get_access_code(int *retval);

/**
 * Sets the inquiry access code that the local Bluetooth device is
 * scanning for during inquiry scans.
 *
 * @param accessCode inquiry access code to be set (valid values are in the
 *        range 0x9e8b00 to 0x9e8b3f), or 0 to take the device out of
 *        discoverable mode
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_set_access_code(int accessCode);

/**
 * Starts asynchronous device discovery.
 *
 * @param accessCode desired access code for the inquiry
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_start_inquiry(int accessCode);

/**
 * Cancels asynchronous device discovery.
 *
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_cancel_inquiry();

/**
 *Starts asynchronous name retrieval.
 *
 * @param bdaddr Bluetooth address of the device which name is to be retrieved
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_ask_friendly_name(const bt_bdaddr_t bdaddr);

/**
 *Starts asynchronous authentication.
 *
 * @param handle connection handle for an ACL connection
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_authenticate(int handle);

/**
 *Starts asynchronous encryption mode change.
 *
 * @param handle connection handle for an ACL connection
 * @param enable flag indicating whether the encryption needs to be enabled
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_encrypt(int handle, bt_bool_t enable);

/**
 * Checks if Bluetooth events are available.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if there are pending events,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_check_events(bt_bool_t *retval);

/**
 * Reads stack implementation-specific event data.
 *
 * @param data buffer where the data will be written to
 * @param len length of the buffer in bytes
 * @param retval pointer to variable where the result is to be stored:
 *         actual number of bytes read
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_stack_read_data(void *data, int len, int *retval);

#ifdef __cplusplus
}
#endif

#endif /* STACK_H */
