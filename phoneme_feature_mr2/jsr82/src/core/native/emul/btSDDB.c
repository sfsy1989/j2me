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

#include <stdio.h>
#include <string.h>

#include <midpMalloc.h>

#define INDEX_TO_SDDBID(index) (bt_sddbid_t)(0x10000 + (index))
#define SDDBID_TO_INDEX(id) (int)(id - 0x10000)

static bt_record_t *g_records;
static int g_count;

bt_record_t *get_record(bt_sddbid_t id)
{
    int index = SDDBID_TO_INDEX(id);
    if (index < 0 || index >= g_count) {
        return NULL;
    }
    return &g_records[index];
}

uint8_t *get_attr_value(bt_record_t *record, uint16_t id)
{
    uint8_t *data = (uint8_t *)record->data;
    uint8_t *end = data + record->size;
    uint8_t type = *data >> 3;
    uint8_t size = *data & 0x07;
    if (type != 6) {
        return NULL;
    }
    switch (size) {
        case 5:
        case 6:
        case 7:
            data += size - 3;
            break;
        default:
            return NULL;
    }
    while (data < end) {
        if (*data != 0x09) {
            return NULL;
        }
        data++;
        if (*data == (id & 0xff00) && *(data + 1) == (id & 0x00ff)) {
            return data + 2;
        }
        data += 2;
        type = *data >> 3;
        size = *data & 0x07;
        data++;
        if (type == 0) {
            continue;
        }
        switch (size) {
            case 0:
            case 1:
            case 2:
            case 3:
                data += 1 << size;
                break;
            case 5:
                data += *data + 1;
                break;
            case 6:
                data += (((uint16_t)*data << 8) | *(data + 1)) + 2;
                break;
            case 7:
                data += (((uint32_t)*data << 24) |
                        ((uint32_t)*(data + 1) << 16) |
                        ((uint16_t)*(data + 2) << 8) |
                        *(data + 3)) + 4;
                break;
        }
    }
    return NULL;
}

bt_result_t bt_sddb_startup()
{
    return BT_RESULT_SUCCESS;
}

void bt_sddb_shutdown()
{
    int i;
    for (i = 0; i < g_count; i++) {
        if (g_records[i].data != NULL) {
            midpFree(g_records[i].data);
        }
    }
    midpFree(g_records);
}

bt_result_t bt_sddb_update_record(bt_record_t *record)
{
    if (record->id == BT_INVALID_SDDB_HANDLE) {
        int i;
        bt_record_t *rec = NULL;
        bt_sddbid_t id = BT_INVALID_SDDB_HANDLE;
        uint8_t *handle;
        for (i = 0; i < g_count; i++) {
            rec = &g_records[i];
            if (rec->data == NULL) {
                id = INDEX_TO_SDDBID(i);
                break;
            }
        }
        if (i == g_count) {
            bt_record_t *records = (bt_record_t *)midpRealloc(g_records,
                    (g_count + 1) * sizeof(bt_record_t));
            if (records == NULL) {
                return BT_RESULT_FAILURE;
            }
            g_records = records;
            rec = &g_records[g_count];
            id = INDEX_TO_SDDBID(g_count);
            g_count++;
        }
        rec->data = midpMalloc(record->size);
        if (rec->data == NULL) {
            return BT_RESULT_FAILURE;
        }
        memcpy(rec->data, record->data, record->size);
        rec->size = record->size;
        rec->classes = record->classes;
        rec->id = record->id = id;
        handle = get_attr_value(rec, 0x0000);
        if (handle != NULL && *handle++ == 0x0a) {
            *handle++ = (uint8_t)(id >> 24);
            *handle++ = (uint8_t)(id >> 16);
            *handle++ = (uint8_t)(id >> 8);
            *handle = (uint8_t)id;
        }
        return BT_RESULT_SUCCESS;
    } else {
        bt_record_t *rec = get_record(record->id);
        if (rec == NULL) {
            return BT_RESULT_FAILURE;
        }
        rec->classes = record->classes;
        if (rec->size != record->size) {
            void *data = midpRealloc(rec->data, record->size);
            if (data == NULL) {
                return BT_RESULT_FAILURE;
            }
            rec->data = data;
            rec->size = record->size;
        }
        memcpy(rec->data, record->data, record->size);
        return BT_RESULT_SUCCESS;
    }
}

