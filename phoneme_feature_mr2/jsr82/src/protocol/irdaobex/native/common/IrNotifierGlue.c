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
#include <midp_logging.h>
#include <midpString.h>
#include <midpError.h>
#include <midpMalloc.h>
#include <commonKNIMacros.h>

static jfieldID peerHandleID = 0;
static jfieldID dataHandleID = 0;

jfieldID IrNativeGetConnHandleID(void);

/**
 * Static class initializer.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_initialize(void)
{
    KNI_StartHandles(1);
    KNI_DeclareHandle(classHandle);
    KNI_GetClassPointer(classHandle);

    peerHandleID = KNI_GetFieldID(classHandle, "peerHandle", "I");
    dataHandleID = KNI_GetFieldID(classHandle, "dataHandle", "I");

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Native finalizer. Releases all native data used by this notifier.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_finalize(void)
{
    ir_handle_t peerfd, *data;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);

    peerfd = KNI_GetIntField(thisHandle, peerHandleID);
    if (peerfd != INVALID_IR_HANDLE) {
        ir_close(peerfd);
    }
    data = (ir_handle_t *)KNI_GetIntField(thisHandle, dataHandleID);
    if (data != INVALID_DATA_HANDLE) {
        int i;
        ir_handle_t *psockfd = data;
        for (i = 0; i < MAX_SOCKETS; ++i) {
            if (*psockfd != INVALID_IR_HANDLE) {
                ir_close(*psockfd);
            }
            ++psockfd;
        }
        midpFree(data);
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Allocates native connection for listening with specified
 * hint bits and IAS.
 *
 * @param index connection number starting from 0
 * @param hints hint bits to be set
 * @param ias IAS to be advertised
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_listen(void)
{
    int i;
    ir_handle_t sockfd, *data;
    jint index = KNI_GetParameterAsInt(1);
    jint hints = KNI_GetParameterAsInt(2);
    jchar iasBuffer[IAS_MAX_CLASSNAME];
    char iasString[IAS_MAX_CLASSNAME + 1];
    jsize iasSize;
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(iasHandle);
    KNI_GetThisPointer(thisHandle);

    REPORT_INFO2(0, "irdaobex::listen(%d, %04x)", index, hints);
    data = (ir_handle_t *)KNI_GetIntField(thisHandle, dataHandleID);
    if (data == INVALID_DATA_HANDLE) {
        data = midpMalloc(MAX_SOCKETS * sizeof(ir_handle_t));
        if (data != INVALID_DATA_HANDLE) {
            for (i = 0; i < MAX_SOCKETS; ++i) {
                *(data + i) = INVALID_IR_HANDLE;
            }
            KNI_SetIntField(thisHandle, dataHandleID, (int)data);
        }
    }
    if (data == INVALID_DATA_HANDLE || index < 0 || index >= MAX_SOCKETS) {
        REPORT_WARN(0, "error accessing socket array");
        KNI_ThrowNew(midpIOException, NULL);
    } else {
        KNI_GetParameterAsObject(3, iasHandle);
        iasSize = KNI_GetStringLength(iasHandle);
        if (iasSize > IAS_MAX_CLASSNAME) iasSize = IAS_MAX_CLASSNAME;
        KNI_GetStringRegion(iasHandle, 0, iasSize, iasBuffer);
        for (i = 0; i < iasSize; i++) {
            iasString[i] = iasBuffer[i];
        }
        iasString[iasSize] = 0;
        if (ir_create(&sockfd) != IR_SUCCESS) {
            KNI_ThrowNew(midpIOException, NULL);
        } else {
            REPORT_INFO1(0, "acquired socket: %d", sockfd);
            if (ir_listen(sockfd, hints, iasString) != IR_SUCCESS) {
                ir_close(sockfd);
                KNI_ThrowNew(midpIOException, NULL);
            } else {
                *(data + index) = sockfd;
            }
        }
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Accepts a pending client connection.
 *
 * @param index connection number starting from 0
 * @return true if the connection was established, false otherwise
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_accept(void)
{
    jboolean ret = KNI_FALSE;
    ir_handle_t sockfd, peerfd, *data;
    jint index = KNI_GetParameterAsInt(1);
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);

    REPORT_INFO1(0, "irdaobex::accept(%d)", index);
    data = (ir_handle_t *)KNI_GetIntField(thisHandle, dataHandleID);
    if (data == INVALID_DATA_HANDLE || index < 0 || index >= MAX_SOCKETS) {
        REPORT_WARN(0, "error accessing socket array");
        KNI_ThrowNew(midpIOException, NULL);
    } else {
        sockfd = *(data + index);
        if (sockfd == INVALID_IR_HANDLE) {
            REPORT_WARN(0, "invalid socket");
            KNI_ThrowNew(midpIOException, NULL);
        } else {
            switch (ir_accept(sockfd, &peerfd)) {
                case IR_SUCCESS:
                    REPORT_INFO1(0, "acquired peer socket: %d", peerfd);
                    KNI_SetIntField(thisHandle, peerHandleID, peerfd);
                    ret = KNI_TRUE;
                    break;
                case IR_ERROR:
                    KNI_ThrowNew(midpIOException, NULL);
                    break;
                case IR_AGAIN:
                    REPORT_INFO(0, "no pending connections");
                    break;
                default:
                    break;
            }
        }
    }

    KNI_EndHandles();
    KNI_ReturnBoolean(ret);
}

/**
 * Releases native resources.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_release(void)
{
    ir_handle_t peerfd, *data;
    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);

    REPORT_INFO(0, "irdaobex::release()");
    peerfd = KNI_GetIntField(thisHandle, peerHandleID);
    if (peerfd != INVALID_IR_HANDLE) {
        ir_close(peerfd);
        KNI_SetIntField(thisHandle, peerHandleID, INVALID_IR_HANDLE);
    }
    data = (ir_handle_t *)KNI_GetIntField(thisHandle, dataHandleID);
    if (data != INVALID_DATA_HANDLE) {
        int i;
        ir_handle_t *psockfd = data;
        for (i = 0; i < MAX_SOCKETS; ++i) {
            if (*psockfd != INVALID_IR_HANDLE) {
                ir_close(*psockfd);
            }
            ++psockfd;
        }
        midpFree(data);
        KNI_SetIntField(thisHandle, dataHandleID, INVALID_DATA_HANDLE);
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Passes ownership of the native peer handle to the IrNativeConnection
 * object.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_passPeer(void)
{
    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(peerHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_GetParameterAsObject(1, peerHandle);

    KNI_SetIntField(peerHandle, IrNativeGetConnHandleID(),
    KNI_GetIntField(thisHandle, peerHandleID));
    KNI_SetIntField(thisHandle, peerHandleID, INVALID_IR_HANDLE);

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Returns an invalid connection handle value to be used during initialization
 * of a IrNativeNotifier instance.
 *
 * @return a native connection handle value considered to be invalid
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_getInvalidPeerHandle(void)
{
    KNI_ReturnInt(INVALID_IR_HANDLE);
}

/**
 * Returns an invalid memory handle value to be used during initialization of
 * a IrNativeNotifier instance.
 *
 * @return a native memory handle value considered to be invalid
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_irdaobex_IrNativeNotifier_getInvalidDataHandle(void)
{
    KNI_ReturnInt(INVALID_DATA_HANDLE);
}
