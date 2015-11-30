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
#include <btMacros.h>

/* sockets */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

/* BT */
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <stdlib.h>
#include <sys/poll.h>

#include <bt_na.h>
#include <btSocket.h>


/** Stores error number of last socket error. */
static int lastError = 0;

/**
 * Helper function that sets connection options.
 *
 * @param fd socket handle
 * @param auth   <code>BT_BOOL_TRUE</code> if authication is required
 * @param authz  <code>BT_BOOL_TRUE</code> if authorization is required
 * @param enc    <code>BT_BOOL_TRUE</code> if required to be encrypted
 * @param master <code>BT_BOOL_TRUE</code> if required
 *                                         to be a connection's master
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
static bt_result_t bt_rfcomm_set_options(int fd, bt_bool_t auth,
        bt_bool_t authz, bt_bool_t enc, bt_bool_t master) {
    /*
     * Note: The code below may not work because this feature
     * ( setsockopt(..., SOL_RFCOMM, RFCOMM_LM, ...) )
     * is supported only in new Linux kernels.
     * Need revisit: about concrete versions but it works in 2.16.11 and newer.
     * But it really does not work in 2.6.8 and older, it returns
     * ENOPROTOOPT (Protocol not available) for RFCOMM protocol.
     */

    int lm, lm_size;

    lastError = 0;

    /* get current link modes */
    lm_size = sizeof(lm);
    if (getsockopt(fd, SOL_RFCOMM, RFCOMM_LM, &lm, &lm_size) == SOCKET_ERROR) {
        lastError = errno;
        return BT_RESULT_FAILURE;
    }

    /* tune link modes */
    if (master == BT_BOOL_TRUE) {
        lm |= RFCOMM_LM_MASTER;
    }
    if (enc == BT_BOOL_TRUE) {
        lm |= RFCOMM_LM_ENCRYPT;
    }
    if (auth == BT_BOOL_TRUE) {
        lm |= RFCOMM_LM_AUTH;
    }
#ifdef RFCOMM_LM_SECURE
    /*
     * Note: 'ifdef' is used in order to be compatible with a particular bluetooth lib
     * that has not such mode support
     */
    if (authz == BT_BOOL_TRUE) {
        lm |= RFCOMM_LM_SECURE;
    }
#endif

    /* apply new link modes */
    if (setsockopt(fd, SOL_RFCOMM, RFCOMM_LM, &lm, lm_size) == SOCKET_ERROR) {
        lastError = errno;
        return BT_RESULT_FAILURE;
    }

    return BT_RESULT_SUCCESS;
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
bt_result_t bt_rfcomm_close(bt_handle_t handle) {
    int fd;

    lastError = 0;
    fd = bt_na_get_fd(handle);

    /* printf("closing fd: %d\n", fd); */
    if (fd != INVALID_SOCKET) {
        bt_na_destroy(handle);
        if (close(fd) == SOCKET_OK) {
            // IMPL_NOTE: this is a temporary workaround, 1 sec delay to let
            // ACL connection terminate; without this, some TCK tests fail
            usleep(1000000);
            return BT_RESULT_SUCCESS;
        } else {
            lastError = errno;
            return BT_RESULT_FAILURE;
        }
    }

    return BT_RESULT_FAILURE;
}

/**
 * Retrieves default ACL handle for the connection.
 *
 * The asynchronous connection-oriented (ACL) logical transport is used to carry
 * LMP and L2CAP control signalling and best effort asynchronous user data.
 * Every active slave device within a piconet has one ACL
 * logical transport to the piconet master, known as the default ACL.
 *
 * @param handle connection handle
 * @param pAclHandle pointer to variable ACL handle is stored in
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occured
 */
bt_result_t bt_rfcomm_get_acl_handle(bt_handle_t handle, int* pAclHandle) {
    struct rfcomm_conninfo cinfo;
    int cinfo_size, fd;

    cinfo_size = sizeof(cinfo);
    memset(&cinfo, 0, cinfo_size);

    *pAclHandle = BT_INVALID_ACL_HANDLE;

    lastError = 0;
    fd = bt_na_get_fd(handle);

    /* get RFCOMM connection info */
    if (getsockopt(fd, SOL_RFCOMM, RFCOMM_CONNINFO,
            &cinfo, &cinfo_size) == SOCKET_ERROR) {
        lastError = errno;
        return BT_RESULT_FAILURE;
    }

    *pAclHandle = cinfo.hci_handle;
    return BT_RESULT_SUCCESS;
}

