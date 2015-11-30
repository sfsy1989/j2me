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

#ifndef _BT_L2CAP_CONNECTION_H_
#define _BT_L2CAP_CONNECTION_H_

#include <btCommon.h>

/**
 * @defgroup jsr82protocol JSR82 Protocol Porting Interface
 * @ingroup jsr82
 * @brief JSR82 protocol porting interface.
 *
 * Protocol porting interface functions for use with L2CAP and RFCOMM protocols,
 * and also with OBEX/IrDA. Most of these functions use connection handles,
 * which are easily mapped to BSD network sockets.
 */

/**
 * @file
 * @defgroup jsr82l2cap L2CAP Protocol Porting Interface
 * @ingroup jsr82protocol
 * @brief #include <btL2CAPConnection.h>
 * @{
 */


/**
 * @def BT_L2CAP_INVALID_PSM
 * Defines invalid protocol/service multiplexer (PSM) value.
 *
 * Note: Valid PSM range: 0x0001-0x0019 (0x1001-0xFFFF dynamically assigned,
 * 0x0019-0x0100 reserved for future use).
 */
#define BT_L2CAP_INVALID_PSM  -1

/**
 * @def BT_L2CAP_MIN_MTU
 * Minimum MTU size according to Bluetooth specification.
 */
#define BT_L2CAP_MIN_MTU 48

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
bt_result_t bt_l2cap_close(bt_handle_t handle);

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
bt_result_t bt_l2cap_get_acl_handle(bt_handle_t handle,
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
/*OPTIONAL*/ bt_result_t bt_l2cap_get_error(bt_handle_t handle,
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
 * @param imtu receive MTU or <code>-1</code> if not specified
 * @param omtu transmit MTU or <code>-1</code> if not specified
 * @param auth   <code>BT_BOOL_TRUE</code> if authication is required
 * @param authz  <code>BT_BOOL_TRUE</code> if authorization is required
 * @param enc    <code>BT_BOOL_TRUE</code> if required to be encrypted
 * @param master <code>BT_BOOL_TRUE</code> if required
 *                                         to be a connection's master
 * @param pHandle pointer to connection handle variable,
 *               new connection handle returned in result.
 * @param pPsm pointer to variable, where reserved PSM is returned in.
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_l2cap_create_server(int imtu, int omtu, bt_bool_t auth,
    bt_bool_t authz, bt_bool_t enc, bt_bool_t master, 
    /*OUT*/ bt_handle_t* pHandle, /*OUT*/ int* pPsm);

/**
 * Puts server connection to listening mode.
 *
 * @param handle server connection handle
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_l2cap_listen(bt_handle_t handle);

/**
 * Accepts pending incoming connection if any.
 *
 * @param handle server connection handle
 * @param pPeerHandle pointer to peer handle to store new connection handle
 *             to work with accepted incoming client connection
 * @param pPeerAddr bluetooth address variable to store
 *                  the address of accepted client,
 *                  if <code>NULL</code> the value is not returned
 * @param pImtu pointer to variable to store negotiated recieve MTU
 * @param pOmtu pointer to variable to store negotiated transmit MTU
 * @param pContext pointer to context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there is no pending
 *             incoming connection
 */
bt_result_t bt_l2cap_accept_start(bt_handle_t handle,
    /*OUT*/ bt_handle_t* pPeerHandle, /*OUT*/ bt_bdaddr_t pPeerAddr,
    /*OUT*/ int* pImtu, /*OUT*/ int* pOmtu, /*OUT*/ void** pContext);

/**
 * Finishs accepting pending incoming connection.
 *
 * @param handle server connection handle
 * @param pPeerHandle pointer to peer handle to store new connection handle
 *             to work with accepted incoming client connection
 * @param pPeerAddr bluetooth address variable to store
 *                  the address of accepted client,
 *                  if <code>NULL</code> the value is not returned
 * @param pImtu pointer to variable to store negotiated recieve MTU
 * @param pOmtu pointer to variable to store negotiated transmit MTU
 * @param context the context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there is no pending
 *             incoming connection
 */
bt_result_t bt_l2cap_accept_finish(bt_handle_t handle,
    /*OUT*/ bt_handle_t* pPeerHandle, /*OUT*/ bt_bdaddr_t pPeerAddr,
    /*OUT*/ int* pImtu, /*OUT*/ int* pOmtu, void* context);


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
 * @param imtu receive MTU or <code>-1</code> if not specified
 * @param omtu transmit MTU or <code>-1</code> if not specified
 * @param auth   <code>BT_BOOL_TRUE</code> if authication is required
 * @param enc    <code>BT_BOOL_TRUE</code> if required to be encrypted
 * @param master <code>BT_BOOL_TRUE</code> if required
 *                   to be a connection's master
 * @param pHandle pointer to connection handle variable,
 *               new connection handle returned in result.
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_l2cap_create_client(int imtu, int omtu, bt_bool_t auth,
    bt_bool_t enc, bt_bool_t master, /*OUT*/ bt_handle_t* pHandle);


/**
 * Initiates connection establishment with the Bluetooth device.
 *
 * @param handle connection handle
 * @param addr bluetooth address of device to connect to
 * @param psm PSM port to connect to
 * @param pImtu pointer to variable to store negotiated recieve MTU
 * @param pOmtu pointer to variable to store negotiated transmit MTU
 * @param pContext pointer to context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULBLOCK</code> if connection
 *             establishment is in progress;
 */
bt_result_t bt_l2cap_connect_start(bt_handle_t handle, const bt_bdaddr_t addr,
    int psm, /*OUT*/ int* pImtu, /*OUT*/ int* pOmtu, /*OUT*/ void **pContext);

/**
 * Finishs connection establishment with the Bluetooth device.
 *
 * @param handle connection handle
 * @param pImtu pointer to variable to store negotiated recieve MTU
 * @param pOmtu pointer to variable to store negotiated transmit MTU
 * @param context the context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if connection
 *             establishment is still in progress
 */
bt_result_t bt_l2cap_connect_finish(bt_handle_t handle,
    /*OUT*/ int* pImtu, /*OUT*/ int* pOmtu, void* context);

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
bt_result_t  bt_l2cap_send_start(bt_handle_t handle, char *pData, int len,
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
bt_result_t bt_l2cap_send_finish(bt_handle_t handle, char *pData, int len,
        /*OUT*/ int *pBytesSent, void *context);

/**
 * Receives data via connection.
 *
 * Note: if size of the buffer is less than size of the received packet,
 * the rest of the packet is discarded.
 *
 * @param handle connection handle
 * @param pData pointer to data buffer
 * @param len length of the buffer
 * @param pBytesReceived number of bytes that were received,
 *             <code>0</code> indicates emtpy packet
 * @param pContext pointer to context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there are no data available
 *             at this moment
 */
bt_result_t bt_l2cap_receive_start(bt_handle_t handle, /*OUT*/ char *pData,
        int len, /*OUT*/ int *pBytesReceived, /*OUT*/ void **pContext);

/**
 * Finishs receiving data via connection.
 *
 * Note: if size of the buffer is less than size of the received packet,
 * the rest of the packet is discarded.
 *
 * @param handle connection handle
 * @param pData pointer to data buffer
 * @param len length of the buffer
 * @param pBytesReceived number of bytes that were received,
 *             <code>0</code> indicates empty packet
 * @param context the context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there are no data available
 *             at this moment
 */
bt_result_t bt_l2cap_receive_finish(bt_handle_t handle, /*OUT*/ char *pData,
        int len, /*OUT*/ int *pBytesReceived, void *context);

/**
 * Determines whether there are data available for reading.
 *
 * @param handle connection handle
 * @param pReady pointer to variable result is stored in
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured
 */
bt_result_t bt_l2cap_get_ready(bt_handle_t handle, /*OUT*/ bt_bool_t* pReady);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BT_L2CAP_CONNECTION_H_ */
