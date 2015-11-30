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

#include "btSDDB.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

static sdp_session_t *g_sess;

bt_result_t bt_sddb_startup()
{
    g_sess = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
    return g_sess != NULL ? BT_RESULT_SUCCESS : BT_RESULT_FAILURE;
}

void bt_sddb_shutdown()
{
    if (g_sess != NULL) {
        sdp_close(g_sess);
    }
}

bt_result_t bt_sddb_update_record(bt_record_t *record)
{
    int scanned;
    sdp_record_t *rec = sdp_extract_pdu((uint8_t *)record->data, &scanned);
    if (rec == NULL) {
        return BT_RESULT_FAILURE;
    }
    if (record->id == BT_INVALID_SDDB_HANDLE) {
        if (sdp_record_register(g_sess, rec, 0) < 0) {
            sdp_record_free(rec);
            return BT_RESULT_FAILURE;
        }
        record->id = (bt_sddbid_t)rec->handle;
        return BT_RESULT_SUCCESS;
    } else {
        int retval;
        rec->handle = record->id;
        retval = sdp_record_update(g_sess, rec);
        sdp_record_free(rec);
        if (retval < 0) {
            return BT_RESULT_FAILURE;
        }
        record->id = (bt_sddbid_t)rec->handle;
        return BT_RESULT_SUCCESS;
    }
}

bt_result_t bt_sddb_update_psm(bt_sddbid_t id, uint16_t psm)
{
    (void)id;
    (void)psm;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_sddb_update_channel(bt_sddbid_t id, uint8_t cn)
{
    (void)id;
    (void)cn;
    return BT_RESULT_FAILURE;
}

bt_bool_t bt_sddb_exists_record(bt_sddbid_t id)
{
    (void)id;
    return BT_BOOL_FALSE;
}

bt_result_t bt_sddb_read_record(bt_record_t *record)
{
    (void)record;
    return BT_RESULT_FAILURE;
}

bt_result_t bt_sddb_remove_record(bt_sddbid_t id)
{
    if (id == 0) {
        return BT_RESULT_FAILURE;
    }
    sdp_record_unregister(g_sess, (sdp_record_t *)id);
    return BT_RESULT_SUCCESS;
}

size_t bt_sddb_get_records(bt_sddbid_t *array, size_t count)
{
    (void)array;
    (void)count;
    return 0;
}

uint32_t bt_sddb_get_service_classes(bt_sddbid_t id)
{
    (void)id;
    return BT_SDDB_SERVICE_CLASSES;
}
