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

#include <btRFCOMMConnection.h>
#include <btL2CAPConnection.h>
#include <emul.h>

bt_result_t create_server(int protocol, int imtu, int omtu, 
        bt_bool_t auth, bt_bool_t authz, bt_bool_t enc, bt_bool_t master, 
        bt_handle_t *pHandle, int *pPort);

bt_result_t create_client(int protocol, 
        int imtu, int omtu, bt_bool_t auth, bt_bool_t enc, 
        bt_bool_t master, bt_handle_t* pHandle);

int available(bt_handle_t handle);

bt_result_t bt_rfcomm_get_error(bt_handle_t handle, char** err_str)
{
    return bt_l2cap_get_error(handle, err_str);
}

bt_result_t bt_rfcomm_get_acl_handle(bt_handle_t handle, int* pAclHandle) 
{
    return bt_l2cap_get_acl_handle(handle, pAclHandle);
}

bt_result_t bt_rfcomm_close(bt_handle_t handle) 
{
    return bt_l2cap_close(handle);
}

bt_result_t bt_rfcomm_create_server( bt_bool_t auth, bt_bool_t authz, 
    bt_bool_t enc, bt_bool_t master, bt_handle_t* pHandle, int* pCn)
{
    return create_server(RFCOMM, 
        -1, -1, auth, authz, enc, master, pHandle, pCn);
}

bt_result_t bt_rfcomm_listen(bt_handle_t handle) 
{
    return bt_l2cap_listen(handle);
}

bt_result_t bt_rfcomm_accept_start(bt_handle_t handle, bt_handle_t* pPeerHandle,
        bt_bdaddr_t pPeerAddr, void **pContext)
{    
    return bt_l2cap_accept_start(handle, pPeerHandle, pPeerAddr, 
            NULL, NULL, pContext);
}

bt_result_t bt_rfcomm_accept_finish(bt_handle_t handle, bt_handle_t* pPeerHandle,
        bt_bdaddr_t pPeerAddr, void *context)
{
     return bt_l2cap_accept_finish(handle, pPeerHandle, pPeerAddr, 
         NULL, NULL, context);
}    

bt_result_t bt_rfcomm_create_client(bt_bool_t auth,
        bt_bool_t enc, bt_bool_t master, bt_handle_t* pHandle)
{
    return create_client(RFCOMM, -1, -1, auth, enc, master, pHandle);
}        

bt_result_t bt_rfcomm_connect_start(bt_handle_t handle, const bt_bdaddr_t addr,
        int cn, void **pContext)
{
    return bt_l2cap_connect_start(handle, addr, cn, NULL, NULL, pContext);
}

bt_result_t bt_rfcomm_connect_finish(bt_handle_t handle, void *context)
{
    return bt_l2cap_connect_finish(handle, NULL, NULL, context);
}

bt_result_t bt_rfcomm_send_start(bt_handle_t handle, char *pData, int len,
        int *pBytesSent, void **pContext)
{
    return bt_l2cap_send_start(handle, pData, len, pBytesSent, pContext);
}    

bt_result_t bt_rfcomm_send_finish(bt_handle_t handle, char *pData, int len,
        int *pBytesSent, void *context)
{
    return bt_l2cap_send_finish(handle, pData, len, pBytesSent, context); 
}

bt_result_t bt_rfcomm_receive_start(bt_handle_t handle, char *pData, int len,
        int *pBytesReceived, void **pContext)
{
    return bt_l2cap_receive_start(handle, pData, len, pBytesReceived, pContext);
}

bt_result_t bt_rfcomm_receive_finish(bt_handle_t handle, char *pData, int len,
        int *pBytesReceived, void *context)
{
    return bt_l2cap_receive_finish(handle, pData, len, 
            pBytesReceived, context);
}

bt_result_t bt_rfcomm_get_available(bt_handle_t handle,
        int* pCount) 
{
    *pCount = available(handle);
    return BT_RESULT_SUCCESS;
}        