bt_result_t bt_sddb_update_psm(bt_sddbid_t id, uint16_t psm)
{
    const uint8_t l2cap_uuid[16] = {
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb
    };
    int i;
    uint8_t *data;
    bt_record_t *record = get_record(id);
    if (record == NULL) {
        return BT_RESULT_FAILURE;
    }
    data = (uint8_t *)record->data;
    if (data == NULL) {
        return BT_RESULT_FAILURE;
    }
    for (i = 0; i < (int)record->size - 18; i++) {
        if (!memcmp(data, l2cap_uuid, 16) && *(data + 16) == 0x09) {
            *(data + 17) = psm >> 8;
            *(data + 18) = psm & 0xff;
            return BT_RESULT_SUCCESS;
        }
        data++;
    }
    return BT_RESULT_FAILURE;
}

bt_result_t bt_sddb_update_channel(bt_sddbid_t id, uint8_t cn)
{
    const uint8_t rfcomm_uuid[16] = {
        0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb
    };
    int i;
    uint8_t *data;
    bt_record_t *record = get_record(id);
    if (record == NULL) {
        return BT_RESULT_FAILURE;
    }
    data = (uint8_t *)record->data;
    if (data == NULL) {
        return BT_RESULT_FAILURE;
    }
    for (i = 0; i < (int)record->size - 17; i++) {
        if (!memcmp(data, rfcomm_uuid, 16) && *(data + 16) == 0x08) {
            *(data + 17) = cn;
            return BT_RESULT_SUCCESS;
        }
        data++;
    }
    return BT_RESULT_FAILURE;
}

bt_bool_t bt_sddb_exists_record(bt_sddbid_t id)
{
    bt_record_t *record = get_record(id);

    if (record == NULL) {
        return BT_BOOL_FALSE;
    }

    return record->data != NULL ? BT_BOOL_TRUE : BT_BOOL_FALSE;
}

bt_result_t bt_sddb_read_record(bt_record_t *record)
{
    bt_record_t *rec = get_record(record->id);
    if (rec == NULL) {
        return BT_RESULT_FAILURE;
    }
    if (record->size == 0) {
        record->size = rec->size;
        return BT_RESULT_SUCCESS;
    }
    if (record->size > rec->size) {
        record->size = rec->size;
    }
    memcpy(record->data, rec->data, record->size);
    record->classes = rec->classes;
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_sddb_remove_record(bt_sddbid_t id)
{
    bt_record_t *record = get_record(id);
    if (record == NULL) {
        return BT_RESULT_FAILURE;
    }
    midpFree(record->data);
    record->data = NULL;
    record->size = 0;
    record->classes = 0;
    return BT_RESULT_SUCCESS;
}

size_t bt_sddb_get_records(bt_sddbid_t *array, size_t count)
{
    int i;
    bt_sddbid_t *last;
    if (count <= 0) {
        count = 0;
        for (i = 0; i < g_count; i++) {
            if (g_records[i].data != NULL) {
                count++;
            }
        }
        return count;
    }
    last = array + count;
    count = 0;
    for (i = 0; i < g_count && array < last; i++) {
        if (g_records[i].data != NULL) {
            *array++ = INDEX_TO_SDDBID(i);
            count++;
        }
    }
    return count;
}

uint32_t bt_sddb_get_service_classes(bt_sddbid_t id)
{
    int i;
    uint32_t classes = BT_SDDB_SERVICE_CLASSES;
    if (id != BT_INVALID_SDDB_HANDLE) {
        bt_record_t *record = get_record(id);
        if (record == NULL) {
            return 0;
        }
        return record->classes;
    }
    for (i = 0; i < g_count; i++) {
        bt_record_t *record = &g_records[i];
        if (record->data != NULL) {
            classes |= record->classes;
        }
    }
    return classes;
}
