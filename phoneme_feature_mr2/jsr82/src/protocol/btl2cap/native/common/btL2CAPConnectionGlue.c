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

#include <btMacros.h>
#include <btL2CAPConnection.h>
#include <btL2CAPCommon.h>
#include <btBCC.h>

#include <midp_thread.h>
#include <sni.h>
#include <midp_libc_ext.h>
#include <kni_globals.h>
//#include <midpResourceLimit.h>
#include <midpUtilKni.h>

static jfieldID connHandleID = NULL;
static jfieldID remoteAddrID = NULL;

/**
 * Registers the connection in BCC.
 * It's neccassary to call the function after new L2CAP connection is
 * estabished.
 *
 * @param addr bluetooth address of remote device
 * @param handle L2CAP connection handle
 * @return <code>BT_BOOL_TRUE</code> if success,
 *         <code>BT_BOOL_FALSE</code> otherwise
 */
bt_bool_t RegisterL2CAPConnection0(bt_bdaddr_t addr, bt_handle_t handle) {
    int acl_handle;
    bt_bool_t res;

    REPORT_INFO(LC_PROTOCOL, "Register L2CAP connection");

    if (bt_l2cap_get_acl_handle(handle, &acl_handle) == BT_RESULT_FAILURE) {
        res = BT_BOOL_FALSE;
        REPORT_ERROR(LC_PROTOCOL, "Getting L2CAP ACL handle failed");
    } else {
        if (bt_bcc_add_connection(addr, acl_handle) == BT_RESULT_FAILURE) {
            res = BT_BOOL_FALSE;
            REPORT_ERROR(LC_PROTOCOL,
                "Registering of the L2CAP connection in BCC failed");
        } else {
            res = BT_BOOL_TRUE;
            REPORT_INFO(LC_PROTOCOL, "Register L2CAP connection done!");
        }
    }

    return res;
}

/**
 * Internal unregistration function.
 * Unregisters the connection in BCC.
 *
 * @param thisHandle KNI handle to this object
 * @return <code>KNI_TRUE</code> if success,
 *         <code>KNI_FALSE</code> otherwise
 */
static bt_bool_t UnregisterL2CAPConnection(jobject thisHandle) {
    int res;
    bt_bdaddr_t addr;

    REPORT_INFO(LC_PROTOCOL, "Unregister L2CAP connection");

    KNI_StartHandles(1);
    KNI_DeclareHandle(arrayHandle);
    KNI_GetObjectField(thisHandle, remoteAddrID, arrayHandle);

    /* copy address from Java input array */
    SNI_BEGIN_RAW_POINTERS;
    memcpy(addr, JavaByteArray(arrayHandle), BT_ADDRESS_SIZE);
    SNI_END_RAW_POINTERS;

    if (bt_bcc_remove_connection(addr) == BT_RESULT_FAILURE) {
        res = BT_BOOL_FALSE;
        REPORT_ERROR(LC_PROTOCOL,
            "Unregistering of the L2CAP connection in BCC failed");
    } else {
        res = BT_BOOL_TRUE;
        REPORT_INFO(LC_PROTOCOL, "Unregister L2CAP connection done!");
    }

    KNI_EndHandles();
    return res;
}

