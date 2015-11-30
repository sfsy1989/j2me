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

#include <btL2CAPConnection.h>
#include <emul.h>
#include <btNotify.h>
#include <btBCC.h>
#include <btPush.h>

/** Appends ServiceConnectionData image to emulation request. */
#define APPEND_ServiceConnectionData(/*int*/ protocol, /*int*/imtu, \
     /*int*/ omtu, /*bt_bool_t*/ auth, /*bt_bool_t*/ authz, \
     /*bt_bool_t*/ enc, /*bt_bool_t*/ master, /*int*/ psm) \
    { \
        int _misc; \
        LOG("putServiceConnectionData"); \
        APPEND_INT(-1 /* socket port, defined later in java */); \
        APPEND_INT(psm); \
        APPEND_INT(imtu); \
        APPEND_INT(omtu); \
        _misc = protocol; \
        _misc |= master? 4 : 0; \
        _misc |= enc? 8 : 0; \
        _misc |= authz? 32 : 0; \
        _misc |= auth? 16 : 0; \
        APPEND_BYTE(_misc); \
    }

#define APPEND_CONN_REQ(handle, req) \
    APPEND_BYTE(CONNECTIONS); \
    APPEND_BYTE(handle); \
    APPEND_BYTE(req);

bt_result_t bt_l2cap_get_error(bt_handle_t handle, char** err_str) {
    static char* empty = "";
    (void)handle;
    *err_str = empty;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_l2cap_get_acl_handle(bt_handle_t handle, int* pAclHandle) {
    *pAclHandle = 0;
    memcpy(pAclHandle,
            &emul_data.handled_info[(int)handle].conn.peer_addr, 2);
    return *pAclHandle == 0 ? BT_RESULT_FAILURE : BT_RESULT_SUCCESS;
}

void resetConnInfo(bt_handle_t handle) {
    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;
    if (info->in) {
        midpFree(info->in);
    }
    if (info->out) {
        midpFree(info->out);
    }
    memset(info, 0, sizeof(connection_info_t));
}

/**
 * Closes the connection.
 * Determines whether the connection is not closed, if so closes it.
 *
 * @param handle connection handle
 * @return <code>BT_RESULT_SUCCESS</code> on success close,
 *         <code>BT_RESULT_FAILURE</code> if connection is already closed or
 *                         an error occurred during close operation
 */
bt_result_t bt_l2cap_close(bt_handle_t handle) {
    LOG1("bt_l2cap_close(%d)", (int)handle);

    START_REQUEST
        APPEND_CONN_REQ(handle, NOTIF_CLOSE);
    END_REQUEST

    resetConnInfo(handle);

    // Notification is just ignored if it is not waited
    bt_notify_protocol_event(BT_EVENT_ACCEPT_COMPLETE, handle,
       BT_RESULT_FAILURE);
    bt_notify_protocol_event(BT_EVENT_RECEIVE_COMPLETE, handle,
       BT_RESULT_FAILURE);
    bt_notify_protocol_event(BT_EVENT_SEND_COMPLETE, handle,
       BT_RESULT_FAILURE);
    bt_notify_protocol_event(BT_EVENT_CONNECT_COMPLETE, handle,
       BT_RESULT_FAILURE);

    return BT_RESULT_SUCCESS;
}

static void saveAuthEnc(bt_handle_t handle, bt_bool_t auth, bt_bool_t enc) {
    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;

    if (auth) {
        info->flags |= AUTHENTICATE;
    }
    if (enc) {
        info->flags |= ENCRYPT;
    }
}

static void checkAuthEnc(bt_handle_t handle) {
    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;
    bt_string_t fakePin = NULL;
    bt_bool_t fakeRetval;

    if (info->flags & AUTHENTICATE) {
        bt_bcc_bond(info->peer_addr, fakePin, &fakeRetval);
    }
    if (info->flags & ENCRYPT) {
        bt_bcc_set_encryption(info->peer_addr, BT_BOOL_TRUE, &fakeRetval);
    }
}

int getPort(int handle, int protocol) {
    int port;

    if (L2CAP == protocol) {
        /*
        * Generate next free psm:
        *    - first call is always made to create SDPServer, it
        *      receives fixed value 1.
        *    - valid PSM for user-defined services are in the
        *      range (0x1001..0xFFFF), with odd low byte and even high 
        *      byte.
        *
        * There is no overflow check here for the highest value
        * is not reached in usual emulation mode scenarios.
        * SDP_REQ_EXPECTED state means requesting psm for SDP server,
        * which is 1.
        */
        if (state & SDP_REQ_EXPECTED) {
            state ^= SDP_REQ_EXPECTED;
            port = 1;
        } else {
            port = 0x1001 + 2 * handle;
        }
    } else {
        port = handle % 30 + 1;
    }

    return port;
}

bt_result_t create_server(int protocol, int imtu, int omtu,
        bt_bool_t auth, bt_bool_t authz, bt_bool_t enc, bt_bool_t master,
        bt_handle_t *pHandle, int *pPort) {

    LOG("create_server()");

    *pHandle = getNextHandle(protocol);

    saveAuthEnc(*pHandle, auth, enc);

    if (BT_INVALID_HANDLE == *pHandle) {
        return BT_RESULT_FAILURE;
    }

    *pPort = getPort((int)*pHandle, protocol);

    START_REQUEST
        APPEND_BYTE(CREATE_NOTIF);
        APPEND_BYTE(*pHandle);

        APPEND_CONN_REQ(*pHandle, NOTIF_INIT);
        APPEND_ServiceConnectionData(protocol, imtu, omtu, auth,
            authz, enc, master, *pPort);
    END_REQUEST

    return BT_RESULT_SUCCESS;
}

bt_result_t bt_l2cap_create_server(int imtu, int omtu, bt_bool_t auth,
        bt_bool_t authz, bt_bool_t enc, bt_bool_t master,
        bt_handle_t* pHandle, int* pPsm) {

    LOG("bt_l2cap_create_server()");
    return create_server(L2CAP,
        imtu, omtu,auth, authz, enc, master, pHandle, pPsm);
}

/**
 * Queues acception request to emulation. Emulation makes no difference
 * between listen and accept operations thus it can be called from
 * either listen() or accept_satrt(). This logic cannot be simply moved
 * to accept_start because Push requires the following scenario:
 * listen() - notifyAccepted() - non-blocking-accept_start().
 */
void request_accept(bt_handle_t handle) {
    START_REQUEST
        APPEND_CONN_REQ(handle, NOTIF_ACCEPT);
    END_REQUEST

    emul_data.handled_info[(int)handle].accept.status = BT_RESULT_WOULDBLOCK;
}

/**
 * Puts server connection to listening mode.
 *
 * @param handle server connection handle
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_l2cap_listen(bt_handle_t handle) {

    LOG("bt_l2cap_listen()");

    request_accept(handle);
    // to avoid re-requesting in immediately following accept_start()
    emul_data.handled_info[(int)handle].accept.flags |= SKIP_REQUEST;

    return BT_RESULT_SUCCESS;
}

bt_result_t bt_l2cap_accept_start(bt_handle_t handle, bt_handle_t* pPeerHandle,
        bt_bdaddr_t pPeerAddr, int* pImtu, int* pOmtu, void **pContext) {

    accept_info_t *info = &emul_data.handled_info[(int)handle].accept;
    LOG1("bt_l2cap_accept_start(%d)", (int)handle);

    *pContext = NULL;

    if (info->flags & SKIP_REQUEST) {
        info->flags ^= SKIP_REQUEST;
    } else {
        request_accept(handle);
    }

    // A connection may be already accepted if acception was initiated by
    // listen()
    return bt_l2cap_accept_finish(handle, pPeerHandle, pPeerAddr,
        pImtu, pOmtu, *pContext);
}

bt_result_t bt_l2cap_accept_finish(bt_handle_t handle, bt_handle_t* pPeerHandle,
        bt_bdaddr_t pPeerAddr, int* pImtu, int* pOmtu, void *context) {

    accept_info_t *info;
    int status;

    LOG("bt_l2cap_accept_finish()");
    (void)context;

    info = &emul_data.handled_info[(int)handle].accept;
    status = info->status;

    if (status == BT_RESULT_SUCCESS) {
        connection_info_t *connInfo =
            &emul_data.handled_info[info->conn_handle].conn;
        *pPeerHandle = (bt_handle_t) info->conn_handle;

        memcpy(pPeerAddr, connInfo->peer_addr, BT_ADDRESS_SIZE);
        if (pImtu && pOmtu) {
            *pImtu = connInfo->imtu;
            *pOmtu = connInfo->omtu;
        }

        checkAuthEnc(handle);

        // It is a patch for push support. Normally accept notification
        // means that stack is ready to accept without blocking. Push
        // detects incoming connections by those notifications.
        // The same is not applied for the common case for it is not
        // absolutely due to the following emulation feature. Emulation
        // calls notification callback when incoming connection is already
        // accepted.

        if (BT_INVALID_PUSH_HANDLE != bt_push_find_server(handle)) {
            bt_l2cap_listen(handle);
        }
    }

    return status;
}

bt_result_t create_client(int protocol,
        int imtu, int omtu, bt_bool_t auth, bt_bool_t enc,
        bt_bool_t master, bt_handle_t *pHandle) {

    LOG("create_client()");

    *pHandle = getNextHandle(protocol);
    if (BT_INVALID_HANDLE == *pHandle) {
         return BT_RESULT_FAILURE;
    }

    saveAuthEnc(*pHandle, auth, enc);

    START_REQUEST
        APPEND_BYTE(CREATE_CONN);
        APPEND_BYTE(*pHandle);

        APPEND_CONN_REQ(*pHandle, CONN_INIT);
        APPEND_ServiceConnectionData(protocol, imtu, omtu, auth,
            BT_BOOL_FALSE, enc, master, -1);
    END_REQUEST

    return BT_RESULT_SUCCESS;
}

bt_result_t bt_l2cap_create_client(int imtu, int omtu, bt_bool_t auth,
        bt_bool_t enc, bt_bool_t master, bt_handle_t* pHandle) {

    LOG("bt_l2cap_create_client()");

    return create_client(L2CAP,
        imtu, omtu, auth, enc, master, pHandle);
}

bt_result_t bt_l2cap_connect_start(bt_handle_t handle, const bt_bdaddr_t addr,
        int psm, int* pImtu, int* pOmtu, void **pContext) {

    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;

    LOG1("bt_l2cap_connect_start(), port %d", psm);
    (void)pImtu;
    (void)pOmtu;
    *pContext = NULL;

    memcpy(info->peer_addr, addr, BT_ADDRESS_SIZE);

    START_REQUEST
        APPEND_CONN_REQ(handle, CONN_OPEN);
        APPEND_BYTES(addr, BT_ADDRESS_SIZE);
        APPEND_INT(psm);
    END_REQUEST

    info->connect_status = BT_RESULT_WOULDBLOCK;
    return BT_RESULT_WOULDBLOCK;
}

bt_result_t bt_l2cap_connect_finish(bt_handle_t handle,
        int* pImtu, int* pOmtu, void *context) {

    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;
    LOG1("bt_l2cap_connect_finish(%d)", (int)handle);
    (void)context;

    if (info->connect_status == BT_RESULT_SUCCESS) {
        if (pImtu) {
            *pImtu = info->imtu;
            *pOmtu = info->omtu;
        }

        checkAuthEnc(handle);
    }

    return info->connect_status;
}

bt_result_t bt_l2cap_send_start(bt_handle_t handle, char *pData, int len,
        int *pBytesSent, void **pContext) {

    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;
    int status = BT_RESULT_WOULDBLOCK;

    LOG1("bt_l2cap_send_start(%d)", (int)handle);
    (void)pBytesSent;
    *pContext = NULL;

    info->sent = CONN_FAILURE;
    START_REQUEST
        APPEND_CONN_REQ(handle, CONN_SEND);
        APPEND_INT(len);
    END_REQUEST

    APPEND_BUFFER(info->out, info->out_len, pData, len);

    return status;
}

bt_result_t bt_l2cap_send_finish(bt_handle_t handle, char *pData, int len,
        int *pBytesSent, void *context) {
    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;
    int status = BT_RESULT_SUCCESS;

    LOG1("bt_l2cap_send_finish(%d)", (int)handle);

    (void)pData;
    (void)len;
    (void)context;

    if (info->sent == CONN_FAILURE) {
        status = BT_RESULT_FAILURE;
        *pBytesSent = -1;
    } else {
       *pBytesSent = info->sent;
    }

    return status;
}

bt_result_t receive_common(bt_handle_t handle, char *pData, int len,
        int *pBytesReceived, int defaultStatus) {

    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;
    int status = BT_RESULT_SUCCESS;
    
    if (info->receive_status == BT_RESULT_FAILURE) {
        status = BT_RESULT_FAILURE;
        info->receive_status = BT_RESULT_SUCCESS;
    
    } else if (info->flags & ENDOF_INP_REACHED) {
        *pBytesReceived = 0;
    
    } else if (info->in_len > 0) {
        if (!(info->flags & RFCOMM) && info->in_len > len) {
            // in case of L2CAP rest of data should be discarded
            info->in_len = len;
        }

        if (info->in_len <= len) {
            *pBytesReceived = info->in_len;
            memcpy(pData, info->in, info->in_len);
            midpFree(info->in);
            info->in = NULL;
            info->in_len = 0;

        } else {
            int restLen = info->in_len - len;
            *pBytesReceived = len;
            memcpy(pData, info->in, len);
            memmove(info->in, &info->in[len], restLen);
            info->in = midpRealloc(info->in, restLen);
            info->in_len = restLen;
        }

    } else {
        status = defaultStatus;
        *pBytesReceived = info->in_len;
    }

    return status;
}

static void requestReceive(bt_handle_t handle, int len) {
    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;
    LOG("requestReceive()");
    (void)len;

    if (!(info->flags & SKIP_REQUEST)) {
        LOG1("requestReceive() makes request for handle %d", (int)handle);
        START_REQUEST
            APPEND_CONN_REQ(handle, CONN_RECEIVE);
        END_REQUEST

        info->flags |= SKIP_REQUEST;
    }
}

bt_result_t bt_l2cap_receive_start(bt_handle_t handle, char *pData, int len,
        int *pBytesReceived, void **pContext) {

    int status;

    LOG1("bt_l2cap_receive_start(), %d bytes", len);
    *pContext= NULL;

    status = receive_common(handle, pData, len, pBytesReceived, 
        BT_RESULT_WOULDBLOCK);
        
    if (status == BT_RESULT_WOULDBLOCK) {
        requestReceive(handle, len);
    }

    return status;
}

bt_result_t bt_l2cap_receive_finish(bt_handle_t handle, char *pData, int len,
        int *pBytesReceived, void *context)
{
    (void)context;
    LOG1("bt_l2cap_receive_finish(), %d bytes", len);
    return receive_common(handle, pData, len, pBytesReceived,
            BT_RESULT_SUCCESS);
}

int available(bt_handle_t handle) {
    connection_info_t *info = &emul_data.handled_info[(int)handle].conn;

    LOG("available()");

    if (info->in_len == 0) {
        if (!(info->flags & ENDOF_INP_REACHED)) {
            // -1 means receive everything available into input buffer
            requestReceive(handle, -1);
        }
    }

    return info->in_len;
}

/**
 * Determines whether there are data available for reading.
 *
 * @param handle connection handle
 * @param ready pointer to variable result is stored in
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred
 */
bt_result_t bt_l2cap_get_ready(bt_handle_t handle, bt_bool_t* ready) {
    LOG("bt_l2cap_get_ready()");

    *ready = available(handle) ? BT_BOOL_TRUE : BT_BOOL_FALSE;
    return BT_RESULT_SUCCESS;
}
