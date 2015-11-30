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
#include <btMacros.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <midp_logging.h>
#include <midp_thread.h>

/** @def PINENTRY pinentry tool executable. */
#define PINENTRY "pinentry-gtk-2"

static bt_bdaddr_t g_pin_bdaddr;
static char g_pin[17];
// IMPL_NOTE: The following symbols are not currently used.
// static int g_pin_in, g_pin_out;
// static pthread_t pin_thread;
// static void *request_pin(void *ptr);

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
    if (ask) {
        return BT_RESULT_FAILURE;
        // IMPL_NOTE: this was just an emulation of BCC, but due use of blunt RPC
        // methods which were not likely to be suitable for embedded
        // platform, the code is currently commented out. Some common
        // solution for handling native dialogs should be used instead.
        /*
        int readfd[2], writefd[2];
        pid_t pid;
        if (pin_thread != 0) {
            return BT_RESULT_FAILURE;
        }
        memcpy(g_pin_bdaddr, bdaddr, BT_ADDRESS_SIZE);
        if (pipe(readfd) == -1 || pipe(writefd) == -1) {
            return BT_RESULT_FAILURE;
        }
        pid = fork();
        if (pid == -1) {
            return BT_RESULT_FAILURE;
        }
        if (pid == 0) {
            if (dup2(readfd[1], STDOUT_FILENO) == -1 ||
                    dup2(writefd[0], STDIN_FILENO) == -1) {
                exit(1);
            }
            close(readfd[0]);
            close(readfd[1]);
            close(writefd[0]);
            close(writefd[1]);
            execlp(PINENTRY, PINENTRY, (char *)NULL);
            exit(1);
        } else {
            close(readfd[1]);
            close(writefd[0]);
            g_pin_in = readfd[0];
            g_pin_out = writefd[1];
            pthread_create(&pin_thread, NULL, request_pin, NULL);
            return BT_RESULT_WOULDBLOCK;
        }
        */
    }
    if (g_pin == NULL || memcmp(g_pin_bdaddr, bdaddr, BT_ADDRESS_SIZE) != 0) {
        return BT_RESULT_FAILURE;
    }
    memcpy(pin, g_pin, sizeof(g_pin));
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
    bcc_pico_t *pico = find_pico(bdaddr);
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
    bcc_pico_t *pico = find_pico(bdaddr);
    bt_bdaddr_t null;
    if (pico != NULL) {
        pico->connections++;
        return BT_RESULT_SUCCESS;
    }
    memset(null, 0, sizeof(null));
    pico = find_pico(null);
    BT_TEST(pico != NULL);
    memcpy(pico->bdaddr, bdaddr, BT_ADDRESS_SIZE);
    pico->connections = 1;
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
    bcc_pico_t *pico = find_pico(bdaddr);
    BT_TEST(pico != NULL);
    if (enable) {
        *retval = BT_BOOL(++pico->encrypted == 1);
        return BT_RESULT_SUCCESS;
    }
    BT_TEST(pico->encrypted > 0);
    *retval = BT_BOOL(--pico->encrypted == 0);
    return BT_RESULT_SUCCESS;
}

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
void bt_bcc_notify_passkey(const bt_bdaddr_t bdaddr, const bt_string_t pin)
{
    memcpy(g_pin_bdaddr, bdaddr, BT_ADDRESS_SIZE);
    if (pin != NULL) {
        memcpy(g_pin, pin, 16);
          // IMPL_NOTE: add BT_SIGNAL to the midp workspace or
          // use existing signal instead
//        midp_thread_signal(BT_SIGNAL, 1, 1);
    } else {
          // IMPL_NOTE: add BT_SIGNAL to the midp workspace or
          // use existing signal instead
//        midp_thread_signal(BT_SIGNAL, 1, 0);
    }
}

/*
 * Thread which communicates with native PIN entry dialog.
 * IMPL_NOTE: This function was used to communicate with other process, and is
 * not currently used.
 */
/*
void *request_pin(void *ptr)
{
    char pin[17];
    char buf[128];
    size_t nread;
    (void)ptr;
    read(g_pin_in, buf, sizeof(buf)); // OK Your orders please
    sprintf(buf, "SETDESC Please enter PIN to authenticate " \
        "%02X:%02X:%02X:%02X:%02X:%02X\n",
        g_pin_bdaddr[5], g_pin_bdaddr[4], g_pin_bdaddr[3],
        g_pin_bdaddr[2], g_pin_bdaddr[1], g_pin_bdaddr[0]);
    write(g_pin_out, buf, strlen(buf));
    read(g_pin_in, buf, sizeof(buf)); // OK
    write(g_pin_out, "GETPIN\n", 7);
    nread = read(g_pin_in, buf, sizeof(buf) - 1);
    buf[nread] = '\0';
    if (*buf == 'D') {
        *strchr(buf, '\n') = '\0';
        strncpy(pin, buf + 2, 16);
        pin[16] = '\0';
        bt_bcc_notify_passkey(g_pin_bdaddr, pin);
    } else {
        bt_bcc_notify_passkey(g_pin_bdaddr, NULL);
    }
    close(g_pin_in);
    close(g_pin_out);
    pin_thread = 0;
    return NULL;
}
*/
