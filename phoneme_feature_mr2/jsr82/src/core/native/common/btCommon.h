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

#ifndef __BT_COMMON_H__
#define __BT_COMMON_H__

/**
 * @file
 * @defgroup jsr82common Common Bluetooth declarations
 * @ingroup jsr82
 * @brief #include <btCommon.h>
 * @{
 */

#include <stddef.h>
#include <btTypes.h>

/**
 * @def BLUETOOTH_STACK_HEADER
 * Header file containing C++ CBluetoothStack-derived class.
 */
#define BLUETOOTH_STACK_HEADER "PlainBluetoothStack.h"

/**
 * @def BLUETOOTH_STACK_CTOR
 * Constructor call of a class derived from CBluetoothStack.
 */
#define BLUETOOTH_STACK_CTOR CPlainBluetoothStack()

/**
 * @def BT_ADDRESS_SIZE
 * Bluetooth address size.
 */
#define BT_ADDRESS_SIZE  6

/**
 * @def MAX_DEVICES
 * Maximum number of devices connected.
 */
#define MAX_DEVICES 7

/** Major device service classes which should always be set. */
#define BT_SDDB_SERVICE_CLASSES 0x100000

/** Type name for storing Bluetooth addresses. */
typedef uint8_t bt_bdaddr_t[BT_ADDRESS_SIZE];

/** Type name for storing UTF-8 strings, e.g. user-friendly device names. */
typedef char *bt_string_t;

/**
 * The result values for porting API functions.
 *
 * Defines possible result values for porting API functions.
 */
typedef enum {
    BT_RESULT_SUCCESS       =  0, /**< Indicates success. */

    BT_RESULT_FAILURE       = -1, /**< Indicates generic failure.
                                   *
                                   * For protocol functions indicates what
                                   * an I/O error of some sort has occurred.
                                   */

    BT_RESULT_WOULDBLOCK    = -2, /**< Indicates that called function
                                   * would block.
                                   *
                                   * The finish-function will need to be called
                                   * later in order to complete the operation.
                                   */

    BT_RESULT_INTERRUPTED   = -3,  /**< Indicates what the operation was
                                    * terminated by an interrupt.
                                    *
                                    * This typically causes
                                    * InterruptedIOException to be thrown in
                                    * calling Java thread.
                                    */

    BT_RESULT_INVALID_PARAM = -4,  /**< Indicates what a function parameter had
                                    * an invalid value. */
} bt_result_t;

/**
 * Event type.
 */
typedef enum {
    BT_EVENT_CONNECT_COMPLETE,
    BT_EVENT_ACCEPT_COMPLETE,
    BT_EVENT_SEND_COMPLETE,
    BT_EVENT_RECEIVE_COMPLETE,
} bt_callback_type_t;

/** Type for storing boolean values. */
typedef enum {
    BT_BOOL_FALSE  = 0, /**< False. */
    BT_BOOL_TRUE   = 1  /**< True. */
} bt_bool_t;

/** Type for storing inquiry result response. */
typedef struct {
    bt_bdaddr_t bdaddr; /**< Bluetooth address of a discovered device. */
    uint32_t cod;       /**< 'Class of device' parameter. */
} bt_inquiry_t;

/**
 * @def BT_INVALID_ACL_HANDLE
 * Defines invalid ACL handle value.
 *
 * Note: ACL valid handle range: 0x0000-0x0EFF (0x0F00-0x0FFF reserved for
 * future use).
 */
#define BT_INVALID_ACL_HANDLE  -1

/** L2CAP/RFCOMM handle type. */
typedef void *bt_handle_t;

/**
 * @def BT_INVALID_HANDLE
 * Defines invalid L2CAP/RFCOMM connection handle.
 */
#define BT_INVALID_HANDLE (void *)(-1)

/** Type for storing 128-bit UUIDs. */
typedef uint8_t bt_uuid_t[16];

/** Protocol types enumeration. */
typedef enum { BT_L2CAP, BT_SPP, BT_GOEP } bt_protocol_t;

/** Type representing push registration entry. */
typedef struct {
    bt_protocol_t protocol; /**< Protocol type. */
    bt_uuid_t uuid;         /**< Service UUID. */
} bt_port_t;

/** Service record handle type. */
typedef uint32_t bt_sddbid_t;

/**
 * @def BT_INVALID_SDDB_HANDLE
 * Defines invalid service record handle.
 */
#define BT_INVALID_SDDB_HANDLE 0

/** Type for stroing service records. */
typedef struct bt_record_t bt_record_t;
struct bt_record_t {
    bt_sddbid_t id;   /**< Service record handle. */
    uint32_t classes; /**< Device service classes activated by this record. */
    void *data;       /**< Binary representation of this record. */
    size_t size;      /**< Size of the binary data. */
};

/** @} */

#endif /* __BT_COMMON_H__ */
