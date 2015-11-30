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

#include <btKNI.h>
#include <commonKNIMacros.h>
#include <midpUtilKni.h>
#include <midpMalloc.h>
#include <midpString.h>
#include <string.h>
#include <btBCC.h>
#include <btformat.h>
#include BLUETOOTH_STACK_HEADER

// maximum name length is 248 according to the spec
// however, we use only this many bytes in order to avoid stack overflow
#define MAX_NAME_LENGTH 128

static jfieldID nativeInstanceID = 0;
static const char *szBluetoothStateException =
    "javax/bluetooth/BluetoothStateException";

/**
 * Creates CBluetoothStack' subclass instance to be used in the current isolate
 * and stores the pointer in the nativeInstance field of the BluetoothStack
 * Java object.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_initialize(void)
{
    KNI_StartHandles(2);
    KNI_DeclareHandle(classHandle);
    KNI_DeclareHandle(thisHandle);
    KNI_GetClassPointer(classHandle);
    KNI_GetThisPointer(thisHandle);
    if (nativeInstanceID == 0) {
        nativeInstanceID = KNI_GetFieldID(classHandle, "nativeInstance", "I");
    }
    CBluetoothStack *pStack = new BLUETOOTH_STACK_CTOR;
    if (pStack->IsInitialized()) {
        KNI_SetIntField(thisHandle, nativeInstanceID, (int)pStack);
    } else {
        delete pStack;
    }
    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Deletes native Bluetooth stack instance object which was used with the
 * current isolate.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_finalize(void)
{
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    jint instance = KNI_GetIntField(thisHandle, nativeInstanceID);
    delete (CBluetoothStack *)instance;
    // No need to update the field since the finalizer won't be called twice
    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Checks if the Bluetooth radio is enabled.
 *
 * @return true if Bluetooth is enabled, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_isEnabled(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = KNI_BOOL(pStack->IsEnabled());
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Enables Bluetooth radio.
 *
 * @return true if Bluetooth is enabled, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_enable(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = KNI_BOOL(pStack->Enable());
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Returns Bluetooth address of the local device.
 *
 * @return Bluetooth address of the local device, or null if
 *         the address could not be retrieved
 */
KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_getLocalAddress(void)
{
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(stringHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        const char *string = pStack->GetLocalAddress();
        if (string != NULL) {
            KNI_NewStringUTF(string, stringHandle);
        } else {
            KNI_ReleaseHandle(stringHandle);
        }
    }
    KNI_EndHandlesAndReturnObject(stringHandle);
}

/**
 * Returns user-friendly name for the local device.
 *
 * @return User-friendly name for the local device, or null if
 *         the name could not be retrieved
 */
KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_getLocalName(void)
{
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(stringHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        char name[MAX_NAME_LENGTH + 1];
        const char *string = pStack->GetLocalName(name, sizeof(name));
        if (string != NULL) {
            KNI_NewStringUTF(string, stringHandle);
        } else {
            KNI_ReleaseHandle(stringHandle);
        }
    }
    KNI_EndHandlesAndReturnObject(stringHandle);
}

/**
 * Retrieves the class of device value that represents the service classes,
 * major device class, and minor device class of the local device.
 *
 * @return class of device value, or -1 if the information could not
 *         be retrieved
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_getDeviceClass(void)
{
    jint retval = -1;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = pStack->GetDeviceClass();
    }
    KNI_EndHandles();
    KNI_ReturnInt(retval);
}

/**
 * Sets major service class bits of the device.
 *
 * @param classes an integer whose binary representation indicates the major
 *        service class bits that should be set
 * @return true if the operation succeeded, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_setServiceClasses(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = KNI_BOOL(pStack->SetServiceClasses(KNI_GetParameterAsInt(1)));
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Retrieves the inquiry access code that the local Bluetooth device is
 * scanning for during inquiry scans.
 *
 * @return inquiry access code, or -1 if the information could not
 *         be retrieved
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_getAccessCode(void)
{
    jint retval = -1;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = pStack->GetAccessCode();
    }
    KNI_EndHandles();
    KNI_ReturnInt(retval);
}

/**
 * Sets the inquiry access code that the local Bluetooth device is
 * scanning for during inquiry scans.
 *
 * @param accessCode inquiry access code to be set (valid values are in the
 *        range 0x9e8b00 to 0x9e8b3f), or 0 to take the device out of
 *        discoverable mode
 * @return true if the operation succeeded, false otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_setAccessCode(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
            (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = KNI_BOOL(pStack->SetAccessCode(KNI_GetParameterAsInt(1)));
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Passes device discovery request to the Bluetooth stack implementation.
 *
 * @param accessCode requested access code
 * @return <code>true</code> if the operation was accepted,
 *         <code>false</code> otherwise
 * @throws BluetoothStateException if the device does not allow an inquiry
 *                                 due to other operations being performed
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_startInquiry(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
        (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = KNI_BOOL(pStack->StartInquiry(KNI_GetParameterAsInt(1)));
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Passes cancellation of device discovery request to the Bluetooth stack
 * implementation.
 *
 * @return <code>true</code> if the operation was accepted,
 *         <code>false</code> otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_cancelInquiry(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
        (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = KNI_BOOL(pStack->CancelInquiry());
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Passes remote device's friendly name acquisition request to the Bluetooth
 * stack implementation.
 *
 * @param addr Bluetooth address of the remote device
 * @return <code>true</code> if the operation was accepted,
 *         <code>false</code> otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_askFriendlyName(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_DeclareHandle(addrHandle);
    CBluetoothStack *pStack =
        (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);

    if (pStack != NULL) {
        bt_bdaddr_t addr;
        KNI_GetParameterAsObject(1, addrHandle);
        get_addr(addrHandle, addr);
        retval = KNI_BOOL(pStack->AskFriendlyName(addr));
    }

    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Gets ACL connection handle from Java string containing Bluetooth address.
 *
 * @param addressHandle handle to Java string containing Bluetooth address
 * @return ACL connection handle
 */