/**
 * Retrieves code and string description of the last occurred error.
 *
 * @param handle connection handle
 * @param pErrStr pointer to string pointer initialized with
 *                    result string pointer,
                  if <code>NULL</code> error string is not returned
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if there was no error occurred
 *              during last connection operation.
 */
bt_result_t bt_rfcomm_get_error(bt_handle_t handle, char** pErrStr) {

    // IMPL_NOTE: use handle to find error just for the connection
    (void)handle;

    if (lastError != 0) {
        if (pErrStr != NULL) {
            *pErrStr = strerror(lastError);
        }

        return BT_RESULT_SUCCESS;
    } else {
        if (pErrStr != NULL) {
            *pErrStr = NULL;
        }

        return BT_RESULT_FAILURE;
    }
}

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
        /*OUT*/ bt_handle_t* pHandle, /*OUT*/ int* pCn) {

    const char *addr = getenv("JSR82BTADDR");
    int flags, free_cn, status, fd;
    struct sockaddr_rc rca;

    *pHandle = BT_INVALID_HANDLE;
    *pCn      = BT_RFCOMM_INVALID_CN;

    lastError = 0;
    fd = INVALID_SOCKET;

    /* create RFCOMM socket */
    fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (fd == SOCKET_ERROR) {
        lastError = errno;
        return BT_RESULT_FAILURE;
    }

    /* switch on non-blocking mode */
    flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == SOCKET_ERROR ) {
        lastError = errno;
        close(fd);
        return BT_RESULT_FAILURE;
    }

    /* set up address */
    memset(&rca, 0, sizeof(rca));
    rca.rc_family = AF_BLUETOOTH;
    if (addr != NULL) {
        str2ba(addr, &rca.rc_bdaddr);
    } else {
        /* copy address bytes (they are already in Bluetooth order) */
        bacpy(&rca.rc_bdaddr, BDADDR_ANY);
    }

    /* setup link options. */
    status = bt_rfcomm_set_options(fd, auth, authz, enc, master);
    if (status == BT_RESULT_FAILURE) {
        close(fd);
        return BT_RESULT_FAILURE;
    }

    /* bind to first free channel */
/*
    // automatically select free channel
    // Note: this approach doesn't work for a particular implementation

    // let Bluetooth stack to select free channel
    rca.rc_channel = 0;
*/
    /* manually select free channel */
    free_cn = 1;
    while(1) {
        int res;

        rca.rc_channel = htobs(free_cn);
        res = bind(fd, (struct sockaddr*)&rca, sizeof(rca));
        if (res == 0) {
            break;
        }
        if (res == -1) {
            if (errno != EADDRINUSE) {
                lastError = errno;
                close(fd);
                return BT_RESULT_FAILURE;
            }
        }

        free_cn++;

        /* check channel for the maximum value */
        if (free_cn > 30) {
            close(fd);
            return BT_RESULT_FAILURE;
        }
    }

    *pCn    = free_cn;
    *pHandle = bt_na_create(fd);

    /* printf("server socket created: fd=%d, cn=%d\n", fd, free_cn); */

    return BT_RESULT_SUCCESS;
}

/**
 * Puts server connection to listening mode.
 *
 * @param handle server connection handle
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> otherwise
 */
bt_result_t bt_rfcomm_listen(bt_handle_t handle) {
    int fd;

    lastError = 0;
    fd = bt_na_get_fd(handle);

    /*
     * set the length of the queue for pending connections to '1'
     * and force listening for incoming connections.
     */
    if (listen(fd, 1) == SOCKET_ERROR) {
        lastError = errno;
        close(fd);
        return BT_RESULT_FAILURE;
    }

    return BT_RESULT_SUCCESS;
}


/**
 * Accepts pending incoming connection if any.
 *
 * @param handle server connection handle
 * @param pPeerHandle pointer to peer handle to store new connection handle
 *             to work with accepted incoming client connection
 * @param pPeerAddr bluetooth address variable to store
 *                  the address of accepted client,
 *                  if <code>NULL</code> the value is not returned
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there is no pending
 *             incoming connection
 */