/**
 * Retrieves native connection handle from temporary storage
 * inside <code>L2CAPNotifierImpl</code> instance
 * and sets it to this <code>L2CAPConnectionImpl</code> instance.
 *
 * Note: the method sets native connection handle directly to
 * <code>handle<code> field of <code>L2CAPConnectionImpl</code> object.
 *
 * @param notif reference to corresponding <code>L2CAPNotifierImpl</code>
 *              instance storing native peer handle
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_setThisConnHandle0(void) {

    REPORT_INFO(LC_PROTOCOL, "btl2cap::setThisConnHandle");

    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(notifHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_GetParameterAsObject(1, notifHandle);

    if (KNI_IsNullHandle(notifHandle)) {
        REPORT_ERROR(LC_PROTOCOL,
            "Notifier handle is null in btl2cap::setThisConnHandle");
    } else {
        jfieldID peerHandleID = GetL2CAPPeerHandleID();

        if (peerHandleID == NULL) {
            REPORT_ERROR(LC_PROTOCOL,
                "Peer handle ID is not initialized"
                "in btl2cap::setThisConnHandle");
        } else {
            bt_handle_t handle =
                (bt_handle_t)KNI_GetIntField(notifHandle, peerHandleID);

            if (handle != BT_INVALID_HANDLE) {
                /* store native connection handle to Java */
                KNI_SetIntField(thisHandle, connHandleID, (jint)handle);

                /* reset temporary storing field */
                KNI_SetIntField(notifHandle,
                    peerHandleID, (jint)BT_INVALID_HANDLE);

                REPORT_INFO(LC_PROTOCOL, "btl2cap::setThisConnHandle done!");
            } else {
                REPORT_ERROR(LC_PROTOCOL,
                    "Peer handle is invalid in btl2cap::setThisConnHandle");
            }
        }
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Native static class initializer.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_initialize(void) {

    REPORT_INFO(LC_PROTOCOL, "btl2cap::initialize");

    KNI_StartHandles(1);
    KNI_DeclareHandle(classHandle);
    KNI_GetClassPointer(classHandle);

    GET_FIELDID(classHandle, "handle", "I", connHandleID)
    GET_FIELDID(classHandle, "remoteDeviceAddress", "[B", remoteAddrID)

    REPORT_INFO(LC_PROTOCOL, "btl2cap::initialize done!");

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Native finalizer.
 * Releases all native resources used by this connection.
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_finalize(void) {
    bt_handle_t handle;
    int status = BT_RESULT_FAILURE;

    REPORT_INFO(LC_PROTOCOL, "btl2cap::finalize");

    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);

    handle = (bt_handle_t)KNI_GetIntField(thisHandle, connHandleID);

    if (handle != BT_INVALID_HANDLE) {
        status = bt_l2cap_close(handle);

        KNI_SetIntField(thisHandle, connHandleID, (jint)BT_INVALID_HANDLE);
        // IMPL_NOTE: add resource counting
/*
        if (midpDecResourceCount(RSC_TYPE_BT_CLI, 1) == 0) {
            REPORT_INFO(LC_PROTOCOL, "Resource limit update error");
        }
*/
        if (status == BT_RESULT_FAILURE) {
            char* pError;
            bt_l2cap_get_error(handle, &pError);
            midp_snprintf(gKNIBuffer, KNI_BUFFER_SIZE,
                "IO error in btl2cap::finalize (%s)\n", pError);
            REPORT_ERROR(LC_PROTOCOL, gKNIBuffer);
        } else if (status == BT_RESULT_WOULDBLOCK) {
            /* blocking during finalize is not supported */
            REPORT_CRIT1(LC_PROTOCOL,
                "btl2cap::finalize blocked, handle= %d\n", handle);
        }

       UnregisterL2CAPConnection(thisHandle);
    }
    // IMPL_NOTE: add bluetooth activity indicator
/*    FINISH_BT_INDICATOR; */


    REPORT_INFO(LC_PROTOCOL, "btl2cap::finalize done!");

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Creates a client connection object.
 *
 * Note: the method sets native connection handle directly to
 * <code>handle<code> field of <code>L2CAPConnectionImpl</code> object.
 *
 * @param imtu receive MTU or <code>-1</code> if not specified
 * @param omtu transmit MTU or <code>-1</code> if not specified
 * @param auth   <code>true</code> if authication is required
 * @param enc    <code>true</code> indicates
 *                what connection must be encrypted
 * @param master <code>true</code> if client requires to be
 *               a connection's master
 *
 * @throws IOException if any I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_create0(void) {
    int imtu = (int)KNI_GetParameterAsInt(1);
    int omtu = (int)KNI_GetParameterAsInt(2);
    bt_bool_t auth  = (KNI_GetParameterAsBoolean(3) == KNI_TRUE)
        ? BT_BOOL_TRUE : BT_BOOL_FALSE;
    bt_bool_t enc  = (KNI_GetParameterAsBoolean(4) == KNI_TRUE)
        ? BT_BOOL_TRUE : BT_BOOL_FALSE;
    bt_bool_t master  = (KNI_GetParameterAsBoolean(5) == KNI_TRUE)
        ? BT_BOOL_TRUE : BT_BOOL_FALSE;

    bt_handle_t handle = BT_INVALID_HANDLE;

    REPORT_INFO(LC_PROTOCOL, "btl2cap::create");

    /* create L2CAP server connection */
    if (bt_l2cap_create_client(imtu, omtu, auth, enc, master, &handle)
            == BT_RESULT_FAILURE) {
        REPORT_ERROR(LC_PROTOCOL,
            "Connection creation failed during btl2cap::create");
        KNI_ThrowNew(midpIOException,
            EXCEPTION_MSG("Can not create L2CAP connection"));
        KNI_ReturnVoid();
    }

    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);

    /* store native connection handle to Java object */
    KNI_SetIntField(thisHandle, connHandleID, (jint)handle);

    REPORT_INFO1(LC_PROTOCOL,
        "btl2cap::create0 handle=%d connection created", handle);

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Performs client connection establishment.
 *
 * Note: the method gets native connection handle directly from
 * <code>handle<code> field of <code>L2CAPConnectionImpl</code> object.
 *
 * @param addr bluetooth address of device to connect to
 * @param psm Protocol Service Multiplexor (PSM) value
 * @return Negotiated ReceiveMTU and TransmitMTU.
 *               16 high bits is ReceiveMTU, 16 low bits is TransmitMTU.
 *
 * @throws IOException if any I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_connect0(void) {
    unsigned char *address = NULL;
    int psm  = (int)KNI_GetParameterAsInt(2);

    bt_handle_t handle = BT_INVALID_HANDLE;
    int status, i, imtu, omtu, mtus;
    void* context = NULL;
    MidpReentryData* info;
    bt_bdaddr_t addr;

    KNI_StartHandles(2);
    KNI_DeclareHandle(thisHandle);
    KNI_DeclareHandle(arrayHandle);
    KNI_GetThisPointer(thisHandle);
    KNI_GetParameterAsObject(1, arrayHandle);
    handle = (bt_handle_t)KNI_GetIntField(thisHandle, connHandleID);

    REPORT_INFO1(LC_PROTOCOL, "btl2cap::connect handle=%d", handle);

    /* copy address from Java input array */
    SNI_BEGIN_RAW_POINTERS;
    address = (unsigned char*)JavaByteArray(arrayHandle);
    for (i = 0; i < BT_ADDRESS_SIZE; i++) {
        addr[i] = address[i];
    }
    SNI_END_RAW_POINTERS;

    info = (MidpReentryData*)SNI_GetReentryData(NULL);
    if (info == NULL) {   /* First invocation */
        // IMPL_NOTE: add resource counting
        /**
         * Verify that the resource is available well within limit as per
         * the policy in ResourceLimiter
         */
/*
        if (midpCheckResourceLimit(RSC_TYPE_BT_CLI, 1) == 0) {
            const char* pMsg = "Resource limit exceeded for BT client sockets";
            REPORT_INFO(LC_PROTOCOL, pMsg);
            KNI_ThrowNew(midpIOException, EXCEPTION_MSG(pMsg));
        } else {
*/
            status = bt_l2cap_connect_start(handle, address, psm,
            &imtu, &omtu, &context);

            if (status == BT_RESULT_SUCCESS) {
                // IMPL_NOTE: add resource counting
/*
                if (midpIncResourceCount(RSC_TYPE_BT_CLI, 1) == 0) {
                    REPORT_INFO(LC_PROTOCOL, "Resource limit update error");
                }
*/
                 RegisterL2CAPConnection0(addr, handle);
            } else if (status == BT_RESULT_FAILURE) {
                char* pError;
                bt_l2cap_get_error(handle, &pError);
                midp_snprintf(gKNIBuffer, KNI_BUFFER_SIZE,
                        "IO error in btl2cap::connect (%s)\n", pError);
                REPORT_INFO(LC_PROTOCOL, gKNIBuffer);
                KNI_ThrowNew(midpIOException, EXCEPTION_MSG(gKNIBuffer));
            } else if (status == BT_RESULT_WOULDBLOCK) {
                // IMPL_NOTE: add bluetooth activity indicator
//                INC_BT_INDICATOR;
                // IMPL_NOTE: add resource counting
/*
                if (midpIncResourceCount(RSC_TYPE_BT_CLI, 1) == 0) {
                    REPORT_INFO(LC_PROTOCOL, "Resource limit update error");
                }
*/
                REPORT_INFO1(LC_PROTOCOL,
                    "btl2cap::connect is waiting for complete notify"
                    ", handle = %d\n", handle);
                midp_thread_wait(NETWORK_WRITE_SIGNAL, (int)handle, context);
            } else {
                char* pMsg = "Unknown error during btl2cap::connect";
                REPORT_INFO(LC_PROTOCOL, pMsg);
                KNI_ThrowNew(midpIOException, EXCEPTION_MSG(pMsg));
            }
//        }
    } else {  /* Reinvocation after unblocking the thread */
        context = info->pResult;

        if ((bt_handle_t)info->descriptor != handle) {
            REPORT_CRIT2(LC_PROTOCOL,
                "btl2cap::connect handles mismatched %d != %d\n",
                 handle, (bt_handle_t)info->descriptor);
        }

        status = bt_l2cap_connect_finish(handle, &imtu, &omtu, context);

        if (status == BT_RESULT_SUCCESS) {
            // IMPL_NOTE: add bluetooth activity indicator
//            DEC_BT_INDICATOR;
            RegisterL2CAPConnection0(addr, handle);
        } else if (status == BT_RESULT_WOULDBLOCK) {
            midp_thread_wait(NETWORK_WRITE_SIGNAL, (int)handle, context);
        } else  {
            char* pError;

            KNI_SetIntField(thisHandle, connHandleID, (jint)BT_INVALID_HANDLE);

            // IMPL_NOTE: add bluetooth activity indicator
//            DEC_BT_INDICATOR;

            // IMPL_NOTE: add resource counting
/*
            if (midpDecResourceCount(RSC_TYPE_BT_CLI, 1) == 0) {
                REPORT_INFO(LC_PROTOCOL, "Resource limit update error");
            }
*/
            bt_l2cap_get_error(handle, &pError);
            midp_snprintf(gKNIBuffer, KNI_BUFFER_SIZE,
                    "Error in btl2cap::connect (%s)", pError);
            REPORT_INFO(LC_PROTOCOL, gKNIBuffer);
            KNI_ThrowNew(midpConnectionNotFoundException,
                EXCEPTION_MSG(gKNIBuffer));
        }
    }

    mtus = (imtu << 16) & 0xFFFF0000;
    mtus |= omtu & 0xFFFF;

    KNI_EndHandles();
    KNI_ReturnInt(mtus);
}

/**
 * Closes client connection.
 *
 * Note: the method gets native connection handle directly from
 * <code>handle<code> field of <code>L2CAPConnectionImpl</code> object.
 *
 * @throws IOException if any I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_close0(void) {
    bt_handle_t handle;

    REPORT_INFO(LC_PROTOCOL, "btl2cap::close");

    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    handle = (bt_handle_t)KNI_GetIntField(thisHandle, connHandleID);

    if (handle != BT_INVALID_HANDLE) {
        if (bt_l2cap_close(handle) == BT_RESULT_FAILURE) {
            REPORT_ERROR(LC_PROTOCOL,
                "L2CAP connection closing failed in btl2cap::close");
            KNI_ThrowNew(midpIOException,
                EXCEPTION_MSG("L2CAP connection closing failed"));
        } else {
            // IMPL_NOTE: add resource counting
/*
            if (midpDecResourceCount(RSC_TYPE_BT_CLI, 1) == 0) {
                REPORT_INFO(LC_PROTOCOL, "Resource limit update error");
            }
*/
        }
        KNI_SetIntField(thisHandle, connHandleID, (jint)BT_INVALID_HANDLE);
        UnregisterL2CAPConnection(thisHandle);
    }

    REPORT_INFO(LC_PROTOCOL, "btl2cap::close done!");

    KNI_EndHandles();
    KNI_ReturnVoid();
}

