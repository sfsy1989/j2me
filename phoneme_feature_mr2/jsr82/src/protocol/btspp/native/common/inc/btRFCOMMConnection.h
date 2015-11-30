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

#ifndef _BT_RFCOMM_CONNECTION_H_
#define _BT_RFCOMM_CONNECTION_H_

#include <btCommon.h>

/**
 * @file
 * @defgroup jsr82rfcomm RFCOMM Protocol Porting Interface
 * @ingroup jsr82protocol
 * @brief #include <btRFCOMMConnection.h>
 * @{
 */

/**
 * @def BT_RFCOMM_INVALID_CN
 * Defines invalid channel number (CN) value.
 *
 * Note: Valid CN range: 1-30.
 */
#define BT_RFCOMM_INVALID_CN  -1


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Common functions used for both types of connections.
 */

/**
 * Closes the connection.
 *
 * Determines whether the connection is not closed, if so closes it.
 *
 * @param handle connection handle
 * @return <code>BT_RESULT_SUCCESS</code> on success close,
 *         <code>BT_RESULT_FAILURE</code> if connection is already closed or
 *                         an error occured during close operation
 */
bt_result_t bt_rfcomm_close(bt_handle_t handle);

/**
 * Retrieves default ACL handle for the connection.
 *
 * The asynchronous connection-oriented (ACL) logical transport is used to carry
 * LMP and L2CAP control signalling and best effort asynchronous user data.
 * Every active slave device within a piconet has one ACL
 * logical transport to the piconet master, known as the default ACL.
 *
 * ACL handle is an unsined 16-bit integer. It should be saved in lower digits
 * of the output parameter. In case ACL handle is not available on a target 
 * stack, any 16-bit integer handles that provide bijection to remote devices
 * can be used.
 *
 * @param handle connection handle
 * @param pAclHandle pointer to variable ACL handle is stored in
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured
 */
bt_result_t bt_rfcomm_get_acl_handle(bt_handle_t handle,
    /*OUT*/ int* pAclHandle);

/**
 * Retrieves code and string description of the last occured error.
 *
 * @param handle connection handle
 * @param pErrStr pointer to string pointer initialized with
 *                    result string pointer,
                  if <code>NULL</code> error string is not returned
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if there was no error occurred
 *              during last connection operation.
 */
/*OPTIONAL*/ bt_result_t bt_rfcomm_get_error(bt_handle_t handle,
    /*OUT*/ char** pErrStr);


/*
 * Functions used for server connections
 */

/**
 * Creates a new server connection.
 *
 * The method creates a server connection instance
 * but does not put it in listen mode.
 * Anyway it selects and reserves a free PSM to listen for
 * incoming connections on after the listen method is called.
 *
 * Note: returned connection is put in non-blocking mode.
 *
 * @param auth   <code>BT_BOOL_TRUE</code> if authication is required
 * @param authz  <code>BT_BOOL_TRUE</code> if authorization is required
 * @param enc    <code>BT_BOOL_TRUE</code> if required to be encrypted
 * @param master <code>BT_BOOL_TRUE</code> if required 
 *                                         to be a connection's master
 * @param pHandle pointer to connection handle variable,
 *               new connection handle returned in result.
 * @param pCn pointer to variable, where reserved channel is returned in.
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_rfcomm_create_server(bt_bool_t auth, bt_bool_t authz,
    bt_bool_t enc, bt_bool_t master,
    /*OUT*/ bt_handle_t* pHandle, /*OUT*/ int* pCn);

/**
 * Puts server connection to listening mode.
 *
 * @param handle server connection handle
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_rfcomm_listen(bt_handle_t handle);

/**
 * Accepts pending incoming connection if any.
 *
 * @param handle server connection handle
 * @param pPeerHandle pointer to peer handle to store new connection handle
 *             to work with accepted incoming client connection
 * @param pPeerAddr bluetooth address variable to store
 *                  the address of accepted client,
 *                  if <code>NULL</code> the value is not returned
 * @param pContext pointer to context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there is no pending
 *             incoming connection
 */
bt_result_t bt_rfcomm_accept_start(bt_handle_t handle,
    /*OUT*/ bt_handle_t* pPeerHandle, /*OUT*/ bt_bdaddr_t pPeerAddr,
    /*OUT*/ void** pContext);

/**
 * Finishs accepting pending incoming connection.
 *
 * @param handle server connection handle
 * @param pPeerHandle pointer to peer handle to store new connection handle
 *             to work with accepted incoming client connection
 * @param pPeerAddr bluetooth address variable to store
 *                  the address of accepted client,
 *                  if <code>NULL</code> the value is not returned
 * @param context the context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there is no pending
 *             incoming connection
 */
