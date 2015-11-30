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

/**
 * @file
 * @defgroup jsr82irda IrDA Protocol Porting Interface
 * @ingroup jsr82protocol
 * @brief #include <IrConnection.h>
 * @{
 */

#ifndef __JSR82_IR_CONNECTION__
#define __JSR82_IR_CONNECTION__

#include <unistd.h>

/**
 * @def IAS_MAX_CLASSNAME
 * Maximum size of IAS class name.
 */
#define IAS_MAX_CLASSNAME 60

/**
 * @def INVALID_IR_HANDLE
 * Invalid connection handle value.
 */
#define INVALID_IR_HANDLE -1

/**
 * @def INVALID_DATA_HANDLE
 * Invalid memory handle value.
 */
#define INVALID_DATA_HANDLE 0

/**
 * @def MAX_SOCKETS
 * Maximum number of listening sockets.
 */
#define MAX_SOCKETS 20

/**
 * @def MAX_DEVICES
 * Maximum number of discoverable devices.
 */
#define MAX_DEVICES 10

#ifdef __cplusplus
extern "C" {
#endif

/** Type for storing connection handles. */
typedef int ir_handle_t;

/** Type for storing device addresses. */
typedef int ir_addr_t;

/** Type for storing hint bits. */
typedef int ir_hints_t;

/** Type for storing IAS class names. */
typedef const char *ir_ias_t;

/** Enumeration type for reporting operation results. */
typedef enum {
    IR_ERROR = -1,
    IR_SUCCESS = 0,
    IR_AGAIN,
    IR_INPROGRESS
} ir_result_t;

/**
 * Creates a new connection handle. Only resources are allocated, and no
 * connection is established by this function.
 *
 * @param handle variable to receive a newly created handle
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_create(ir_handle_t *handle);

/**
 * Discovers nearby infrared devices.
 *
 * @param hints hint bits used for filtering
 * @param ias IAS class name used for filtering
 * @param addr array to receive addresses of discovered devices
 * @param count contains the number of entries in the <code>addr</code> array
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_discover(ir_hints_t hints, ir_ias_t ias,
        ir_addr_t *addr, size_t *count);

/**
 * Opens connection to a remote device.
 *
 * @param handle connection handle to be associated with a connection
 * @param addr address of a remote device
 * @param ias IAS class name
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_INPROGRESS</code> if the operation was not completed
 */
ir_result_t ir_connect(ir_handle_t handle, ir_addr_t addr, ir_ias_t ias);

/**
 * Completes opening connection to a remote device.
 *
 * @param handle connection handle to be associated with a connection
 * @param addr address of a remote device
 * @param ias IAS class name
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_connect_complete(ir_handle_t handle);

/**
 * Closes connection associated with a connection handle and releases
 * resources associated with the handle.
 *
 * @param handle connection handle
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_close(ir_handle_t handle);

/**
 * Receives data from an open connection.
 *
 * @param handle handle to an open connection
 * @param buf buffer to receive the data
 * @param count number of bytes to be received / actually received
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_receive(ir_handle_t handle, void *buf, size_t *count);

/**
 * Transmits data over an open connection.
 *
 * @param handle handle to an open connection
 * @param buf buffer containing the data to send
 * @param count number of bytes to be sent / actually sent
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_send(ir_handle_t handle, const void *buf, size_t *count);

/**
 * Prepares to listen for incoming connections.
 *
 * @param handle connection handle
 * @param hints hint bits to be advertised
 * @param ias IAS class name
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure
 */
ir_result_t ir_listen(ir_handle_t handle, ir_hints_t hints, ir_ias_t ias);

/**
 * Accepts an incoming connection.
 *
 * @param handle connection handle in listening state
 * @param peer variable to receive peer connection handle
 * @return <code>IR_SUCCESS</code> on success, <code>IR_ERROR</code> on failure,
 *         <code>IR_AGAIN</code> if the operation needs to be repeated
 */
ir_result_t ir_accept(ir_handle_t handle, ir_handle_t *peer);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __JSR82_IR_CONNECTION__ */