/**
 * Sends the specified data via Bluetooth stack.
 *
 * Note: the method gets native connection handle directly from
 * <code>handle<code> field of <code>L2CAPConnectionImpl</code> object.
 *
 * @param buf the data to send
 * @param off the offset into the data buffer
 * @param len the length of the data in the buffer
 * @return total number of send bytes,
 *         or <code>0</code> if empty pcaket has been send
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_send0(void) {
    int length, offset;
    bt_handle_t handle;
    int bytesWritten = 0;
    int status = BT_RESULT_FAILURE;
    void *context = NULL;
    MidpReentryData* info;

    offset = (int)KNI_GetParameterAsInt(2);
    length = (int)KNI_GetParameterAsInt(3);

    KNI_StartHandles(2);
    KNI_DeclareHandle(arrayHandle);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    handle = (bt_handle_t)KNI_GetIntField(thisHandle, connHandleID);
    KNI_GetParameterAsObject(1, arrayHandle);

    REPORT_INFO3(LC_PROTOCOL,
        "btl2cap::send off=%d len=%d handle=%d\n", offset, length, handle);

    info = (MidpReentryData*)SNI_GetReentryData(NULL);

    // IMPL_NOTE: add bluetooth activity indicator
//    START_BT_INDICATOR;

    if (info == NULL) {   /* First invocation */
        if (BT_INVALID_HANDLE == handle) {
            KNI_ThrowNew(midpIOException,
                EXCEPTION_MSG("Invalid handle during btl2cap::send"));
        } else {
            // IMPL_NOTE: add bluetooth activity indicator
//            INC_BT_INDICATOR;
            SNI_BEGIN_RAW_POINTERS;
            status = bt_l2cap_send_start(handle,
                           (char*)&(JavaByteArray(arrayHandle)[offset]),
                           length, &bytesWritten, &context);
            SNI_END_RAW_POINTERS;
        }
    } else { /* Reinvocation after unblocking the thread */
        if (BT_INVALID_HANDLE == handle) {
            /* closed by another thread */
            KNI_ThrowNew(midpInterruptedIOException, EXCEPTION_MSG(
                         "Interrupted IO error during btl2cap::send"));
            // IMPL_NOTE: add bluetooth activity indicator
//            DEC_BT_INDICATOR;
        } else {
            if ((bt_handle_t)info->descriptor != handle) {
                REPORT_CRIT2(LC_PROTOCOL,
                    "btl2cap::send handles mismatched %d != %d\n",
                    handle, (bt_handle_t)info->descriptor);
            }
            context = info->pResult;
            SNI_BEGIN_RAW_POINTERS;
            status = bt_l2cap_send_finish(handle,
                       (char*)&(JavaByteArray(arrayHandle)[offset]),
                       length, &bytesWritten, context);
            SNI_END_RAW_POINTERS;
        }
    }

   if (BT_INVALID_HANDLE != handle) {
        if (status == BT_RESULT_SUCCESS) {
            // IMPL_NOTE: add bluetooth activity indicator
//            DEC_BT_INDICATOR;
        } else {
            char* pError;
            bt_l2cap_get_error(handle, &pError);
            REPORT_INFO1(LC_PROTOCOL, "btl2cap::send (%s)\n", pError);

            if (status == BT_RESULT_WOULDBLOCK) {
                midp_thread_wait(NETWORK_WRITE_SIGNAL, (int)handle, context);
            } else if (status == BT_RESULT_INTERRUPTED) {
                char* pError;
                bt_l2cap_get_error(handle, &pError);
                midp_snprintf(gKNIBuffer, KNI_BUFFER_SIZE,
                    "Interrupted IO during btl2cap::send (%s)", pError);
                KNI_ThrowNew(midpInterruptedIOException,
                    EXCEPTION_MSG(gKNIBuffer));

                // IMPL_NOTE: add bluetooth activity indicator
//                DEC_BT_INDICATOR;
            } else {
                char* pError;
                bt_l2cap_get_error(handle, &pError);
                midp_snprintf(gKNIBuffer, KNI_BUFFER_SIZE,
                    "IO error during btl2cap::send (%s)", pError);
                KNI_ThrowNew(midpIOException, EXCEPTION_MSG(gKNIBuffer));

                // IMPL_NOTE: add bluetooth activity indicator
//                DEC_BT_INDICATOR;
            }
        }
    }

    REPORT_INFO1(LC_PROTOCOL, "btl2cap::send bytes=%d\n", bytesWritten);

    KNI_EndHandles();
    KNI_ReturnInt((jint)bytesWritten);
}