static bt_result_t bt_rfcomm_accept_common(bt_handle_t handle, bt_handle_t*
        pPeerHandle, bt_bdaddr_t pPeerAddr) {

    struct sockaddr_rc cl_addr;
    int cl_size, clientfd, fd;

    *pPeerHandle = BT_INVALID_HANDLE;
    memset(pPeerAddr, 0, sizeof(bt_bdaddr_t));

    lastError = 0;
    fd = bt_na_get_fd(handle);

    cl_size = sizeof(cl_addr);
    clientfd = accept(fd, (struct sockaddr*)&cl_addr, &cl_size);
    if (clientfd == INVALID_SOCKET) {
        /*
         * BT_RESULT_FAILURE is returned in the case of an I/O error
         * otherwise BT_RESULT_WOULDBLOCK is returned.
         */
        if (errno != EWOULDBLOCK) {
            lastError = errno;
            return BT_RESULT_FAILURE;
        }

        return BT_RESULT_WOULDBLOCK;
    } else {
        int flags, i;

        /* switch on non-blocking mode for the new client socket */
        flags = fcntl(clientfd, F_GETFL, 0);
        if (fcntl(clientfd, F_SETFL, flags | O_NONBLOCK) == -1) {
            lastError = errno;
            close(clientfd);
            return BT_RESULT_FAILURE;
        }

        *pPeerHandle = bt_na_create(clientfd);

        if (pPeerAddr != NULL) {
            for (i = 0; i < BT_ADDRESS_SIZE; i++) {
                pPeerAddr[i] = cl_addr.rc_bdaddr.b[i];
            }
        }

        return BT_RESULT_SUCCESS;
    }
}

/**
 * See bt_rfcomm_accept_common for definition.
 */
bt_result_t bt_rfcomm_accept_start(bt_handle_t handle,
        bt_handle_t* pPeerHandle, bt_bdaddr_t pPeerAddr, void **pContext) {

    int status;

    status = bt_rfcomm_accept_common(handle, pPeerHandle, pPeerAddr);

    if (status == BT_RESULT_WOULDBLOCK) {
        bt_na_register_for_read(handle);
        *pContext = NULL;
    }

    /* printf("server_accept_start ok: handle=%d\n", handle); */

    return status;
}

/**
 * See bt_rfcomm_accept_common for definition.
 */
bt_result_t bt_rfcomm_accept_finish(bt_handle_t handle, bt_handle_t*
        pPeerHandle, bt_bdaddr_t pPeerAddr, void *context) {

    int status;
    (void)context;

    status = bt_rfcomm_accept_common(handle, pPeerHandle, pPeerAddr);

    if (status == BT_RESULT_WOULDBLOCK) {
        bt_na_register_for_read(handle);
    } else {
        bt_na_unregister_for_read(handle);
    }

    /* printf("server_accept_finish ok: handle=%d ok\n", handle); */

    return status;
}


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
        bt_bool_t master, /*OUT*/ bt_handle_t* pHandle) {

    const char *addr = getenv("JSR82BTADDR");
    int flags, status, fd;
    struct sockaddr_rc rca;

    *pHandle = BT_INVALID_HANDLE;

    lastError = 0;
    fd = INVALID_SOCKET;

    /* create RFCOMM socket */
    fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (fd == INVALID_SOCKET) {
        lastError = errno;
        return BT_RESULT_FAILURE;
    }

    if (addr != NULL) {
        memset(&rca, 0, sizeof(rca));
        rca.rc_family = AF_BLUETOOTH;
        str2ba(addr, &rca.rc_bdaddr);
        rca.rc_channel = 0;
        if (bind(fd, (struct sockaddr*)&rca, sizeof(rca)) == SOCKET_ERROR) {
            lastError = errno;
            close(fd);
            return BT_RESULT_FAILURE;
        }
    }

    // switch on non-blocking mode
    flags = fcntl(fd, F_GETFL, 0);
    status = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (status == SOCKET_ERROR) {
        lastError = errno;
        close(fd);
        return BT_RESULT_FAILURE;
    }
    
    /* setup link options. */
    status = bt_rfcomm_set_options(fd, auth, BT_BOOL_FALSE, enc, master);
    if (status == BT_RESULT_FAILURE) {
        close(fd);
       return BT_RESULT_FAILURE;
    }

    /* printf("create_client ok: fd=%d\n", fd); */

    *pHandle = bt_na_create(fd);
    return BT_RESULT_SUCCESS;
}

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
        const bt_bdaddr_t addr, int cn, /*OUT*/ void** pContext) {

    int i, status, fd;
    struct sockaddr_rc rca;

    *pContext = NULL;

    lastError = 0;
    fd = bt_na_get_fd(handle);

    /* printf("connect_start: handle=%d, addr={%X%X%X%X%X%X}, cn=%x\n",
        handle, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0], cn); */

    /* set up address */
    memset(&rca, 0, sizeof(rca));
    rca.rc_family = AF_BLUETOOTH;
    rca.rc_channel = htobs(cn);
    /* copy address bytes */
    for (i = 0; i < BT_ADDRESS_SIZE; i++) {
        rca.rc_bdaddr.b[i] = addr[i];
    }

    /* establish connection */
    status = connect(fd, (struct sockaddr*)&rca, sizeof(rca));

    /*
     * connect() can not return 0 for Bluetooth sockets but we should
     * handle this magic case.
     * No need to create a blocked handle
     * if connection is successfully established.
     */
    if (status == SOCKET_OK) {
        /* printf("connect_start: success\n"); */
        return BT_RESULT_SUCCESS;
    }

    if (status == SOCKET_ERROR) {
        if (errno == EWOULDBLOCK || /* workaround for a particular implementation */
            errno == EINPROGRESS ||
            errno == EALREADY) {
            bt_na_register_for_write(handle);
            return BT_RESULT_WOULDBLOCK;
        }
    }

    lastError = errno;
    close(fd);
    return BT_RESULT_FAILURE;
}