bt_result_t bt_rfcomm_accept_finish(bt_handle_t handle,
    /*OUT*/ bt_handle_t* pPeerHandle, /*OUT*/ bt_bdaddr_t pPeerAddr,
    void* context);


/*
 * Functions used for client connections
 */


/**
 * Creates a new client connection.
 *
 * The method does not establishs real bluetooth connection
 * just creates a client connection instance.
 *
 * Note: returned connection is put in non-blocking mode.
 *
 * @param auth   <code>BT_BOOL_TRUE</code> if authication is required
 * @param enc    <code>BT_BOOL_TRUE</code> if required to be encrypted
 * @param master <code>BT_BOOL_TRUE</code> if required 
 *                   to be a connection's master
 * @param pHandle pointer to connection handle variable,
 *               new connection handle returned in result.
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_rfcomm_create_client(bt_bool_t auth, bt_bool_t enc,
    bt_bool_t master, /*OUT*/ bt_handle_t* pHandle);

/**
 * Initiates connection establishment with the Bluetooth device.
 *
 * @param handle connection handle
 * @param addr bluetooth address of device to connect to
 * @param cn channel number to connect to
 * @param pContext pointer to context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_INPROGESS</code> if connection
 *             establishment is in progress;
 *         it's neccessary to call <code>connect_complete</code>
 *         to complete connection establishment
 */
bt_result_t bt_rfcomm_connect_start(bt_handle_t handle,
    const bt_bdaddr_t addr, int cn, /*OUT*/ void** pContext);

/**
 * Finishs connection establishment with the Bluetooth device.
 *
 * Note: it's neccessary to call this function if <code>connect</code>
 * function returns <code>BT_RESULT_INPROGESS</code>.
 *
 * @param handle connection handle
 * @param context the context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_INPROGESS</code> if connection
 *             establishment is still in progress
 */
bt_result_t bt_rfcomm_connect_finish(bt_handle_t handle, void* context);

/**
 * Sends data via connection.
 *
 * Note: Since the connection is non-blocking,
 * the method can return after sending just a part of the data.
 *
 * @param handle connection handle
 * @param pData pointer to data buffer
 * @param len length of the data
 * @param pBytesSent number of bytes that were really sent
 * @param pContext pointer to context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured,
 *         <code>BT_RESULT_WOULDBLOCK</code> if data can not be sent
 *             at this moment
 */
bt_result_t  bt_rfcomm_send_start(bt_handle_t handle, char *pData, int len,
        /*OUT*/ int *pBytesSent, /*OUT*/ void **pContext);

/**
 * Finishs sending data via connection.
 *
 * Note: Since the connection is non-blocking,
 * the method can return after sending just a part of the data.
 *
 * @param handle connection handle
 * @param pData pointer to data buffer
 * @param len length of the data
 * @param pBytesSent number of bytes that were really sent
 * @param context the context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured,
 *         <code>BT_RESULT_WOULDBLOCK</code> if data can not be sent
 *             at this moment
 */
bt_result_t bt_rfcomm_send_finish(bt_handle_t handle, char *pData, int len,
        /*OUT*/ int *pBytesSent, void *context);

/**
 * Receives data via connection.
 *
 * @param handle connection handle
 * @param pData pointer to data buffer
 * @param len length of the buffer
 * @param pBytesReceived number of bytes that were received,
 *             <code>0</code> indicates end-of-stream
 * @param pContext pointer to context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there are no data available
 *             at this moment
 */
bt_result_t bt_rfcomm_receive_start(bt_handle_t handle, /*OUT*/ char *pData,
        int len, /*OUT*/ int *pBytesReceived, /*OUT*/ void **pContext);

/**
 * Finishs receiving data via connection.
 *
 * @param handle connection handle
 * @param pData pointer to data buffer
 * @param len length of the buffer
 * @param pBytesReceived number of bytes that were received,
 *             <code>0</code> indicates end-of-stream
 * @param context the context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there are no data available
 *             at this moment
 */
bt_result_t bt_rfcomm_receive_finish(bt_handle_t handle, /*OUT*/ char *pData,
        int len, /*OUT*/ int *pBytesReceived, void *context);

/**
 * Returns the number of bytes available to be read from the connection
 * without blocking.
 *
 * @param handle connection handle
 * @param pCount pointer to variable the number of available bytes is stored in
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred
 */
bt_result_t bt_rfcomm_get_available(bt_handle_t handle,
    /*OUT*/ int* pCount);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BT_RFCOMM_CONNECTION_H_ */