/**
 * Reads data from a packet received via Bluetooth stack.
 *
 * Note: the method gets native connection handle directly from
 * <code>handle<code> field of <code>L2CAPConnectionImpl</code> object.
 *
 * @param buf the buffer to read to
 * @param offset he start offset in array <code>buf</code>
 *               at which the data to be written
 * @param size the maximum number of bytes to read,
 *             the rest of the packet is discarded.
 * @return total number of bytes read into the buffer or
 *             <code>0</code> if a zero length packet is received
 * @throws IOException if an I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_receive0(void) {
    int length, offset;
    bt_handle_t handle;
    int bytesRead = -1;
    int status = BT_RESULT_FAILURE;
    void* context = NULL;
    MidpReentryData* info;

    offset = (int)KNI_GetParameterAsInt(2);
    length = (int)KNI_GetParameterAsInt(3);

    KNI_StartHandles(2);
    KNI_DeclareHandle(arrayHandle);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    handle = (bt_handle_t)KNI_GetIntField(thisHandle, connHandleID);
    KNI_GetParameterAsObject(1, arrayHandle);

    REPORT_INFO3(LC_PROTOCOL,
        "btl2cap::receive off=%d len=%d handle=%d\n", offset, length, handle);

    info = (MidpReentryData*)SNI_GetReentryData(NULL);

    // IMPL_NOTE: add bluetooth activity indicator
//    START_BT_INDICATOR;

    if (info == NULL) {   /* First invocation */
        if (BT_INVALID_HANDLE == handle) {
            KNI_ThrowNew(midpIOException, EXCEPTION_MSG(
                "Invalid handle during btl2cap::receive"));
        } else {
            // IMPL_NOTE: add bluetooth activity indicator
//            INC_BT_INDICATOR;

            SNI_BEGIN_RAW_POINTERS;
            status = bt_l2cap_receive_start(handle,
                (char*)&(JavaByteArray(arrayHandle)[offset]),
                length, &bytesRead, &context);
            SNI_END_RAW_POINTERS;
        }
    } else {  /* Reinvocation after unblocking the thread */
        if (BT_INVALID_HANDLE == handle) {
            /* closed by another thread */
            KNI_ThrowNew(midpInterruptedIOException, EXCEPTION_MSG(
                         "Interrupted IO error during btl2cap::receive"));

            // IMPL_NOTE: add bluetooth activity indicator
//            DEC_BT_INDICATOR;
        } else {
            if ((bt_handle_t)info->descriptor != handle) {
                REPORT_CRIT2(LC_PROTOCOL,
                    "btl2cap::receive handles mismatched %d != %d\n",
                    handle, (bt_handle_t)info->descriptor);
            }
            context = info->pResult;
            SNI_BEGIN_RAW_POINTERS;
            status = bt_l2cap_receive_finish(handle,
                (char*)&(JavaByteArray(arrayHandle)[offset]),
                length, &bytesRead, context);
            SNI_END_RAW_POINTERS;
        }
    }

    REPORT_INFO1(LC_PROTOCOL, "btl2cap::receive bytes=%d\n", bytesRead);

    if (BT_INVALID_HANDLE != handle) {
        if (status == BT_RESULT_SUCCESS) {
            // IMPL_NOTE: add bluetooth activity indicator
//            DEC_BT_INDICATOR;
        } else {
            char* pError;
            bt_l2cap_get_error(handle, &pError);
            REPORT_INFO1(LC_PROTOCOL, "btl2cap::receive (%s)\n", pError);

            if (status == BT_RESULT_WOULDBLOCK) {
                midp_thread_wait(NETWORK_READ_SIGNAL, (int)handle, context);
            } else if (status == BT_RESULT_INTERRUPTED) {
                char* pError;
                bt_l2cap_get_error(handle, &pError);
                midp_snprintf(gKNIBuffer, KNI_BUFFER_SIZE,
                        "Interrupted IO error during btl2cap::receive (%s)",
                        pError);
                KNI_ThrowNew(midpInterruptedIOException,
                    EXCEPTION_MSG(gKNIBuffer));

                // IMPL_NOTE: add bluetooth activity indicator
//                DEC_BT_INDICATOR;
            } else {
                char* pError;
                bt_l2cap_get_error(handle, &pError);
                midp_snprintf(gKNIBuffer, KNI_BUFFER_SIZE,
                    "Unknown error during btl2cap::receive (%s)", pError);
                KNI_ThrowNew(midpIOException, EXCEPTION_MSG(gKNIBuffer));

                // IMPL_NOTE: add bluetooth activity indicator
//                DEC_BT_INDICATOR;
            }
        }
    }

    // IMPL_NOTE: add bluetooth activity indicator
