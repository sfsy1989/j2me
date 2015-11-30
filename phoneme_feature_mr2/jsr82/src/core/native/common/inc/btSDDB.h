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

#ifndef __BT_SDDB_H__
#define __BT_SDDB_H__

#include <unistd.h>

#include "btCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes service discovery database.
 *
 * @return BT_RESULT_SUCCESS on success, BT_RESULT_FAILURE on failure
 */
bt_result_t bt_sddb_startup();

/**
 * Closes service discovery database. All remaining Java-supplied service
 * records must be removed.
 */
void bt_sddb_shutdown();

/**
 * Creates or updates service record in the service discovery database (SDDB).
 *
 * @param record structure with binary data representing the service record.
 *         The <code>id</code> field of the structure is used to identify
 *         the service record in SDDB to be updated. If equals to
 *         <code>0</code>, a new record is created. The <code>id</code>
 *         field is modified upon return from this function to indicate the
 *         new servie record handle value; the function also updates
 *         attribute 0x0000 in SDDB (but not in the data supplied with
 *         the structure)
 * @return BT_RESULT_SUCCESS on success, BT_RESULT_FAILURE on failure
 */
bt_result_t bt_sddb_update_record(bt_record_t *record);

/**
 * Updates PSM parameter value of the L2CAP protocol in the service record.
 * This function is required for push implementation.
 *
 * @param id handle of the service record to be updated
 * @param psm new PSM value
 * @return BT_RESULT_SUCCESS on success, BT_RESULT_FAILURE on failure
 */
bt_result_t bt_sddb_update_psm(bt_sddbid_t id, uint16_t psm);

/**
 * Updates channel parameter value of the RFCOMM protocol in the service record.
 * This function is required for push implementation.
 *
 * @param id handle of the service record to be updated
 * @param cn new channel value
 * @return BT_RESULT_SUCCESS on success, BT_RESULT_FAILURE on failure
 */
bt_result_t bt_sddb_update_channel(bt_sddbid_t id, uint8_t cn);

/**
 * Checks if a service record exists in SDDB.
 *
 * @param id handle of a service record to look up
 * @return BT_BOOL_TRUE if the record exists, BT_BOOL_FALSE otherwise
 */
bt_bool_t bt_sddb_exists_record(bt_sddbid_t id);

/**
 * Reads a service record from SDDB.
 * This function is required for push implementation.
 *
 * @param record structure containing handle of a service record to read,
 *         buffer to receive binary data representing the service record,
 *         and length of the buffer in bytes (or 0 for size query).
 * @return BT_RESULT_SUCCESS on success, BT_RESULT_FAILURE on failure
 */
bt_result_t bt_sddb_read_record(bt_record_t *record);

/**
 * Removes service record from SDDB.
 *
 * @param id handle of a service record to remove
 * @return BT_RESULT_SUCCESS on success, BT_RESULT_FAILURE otherwise
 */
bt_result_t bt_sddb_remove_record(bt_sddbid_t id);

/**
 * Returns handles for all service records in SDDB.
 * This function is required for the emulation build, when SDP server is
 * maintained by Java, rather than by Bluetooth stack.
 *
 * @param array buffer to receive service record handles
 * @param count number of entries in the array, or 0 for count query
 * @return number of entries available/saved to the array,
 *         or 0 if an error occurs
 */
size_t bt_sddb_get_records(bt_sddbid_t *array, size_t count);

/**
 * Retrieves service classes for either a single service record,
 * or representing the current state of entire SDDB.
 *
 * @param id service record handle
 * @return major device service classes
 */
uint32_t bt_sddb_get_service_classes(bt_sddbid_t id);

#ifdef __cplusplus
}
#endif

#endif
