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

#include <stdlib.h>
#include <btKNI.h>
#include <commonKNIMacros.h>
#include <midpError.h>
#include <midpMalloc.h>
#include <midpUtilKni.h>
#include <midp_thread.h>
#include <sni.h>
#include <midpUtilKni.h>
#include <midpError.h>

#include <btBCC.h>
#include <btformat.h>

#define MAX_PREKNOWN_DEVICES 10

/**
 * Allocates native resources.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_initialize(void)
{
    bt_bcc_initialize();
    KNI_ReturnVoid();
}

/**
 * Releases native resources.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_finalize(void)
{
    bt_bcc_finalize();
    KNI_ReturnVoid();
}

/**
 * Asks user whether Bluetooth radio is allowed to be turned on.
 *
 * @return true if user has allowed to enable Bluetooth, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_confirmEnable(void)
{
    bt_bool_t retval;
    KNI_TEST(bt_bcc_confirm_enable(&retval) == BT_RESULT_SUCCESS);
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Determines if the local device is in connectable mode.
 *
 * @return true if the device is connectable, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_isConnectable(void)
{
    bt_bool_t retval;
    KNI_TEST(bt_bcc_is_connectable(&retval) == BT_RESULT_SUCCESS);
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Checks if the local device has a bond with a remote device.
 *
 * @param address Bluetooth address of a remote device
 * @return true if the two devices were paired, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_isPaired(void)
{
    bt_bool_t retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_is_paired(addr, &retval) == BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Checks if a remote device was authenticated.
 *
 * @param address Bluetooth address of a remote device
 * @return true if the device was authenticated, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_isAuthenticated(void)
{
    bt_bool_t retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_is_authenticated(addr, &retval) == BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Checks if a remote device is trusted (authorized for all services).
 *
 * @param address Bluetooth address of a remote device
 * @return true if the device is trusted, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_isTrusted(void)
{
    bt_bool_t retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_is_trusted(addr, &retval) == BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Determines if data exchanges with a remote device are being encrypted.
 *
 * @param address Bluetooth address of a remote device
 * @return true if connection to the device is encrypted, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_isEncrypted(void)
{
    bt_bool_t retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_is_encrypted(addr, &retval) == BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Retrieves PIN code to use for pairing with a remote device. If the
 * PIN code is not known, PIN entry dialog is displayed.
 *
 * @param address the Bluetooth address of the remote device
 * @return string containing the PIN code
 */
KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_getPasskey(void)
{
    char passkey[16];
    bt_bdaddr_t addr;
    KNI_StartHandles(2);
    KNI_DeclareHandle(addressHandle);
    KNI_DeclareHandle(passkeyHandle);
    MidpReentryData *reentry = (MidpReentryData *)SNI_GetReentryData(NULL);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    if (reentry == NULL) {
        switch (bt_bcc_get_passkey(addr, passkey, BT_BOOL_TRUE)) {
            case BT_RESULT_SUCCESS:
                KNI_NewStringUTF(passkey, passkeyHandle);
                break;
            case BT_RESULT_FAILURE:
                KNI_ReleaseHandle(passkeyHandle);
                break;
            case BT_RESULT_WOULDBLOCK:
                // Need revisit: add BT_SIGNAL to the midp workspace or
                // use existing signal instead
//                midp_thread_wait(BT_SIGNAL, 1, NULL);
                break;
            default:
                break;
        }
    } else {
        if (reentry->status) {
            switch (bt_bcc_get_passkey(addr, passkey, BT_BOOL_FALSE)) {
                case BT_RESULT_SUCCESS:
                    KNI_NewStringUTF(passkey, passkeyHandle);
                    break;
                case BT_RESULT_FAILURE:
                    KNI_ReleaseHandle(passkeyHandle);
                    break;
                default:
                    break;
            }
        } else {
            KNI_ReleaseHandle(passkeyHandle);
        }
    }
    KNI_EndHandlesAndReturnObject(passkeyHandle);
}

/**
 * Initiates pairing with a remote device.
 *
 * @param address the Bluetooth address of the device with which to pair
 * @param pin an array containing the PIN code
 * @return true if the device was authenticated, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_bond(void)
{
    bt_bool_t retval;
    bt_bdaddr_t addr;
    MidpString str;
    bt_string_t pin;
    KNI_StartHandles(2);
    KNI_DeclareHandle(addressHandle);
    KNI_DeclareHandle(pinHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    KNI_GetParameterAsObject(2, pinHandle);
    get_addr(addressHandle, addr);
    str = midpNewString(pinHandle);
    pin = midpJcharsToChars(str);
    KNI_TEST(bt_bcc_bond(addr, pin, &retval) == BT_RESULT_SUCCESS);
    midpFree(pin);
    MIDP_FREE_STRING(str);
    KNI_EndHandles();
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Authorizes a Bluetooth connection.
 *
 * @param address Bluetooth address of a remote device
 * @param handle handle for the service record of the srvice the remote
 *         device is trying to access
 * @return true if authorization succeeded, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_authorize(void)
{
    bt_bool_t retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_authorize(addr, KNI_GetParameterAsInt(2), &retval) ==
            BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnBoolean(KNI_BOOL(retval));
}

/**
 * Returns list of preknown devices in a packed string.
 *
 * @return packed string containing preknown devices
 */
KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_getPreknown(void)
{
    char buffer[MAX_PREKNOWN_DEVICES * 13];
    int retval;
    KNI_StartHandles(1);
    KNI_DeclareHandle(stringHandle);
    KNI_TEST(bt_bcc_get_preknown_devices(buffer, sizeof(buffer), &retval) ==
            BT_RESULT_SUCCESS);
    if (retval > 0) {
        KNI_NewStringUTF(buffer, stringHandle);
    } else {
        KNI_ReleaseHandle(stringHandle);
    }
    KNI_EndHandlesAndReturnObject(stringHandle);
}

/**
 * Checks if there is a connection to the remote device.
 *
 * @param address the Bluetooth address of the remote device
 * @return true if connection is established with the remote device
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_isConnected(void)
{
    int retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_get_connection_count(addr, &retval) == BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnBoolean(KNI_BOOL(retval > 0));
}

/**
 * Increases or decreases encryption request counter for a remote device.
 *
 * @param address the Bluetooth address of the remote device
 * @param enable indicated whether the encryption needs to be enabled
 * @return true if the encryption needs to been changed, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_kvem_jsr082_bluetooth_NativeBCC_setEncryption(void)
{
    bt_bool_t retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_set_encryption(addr, KNI_GetParameterAsBoolean(2), &retval)
              == BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnInt(KNI_BOOL(retval));
}

/**
 * Retrieves default ACL connection handle for the specified remote device.
 *
 * @param address the Bluetooth address of the remote device
 * @return ACL connection handle value
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_getHandle(void)
{
    int retval;
    bt_bdaddr_t addr;
    KNI_StartHandles(1);
    KNI_DeclareHandle(addressHandle);
    KNI_GetParameterAsObject(1, addressHandle);
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_get_handle(addr, &retval) == BT_RESULT_SUCCESS);
    KNI_EndHandles();
    KNI_ReturnInt(retval);
}
