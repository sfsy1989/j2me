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

#ifndef BCC_H
#define BCC_H

#include <btCommon.h>

/**
 * @defgroup jsr82bcc JSR82 BCC Porting Interface
 * @ingroup jsr82
 * @brief JSR82 BCC Porting Interface.
 *
 * Group description pending.
 */

/**
 * @file
 * @ingroup jsr82bcc
 * @brief #include <NativeBCC.h>
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
bt_result_t bt_bcc_initialize();

/**
 * Releases native resources.
 *
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_finalize();

/**
 * Asks user whether Bluetooth radio is allowed to be turned on.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the user has allowed to enable Bluetooth,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_confirm_enable(bt_bool_t *retval);

/**
 * Determines if the local device is in connectable mode.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the device is connectable,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_is_connectable(bt_bool_t *retval);

/**
 * Checks if the local device has a bond with a remote device.
 *
 * @param bdaddr Bluetooth address of a remote device
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the devices are paired,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_is_paired(const bt_bdaddr_t bdaddr, bt_bool_t *retval);

/**
 * Checks if a remote device was authenticated.
 *
 * @param bdaddr Bluetooth address of a remote device
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the remote device is authenticated,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_is_authenticated(const bt_bdaddr_t bdaddr, bt_bool_t *retval);

/**
 * Checks if a remote device is trusted (authorized for all services).
 *
 * @param bdaddr Bluetooth address of a remote device
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the remote device is trusted,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_is_trusted(const bt_bdaddr_t bdaddr, bt_bool_t *retval);

/**
 * Determines if data exchanges with a remote device are being encrypted.
 *
 * @param bdaddr Bluetooth address of a remote device
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if connection to the device is encrypted,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_is_encrypted(const bt_bdaddr_t bdaddr, bt_bool_t *retval);

/**
 * Retrieves PIN code to use for pairing with a remote device. If the
 * PIN code is not known, PIN entry dialog is displayed.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param pin array to receive the PIN code (null-terminated UTF-8 encoded)
 * @param ask indicates whether PIN can be retrieved from cache, or user must be
 *         asked to enter one (regardless whether cached PIN is available)
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure,
 *         BT_RESULT_WOULDBLOCK if async dialog was displayed
 */
bt_result_t bt_bcc_get_passkey(const bt_bdaddr_t bdaddr, bt_string_t pin,
        bt_bool_t ask);

/**
 * Initiates pairing with a remote device.
 *
 * @param bdaddr the Bluetooth address of the device with which to pair
 * @param pin an array containing the PIN code
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the device was paired,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_bond(const bt_bdaddr_t bdaddr, const bt_string_t pin,
        bt_bool_t *retval);

/**
 * Authorizes a Bluetooth connection.
 *
 * @param bdaddr Bluetooth address of a remote device
 * @param record handle for the service record of the srvice the remote
 *         device is trying to access
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the connection was authorized,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_authorize(const bt_bdaddr_t bdaddr, int record,
        bt_bool_t *retval);

/**
 * Returns list of preknown devices in a packed string.
 *
 * @param buffer output buffer which will receive the list of preknown devices
 * @param len length of the buffer
 * @param retval pointer to variable where the result is to be stored:
 *         number of records stored in the buffer
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_get_preknown_devices(bt_string_t buffer, int len,
        int *retval);

/**
 * Retrieves default ACL connection handle for the specified remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param retval pointer to variable where the result is to be stored:
 *         default ACL connection handle
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_get_handle(const bt_bdaddr_t bdaddr, int *retval);

/**
 * Registers a new connection to a remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param handle default ACL connection handle
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_add_connection(const bt_bdaddr_t bdaddr, int handle);

/**
 * Unregisters an existing connection to a remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_remove_connection(const bt_bdaddr_t bdaddr);

/**
 * Returns the number of connections to the remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param retval pointer to variable where the result is to be stored:
 *         number of connections established with the remote device
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_get_connection_count(const bt_bdaddr_t bdaddr, int *retval);

/**
 * Increases or decreases encryption request counter for a remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param enable indicated whether the encryption needs to be enabled
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the encryption needs to been changed,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_set_encryption(const bt_bdaddr_t bdaddr, bt_bool_t enable,
        bt_bool_t *retval);

/**
 * Provides the implementation with the PIN code which was requested earlier
 * using bt_bcc_get_passkey() function. If the user has canceled PIN entry,
 * NULL is returned. PIN code should be stored, so it could be retrieved with
 * the subsequent bt_bcc_get_passkey() call.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param pin array containing the PIN code (null-terminated UTF-8 encoded),
 *         or NULL if user has canceled PIN entry
 */
void bt_bcc_notify_passkey(const bt_bdaddr_t bdaddr, const bt_string_t pin);

#ifdef __cplusplus
}
#endif

#endif
