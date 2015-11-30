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

#include "IrConnection.h"

#include <kni.h>
#include <midpString.h>
#include <midpError.h>
#include <midpMalloc.h>
#include <commonKNIMacros.h>

/** @def PERROR Prints diagnostic message. */
#define PERROR(msg) \
    REPORT_WARN2(0, "%s: %s", msg, strerror(errno))

static jfieldID connHandleID = 0;

jfieldID IrNativeGetConnHandleID(void)
{
    return connHandleID;
}

/**
 * Static class initializer.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_initialize(void)
{
    KNI_StartHandles(1);
    KNI_DeclareHandle(classHandle);
    KNI_GetClassPointer(classHandle);

    connHandleID = KNI_GetFieldID(classHandle, "connHandle", "I");

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Native finalizer. Releases all native resources used by this connection.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_finalize(void)
{
    ir_handle_t sockfd;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);

    sockfd = KNI_GetIntField(thisHandle, connHandleID);
    if (sockfd != INVALID_IR_HANDLE) {
        ir_close(sockfd);
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Closes the connection.
 *
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_close(void)
{
    ir_handle_t sockfd;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);

    REPORT_INFO(0, "irdaobex::close()");
    sockfd = KNI_GetIntField(thisHandle, connHandleID);
    if (sockfd != INVALID_IR_HANDLE) {
        if (ir_close(sockfd) != IR_SUCCESS) {
            KNI_ThrowNew(midpIOException, NULL);
        }
        KNI_SetIntField(thisHandle, connHandleID, INVALID_IR_HANDLE);
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Discovers devices in range with matching hint bits and IAS.
 *
 * @param hints hint bits to be used during discovery
 * @param ias IAS string to be used during discovery
 * @param addr output array to receive addresses of discovered devices
 * @return number of discovered devices
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_discover(void)
{
    int i;
    unsigned int *addr;
    jint hints = KNI_GetParameterAsInt(1);
    jchar iasBuffer[IAS_MAX_CLASSNAME];
    char iasString[IAS_MAX_CLASSNAME + 1];
    jsize iasSize, addrSize;
    KNI_StartHandles(3);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(iasHandle);
    KNI_DeclareHandle(addrHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_GetParameterAsObject(2, iasHandle);
    KNI_GetParameterAsObject(3, addrHandle);

    iasSize = KNI_GetStringLength(iasHandle);
    addrSize = KNI_GetStringLength(addrHandle);
    if (iasSize > IAS_MAX_CLASSNAME) iasSize = IAS_MAX_CLASSNAME;
    KNI_GetStringRegion(iasHandle, 0, iasSize, iasBuffer);
    for (i = 0; i < iasSize; i++) {
        iasString[i] = iasBuffer[i];
    }
    iasString[iasSize] = 0;
    addr = JavaIntArray(addrHandle);
    REPORT_INFO1(0, "irdaobex::discover(%04x)", hints);
    if (ir_discover(hints, iasString, addr, &addrSize) != IR_SUCCESS) {
        KNI_ThrowNew(midpIOException, NULL);
    }

    KNI_EndHandles();
    KNI_ReturnInt(addrSize);
}

/**
 * Establishes a new connection to device with the specified address.
 *
 * @param ias IAS class name
 * @param addr address of the target device
 * @return true if the connection was established, false otherwise
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_connect(void)
{
    int i;
    ir_handle_t sockfd;
    jboolean ret = KNI_FALSE;
    jchar iasBuffer[IAS_MAX_CLASSNAME];
    char iasString[IAS_MAX_CLASSNAME + 1];
    jsize iasSize;
    jint addr = KNI_GetParameterAsInt(1);
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(iasHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_GetParameterAsObject(2, iasHandle);
    iasSize = KNI_GetStringLength(iasHandle);
    if (iasSize > IAS_MAX_CLASSNAME) iasSize = IAS_MAX_CLASSNAME;
    KNI_GetStringRegion(iasHandle, 0, iasSize, iasBuffer);
    for (i = 0; i < iasSize; i++) {
        iasString[i] = iasBuffer[i];
    }
    iasString[iasSize] = 0;

    REPORT_INFO1(0, "irdaobex::connect(%08x)", daddr);
    sockfd = KNI_GetIntField(thisHandle, connHandleID);
    if (sockfd == INVALID_IR_HANDLE) {
        if (ir_create(&sockfd) != IR_SUCCESS) {
            KNI_ThrowNew(midpIOException, NULL);
        } else {
            REPORT_INFO1(0, "acquired socket: %d", sockfd);
            KNI_SetIntField(thisHandle, connHandleID, sockfd);
            switch (ir_connect(sockfd, addr, iasString)) {
                case IR_SUCCESS:
                    ret = KNI_TRUE;
                    break;
                case IR_ERROR:
                    ir_close(sockfd);
                    KNI_SetIntField(thisHandle, connHandleID, INVALID_IR_HANDLE);
                    KNI_ThrowNew(midpIOException, NULL);
                    break;
                default:
                    break;
            }
        }
    } else {
        switch (ir_connect_complete(sockfd)) {
            case IR_SUCCESS:
                ret = KNI_TRUE;
                break;
            case IR_ERROR:
                ir_close(sockfd);
                KNI_SetIntField(thisHandle, connHandleID, INVALID_IR_HANDLE);
                KNI_ThrowNew(midpIOException, NULL);
                break;
            default:
                break;
        }
    }

    KNI_EndHandles();
    KNI_ReturnBoolean(ret);
}

/**
 * Reads a number of bytes to the specified byte array starting from the
 * given offset.
 *
 * @param b destination byte array
 * @param off offset in the array
 * @param len number of bytes to read
 * @return number of bytes actually read
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_read(void)
{
    ir_handle_t sockfd;
    jint off = KNI_GetParameterAsInt(2);
    jint len = KNI_GetParameterAsInt(3);
    unsigned char *buffer;
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(bufferHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_GetParameterAsObject(1, bufferHandle);
    buffer = JavaByteArray(bufferHandle);

    REPORT_INFO2(0, "irdaobex::read(%d, %d)", off, len);
    sockfd = KNI_GetIntField(thisHandle, connHandleID);
    if (sockfd == INVALID_IR_HANDLE) {
        REPORT_WARN(0, "invalid handle");
        KNI_ThrowNew(midpIOException, NULL);
    } else {
        switch (ir_receive(sockfd, buffer + off, &len)) {
            case IR_ERROR:
                KNI_ThrowNew(midpIOException, NULL);
                break;
            case IR_AGAIN:
                REPORT_INFO(0, "no data available");
                break;
            default:
                break;
        }
    }
    REPORT_INFO1(0, "read %d bytes", len);

    KNI_EndHandles();
    KNI_ReturnInt(len);
}

/**
 * Writes a number of bytes from the specified byte array starting from
 * the given offset.
 *
 * @param b source byte array
 * @param off offset in the array
 * @param len number of bytes to write
 * @return number of bytes actually written
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_write(void)
{
    int sockfd;
    jint off = KNI_GetParameterAsInt(2);
    jint len = KNI_GetParameterAsInt(3);
    unsigned char *buffer;
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(bufferHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_GetParameterAsObject(1, bufferHandle);
    buffer = JavaByteArray(bufferHandle);

    REPORT_INFO2(0, "irdaobex::write(%d, %d)", off, len);
    sockfd = KNI_GetIntField(thisHandle, connHandleID);
    if (sockfd == INVALID_IR_HANDLE) {
        REPORT_WARN(0, "invalid handle");
        KNI_ThrowNew(midpIOException, NULL);
    } else {
        switch (ir_send(sockfd, buffer + off, &len)) {
            case IR_ERROR:
                KNI_ThrowNew(midpIOException, NULL);
                break;
            case IR_AGAIN:
                REPORT_INFO(0, "operation would be blocked");
                break;
            default:
                break;
        }
    }
    REPORT_INFO1(0, "written %d bytes", count);

    KNI_EndHandles();
    KNI_ReturnInt(len);
}

/**
 * Returns an invalid handle value to be used during initialization of
 * a IrNativeConnection instance.
 *
 * @return a native handle value considered to be invalid
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_irdaobex_IrNativeConnection_getInvalidConnHandle(void)
{
    KNI_ReturnInt(INVALID_IR_HANDLE);
}
