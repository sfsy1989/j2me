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

#include <btBCC.h>
#include <string.h>
#include <midp_logging.h>
#include <emul.h>


static char *emul_pin = "1234";

/** Piconet record type. */
typedef struct {
    /** Address of a remote device. */
    bt_bdaddr_t bdaddr;
    /** Default ACL handle of the connection. */
    int handle;
    /** Number of open connections. */
    int connections;
    /** Indicates whether the device has been authenticated. */
    int authenticated;
    /** Number of encrypted connections. */
    int encrypted;
} bcc_pico_t;

/** Piconect. */
static bcc_pico_t bcc_piconet[MAX_DEVICES];

/**
 * Searches piconet record by address.
 */
static bcc_pico_t *find_pico(const bt_bdaddr_t bdaddr)
{
    int i;
    for (i = 0; i < MAX_DEVICES; i++) {
        bcc_pico_t *pico = &bcc_piconet[i];
        if (!memcmp(pico->bdaddr, bdaddr, BT_ADDRESS_SIZE)) {
            return pico;
        }
    }
    
    return NULL;
}

/**
 * Searches piconet record by address, creates new if not found.
 */
static bcc_pico_t *get_pico(const bt_bdaddr_t bdaddr) {
    bcc_pico_t *pico = find_pico(bdaddr);
    
    if (pico == NULL) {
        bt_bdaddr_t null;
        memset(null, 0, sizeof(null));
        pico = find_pico(null);
        if (pico != NULL) {
            memcpy(pico->bdaddr, bdaddr, BT_ADDRESS_SIZE);
        }
    }
    
    return pico;
}
    


/**
 * Allocates native resources.
 *
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_initialize()
{
    return BT_RESULT_SUCCESS;
}

/**
 * Releases native resources.
 *
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_finalize()
{
    return BT_RESULT_SUCCESS;
}

/**
 * Asks user whether Bluetooth radio is allowed to be turned on.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the user has allowed to enable Bluetooth,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_confirm_enable(bt_bool_t *retval)
{
    *retval = BT_BOOL_TRUE;
    return BT_RESULT_SUCCESS;
}

/**
 * Determines if the local device is in connectable mode.
 *
 * @param retval pointer to variable where the result is to be stored:
 *         BT_BOOL_TRUE if the device is connectable,
 *         BT_BOOL_FALSE otherwise
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_is_connectable(bt_bool_t *retval)
{
    *retval = BT_BOOL_TRUE;
    return BT_RESULT_SUCCESS;
}

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
bt_result_t bt_bcc_is_paired(const bt_bdaddr_t bdaddr, bt_bool_t *retval)
{
    (void)bdaddr;
    // returns false to force display of PIN entry dialog
    *retval = BT_BOOL_FALSE;
    return BT_RESULT_SUCCESS;
}

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
bt_result_t bt_bcc_is_authenticated(const bt_bdaddr_t bdaddr, bt_bool_t *retval)
{
    bcc_pico_t *pico = find_pico(bdaddr);
    *retval = BT_BOOL(pico != NULL && pico->authenticated);
    return BT_RESULT_SUCCESS;
}

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
bt_result_t bt_bcc_is_trusted(const bt_bdaddr_t bdaddr, bt_bool_t *retval)
{
    (void)bdaddr;
    *retval = BT_BOOL_FALSE;
    return BT_RESULT_SUCCESS;
}

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
bt_result_t bt_bcc_is_encrypted(const bt_bdaddr_t bdaddr, bt_bool_t *retval)
{
    bcc_pico_t *pico = find_pico(bdaddr);
    *retval = BT_BOOL(pico != NULL && pico->encrypted);
    
    return BT_RESULT_SUCCESS;
}

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
        bt_bool_t ask)
{
    (void)bdaddr;
    (void)ask;
    memcpy(pin, emul_pin, strlen(emul_pin) + 1);
    return BT_RESULT_SUCCESS;
}

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
        bt_bool_t *retval)
{
    bcc_pico_t *pico = get_pico(bdaddr);
    (void)pin;
    if (pico == NULL) {
        return BT_RESULT_FAILURE;
    }
    // link keys exchange should be performed;
    // here we just assume devices already share link keys
    pico->authenticated = 1;
    *retval = BT_BOOL_TRUE;
    return BT_RESULT_SUCCESS;
}

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
        bt_bool_t *retval)
{
    (void)bdaddr;
    (void)record;
    *retval = BT_BOOL_TRUE;
    return BT_RESULT_SUCCESS;
}

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
        int *retval)
{
    (void)buffer;
    (void)len;
    *retval = 0;
    return BT_RESULT_SUCCESS;
}

/**
 * Retrieves default ACL connection handle for the specified remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param retval pointer to variable where the result is to be stored:
 *         default ACL connection handle
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_get_handle(const bt_bdaddr_t bdaddr, int *retval)
{
    bcc_pico_t *pico = find_pico(bdaddr);
    *retval = pico != NULL ? pico->handle : -1;
    return BT_RESULT_SUCCESS;
}

/**
 * Registers a new connection to a remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param handle default ACL connection handle
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_add_connection(const bt_bdaddr_t bdaddr, int handle)
{
    bcc_pico_t *pico = get_pico(bdaddr);
    
    BT_TEST(pico != NULL);
    pico->connections++;
    pico->handle = handle;
    
    return BT_RESULT_SUCCESS;
}

/**
 * Unregisters an existing connection to a remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_remove_connection(const bt_bdaddr_t bdaddr)
{
    bcc_pico_t *pico = find_pico(bdaddr);
    BT_TEST(pico != NULL);
    if (--pico->connections == 0) {
        memset(pico, 0, sizeof(bcc_pico_t));
    }
    return BT_RESULT_SUCCESS;
}

/**
 * Returns the number of connections to the remote device.
 *
 * @param bdaddr the Bluetooth address of the remote device
 * @param retval pointer to variable where the result is to be stored:
 *         number of connections established with the remote device
 * @return BT_RESULT_SUCCESS on success,
 *         BT_RESULT_FAILURE on failure
 */
bt_result_t bt_bcc_get_connection_count(const bt_bdaddr_t bdaddr, int *retval)
{
    bcc_pico_t *pico = find_pico(bdaddr);
    *retval = pico != NULL ? pico->connections : 0;
    return BT_RESULT_SUCCESS;
}

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
        bt_bool_t *retval)
{
    bcc_pico_t *pico = get_pico(bdaddr);

    BT_TEST(pico != NULL);

    /* Turn on encryption if requested, do nothing if asked to turn it off. */     
    if (enable) {
        *retval = BT_BOOL(pico->encrypted == 0);
        pico->encrypted = 1;
    } else {
        *retval = BT_BOOL_FALSE;
    }
    
    return BT_RESULT_SUCCESS;
}