/**
 * Finishs connection establishment with the Bluetooth device.
 *
 * @param handle connection handle
 * @param context context
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if connection
 *             establishment is still in progress
 */
bt_result_t bt_rfcomm_connect_finish(bt_handle_t handle, void *context) {
    int err, status,ret, fd, err_size;
    struct pollfd pfd;

    (void)context;
    lastError = 0;
    fd = bt_na_get_fd(handle);

    pfd.fd = fd;
    pfd.events = POLLERR;
    ret = poll(&pfd, 1, 0);

    if (ret < 0) {
        lastError = errno;
        return BT_RESULT_FAILURE;
    }

    if (pfd.revents & POLLERR) {
        return BT_RESULT_FAILURE;
    }

    err_size = sizeof(err);
    status = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &err_size);
    /* printf( "bt_rfcomm_connect_finish: fd=%d, err=%d\n", fd, err); */

    if (status == SOCKET_OK && err == 0) {
        bt_na_unregister_for_write(handle);
        /* printf("bt_rfcomm_connect_finish connected: fd=%d\n", fd); */
        return BT_RESULT_SUCCESS;
    } else {
        lastError = err;
        bt_na_destroy(handle);
        close(fd);
        return BT_RESULT_FAILURE;
    }
}


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
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if data can not be sent
 *             at this moment
 */
static bt_result_t bt_rfcomm_send_common(bt_handle_t handle, char *pData,
        int len, int *pBytesSent)
{
    int status, fd;

    if (bt_na_get_status(handle) == BT_RESULT_INTERRUPTED) {
        /*
         * VMSocket status need not be set to any value at this point
         * as the VMSocket is deleted and corresponding BSD socket is
         * also closed
         */
        return BT_RESULT_INTERRUPTED;
    }

    lastError = 0;
    fd = bt_na_get_fd(handle);

    status = send(fd, pData, len, 0);

    if (SOCKET_ERROR == status) {
        if (EWOULDBLOCK == errno || EINPROGRESS == errno) {
            return BT_RESULT_WOULDBLOCK;
        } else if (EINTR == errno) {
            lastError = errno;
            return BT_RESULT_INTERRUPTED;
        } else {
            lastError = errno;
            return BT_RESULT_FAILURE;
        }
    }

    /* printf("send complete: fd=%d, len=%d, data={%s}\n",
        fd, status, pData); */

    *pBytesSent = status;
    return BT_RESULT_SUCCESS;
}

/**
 * See bt_rfcomm_send_common for definition.
 */
bt_result_t bt_rfcomm_send_start(bt_handle_t handle, char *pData, int len,
        int *pBytesSent, void **pContext)
{
    int status;

    status = bt_rfcomm_send_common(handle, pData, len, pBytesSent);

    if (status == BT_RESULT_WOULDBLOCK) {
        bt_na_register_for_write(handle);
        *pContext = NULL;
    }

    return status;
}


/**
 * See bt_rfcomm_send_common for definition.
 */