static int get_acl_handle(jstring addressHandle) {
    int retval;
    bt_bdaddr_t addr;
    get_addr(addressHandle, addr);
    KNI_TEST(bt_bcc_get_handle(addr, &retval) == BT_RESULT_SUCCESS);
    return retval;
}

/**
 * Passes authentication request to the Bluetooth stack implementation.
 *
 * @param addr Bluetooth address of the remote device
 * @return <code>true</code> if the operation was accepted,
 *         <code>false</code> otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_authenticate(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_DeclareHandle(addrHandle);
    CBluetoothStack *pStack =
        (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        int handle;
        KNI_GetParameterAsObject(1, addrHandle);
        handle = get_acl_handle(addrHandle);
        retval = KNI_BOOL(pStack->Authenticate(handle));
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Passes set encryption request to the Bluetooth stack implementation.
 *
 * @param addr Bluetooth address of the remote device
 * @param enable <code>true</code> if the encryption needs to be enabled,
 *               <code>false</code> otherwise
 * @return <code>true</code> if the operation was accepted,
 *         <code>false</code> otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_encrypt(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_DeclareHandle(addrHandle);
    CBluetoothStack *pStack =
        (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        int handle;
        KNI_GetParameterAsObject(1, addrHandle);
        handle = get_acl_handle(addrHandle);
        retval = KNI_BOOL(pStack->Encrypt(handle,
                KNI_GetParameterAsBoolean(2)));
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Checks if Bluetooth events are available for retrieval from the Bluetooth
 * stack implementation.
 *
 * @return <code>true</code> if there are pending events,
 *         <code>false</code> otherwise
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_checkEvents(void)
{
    jboolean retval = KNI_FALSE;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    CBluetoothStack *pStack =
        (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack != NULL) {
        retval = KNI_BOOL(pStack->CheckEvents());
    }
    KNI_EndHandles();
    KNI_ReturnBoolean(retval);
}

/**
 * Reads binary event data from the Bluetooth stack implementation.
 *
 * @param data byte array to be filled with data
 * @return number of bytes read
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_readData(void)
{
    jint retval = -1;
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_DeclareHandle(dataHandle);
    KNI_GetParameterAsObject(1, dataHandle);
    jint size = KNI_GetArrayLength(dataHandle);
    CBluetoothStack *pStack =
        (CBluetoothStack *)KNI_GetIntField(thisHandle, nativeInstanceID);
    if (pStack == NULL) {
        KNI_ThrowNew(szBluetoothStateException, NULL);
    } else {
        unsigned char *data = (unsigned char *)JavaByteArray(dataHandle);
        retval = pStack->ReadData(data, size);
    }
    KNI_EndHandles();
    KNI_ReturnInt(retval);
}

/**
 * Creates Java String object from UTF-8 encoded string.
 *
 * @param buffer buffer containing the string in UTF-8 format
 * @param offset offset of the first character in the buffer
 * @param length length of the encoded string in bytes
 * @return Java String object containing the string in UTF-2 format
 */
KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_sun_midp_jsr082_bluetooth_BluetoothStack_stringUTF8(void)
{
    KNI_StartHandles(2);
    KNI_DeclareHandle(bufferHandle);
    KNI_DeclareHandle(stringHandle);
    KNI_GetParameterAsObject(1, bufferHandle);
    jint size = KNI_GetArrayLength(bufferHandle);
    jint offset = KNI_GetParameterAsInt(2);
    jint length = KNI_GetParameterAsInt(3);
    char *string = (char *)JavaByteArray(bufferHandle) + offset;
    if (offset + length < size) {
        *(string + length) = '\0';
    } else {
        *(string - offset + size - 1) = '\0';
    }
    KNI_NewStringUTF(string, stringHandle);
    KNI_EndHandlesAndReturnObject(stringHandle);
}