//    STOP_BT_INDICATOR;

    KNI_EndHandles();
    KNI_ReturnInt((jint)bytesRead);
}


/**
 * Checks if there is data that can be received without blocking.
 *
 * Note: the method gets native connection handle directly from
 * <code>handle<code> field of <code>L2CAPConnectionImpl</code> object.
 *
 * @return <code>true</code> if the data found, <code>false</code> otherwise
 * @throws IOException if any I/O error occurs
 */
KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_io_j2me_btl2cap_L2CAPConnectionImpl_ready0(void) {
    int res;
    bt_handle_t handle;
    bt_bool_t ready;

    REPORT_INFO(LC_PROTOCOL, "btl2cap::ready");

    KNI_StartHandles(1);
    KNI_DeclareHandle(thisHandle);
    KNI_GetThisPointer(thisHandle);
    handle = (bt_handle_t)KNI_GetIntField(thisHandle, connHandleID);

    switch (bt_l2cap_get_ready(handle, &ready)) {
    case BT_RESULT_SUCCESS:
        res = (ready == BT_BOOL_TRUE) ? KNI_TRUE : KNI_FALSE;
        REPORT_INFO(LC_PROTOCOL, "btl2cap::ready done!");
        break;

    case BT_RESULT_FAILURE:
        res = KNI_FALSE;
        REPORT_ERROR(LC_PROTOCOL,
            "L2CAP connection state check failed in btl2cap::ready");
        KNI_ThrowNew(midpIOException,
            EXCEPTION_MSG("L2CAP connection ready check failure"));
        break;

    default: /* illegal argument */
        REPORT_CRIT(LC_PROTOCOL, "Internal error in btl2cap::ready");
        res = KNI_FALSE;
    }

    KNI_EndHandles();
    KNI_ReturnBoolean(res);
}