bt_result_t bt_rfcomm_send_finish(bt_handle_t handle, char *pData, int len,
        int *pBytesSent, void *context)
{
    int status;
    (void)context;

    status = bt_rfcomm_send_common(handle, pData, len, pBytesSent);

    if (status == BT_RESULT_WOULDBLOCK) {
        bt_na_register_for_write(handle);
    } else {
        bt_na_unregister_for_write(handle);
    }
    return status;
}

/**
 * Receives data via connection.
 *
 * @param handle connection handle
 * @param buf pointer to data buffer
 * @param len length of the buffer
 * @param pBytesReceived number of bytes that were received,
 *             <code>0</code> indicates end-of-stream
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred,
 *         <code>BT_RESULT_WOULDBLOCK</code> if there are no data available
 *             at this moment
 */
static bt_result_t bt_rfcomm_receive_common(bt_handle_t handle, char *pData,
        int len, int *pBytesReceived) {
    int status, fd;

    if (bt_na_get_status(handle) == BT_RESULT_INTERRUPTED) {
        /*
         * VMSocket status need not be set to any value at this point
         * as the VMSocket is deleted and corresponding BSD socket is
         * also closed after emitting an interrupted IO exception
         */
        return BT_RESULT_INTERRUPTED;
    }

    lastError = 0;
    fd = bt_na_get_fd(handle);

    status = recv(fd, pData, len, 0);

    if (SOCKET_ERROR == status) {
        if (EWOULDBLOCK == errno || EINPROGRESS == errno) {
            return BT_RESULT_WOULDBLOCK;
        } else if (EINTR == errno) {
            lastError = errno;
            return BT_RESULT_INTERRUPTED;
        } else {
            lastError = errno;
            return BT_RESULT_FAILURE;
        }
    }

    /* printf("receive complete: fd=%d, len=%d, data={%s}\n",
        fd, len, pData); */

    *pBytesReceived = status;
    return BT_RESULT_SUCCESS;
}

/**
 * See bt_rfcomm_receive_common for definition.
 */
bt_result_t bt_rfcomm_receive_start(bt_handle_t handle, char *pData, int len,
        int *pBytesReceived, void **pContext) {
    int status;

    status = bt_rfcomm_receive_common(handle, pData, len, pBytesReceived);

    if (status == BT_RESULT_WOULDBLOCK) {
        bt_na_register_for_read(handle);
        *pContext = NULL;
    }

    return status;
}


/**
 * See bt_rfcomm_receive_common for definition.
 */
bt_result_t bt_rfcomm_receive_finish(bt_handle_t handle, char *pData, int len,
        int *pBytesReceived, void *context)
{
    int status;

    (void)context;

    status = bt_rfcomm_receive_common(handle, pData, len, pBytesReceived);

    if (status == BT_RESULT_WOULDBLOCK) {
        bt_na_register_for_read(handle);
    } else {
        bt_na_unregister_for_read(handle);
    }

    return status;
}

/**
 * Returns the number of bytes available to be read from the connection
 * without blocking.
 *
 * @param handle connection handle
 * @param pCount pointer to variable the number of available bytes is stored in
 * @return <code>BT_RESULT_SUCCESS</code> on success,
 *         <code>BT_RESULT_FAILURE</code> if an error occurred
 */
bt_result_t bt_rfcomm_get_available(bt_handle_t handle, int* pCount) {
    const int BUF_SIZE = RFCOMM_DEFAULT_MTU;
    char buffer[BUF_SIZE];
    int res, fd;

    lastError = 0;
    fd = bt_na_get_fd(handle);

    /*
     * Check available input data in non-blocking mode
     * keeping the data in the queue.
     *
     * Note: there is a minus of this approach - the maximum number of
     * available bytes is limited by the buffer size.
     *
     * Note: there is another approach to determine the number of
     * available bytes bytes -
     * 'int n; ioctl(handle, FIONREAD, (void*)&n);'
     * but on some machines it fails with 'EINVAL (Invalid argument)' error,
     * on others always returns '0'.
     */
    res = recv(fd, buffer, BUF_SIZE, MSG_PEEK);

    if (res == SOCKET_ERROR) {
        *pCount = 0;
        if (errno != EWOULDBLOCK) {
            lastError = errno;
            return BT_RESULT_FAILURE;
        }
        return BT_RESULT_SUCCESS;
    }

    *pCount = res;
    return BT_RESULT_SUCCESS;
}
