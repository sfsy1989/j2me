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


#include "btPush.h"
#include <javacall_bt.h>

#include <kni.h>
#include <commonKNIMacros.h>
#include <midpUtilKni.h>
#include <midpMalloc.h>
#include <midpString.h>

/**
 * Creates or updates service record in the SDDB.
 *
 * @param handle handle of the service record to be updated;
 *         if equals to 0, a new record will be created
 * @param classes device service classes associated with the record
 * @param data binary data containing attribute-value pairs in the format
 *         identical to the one used in the AttributeList parameter of
 *         the SDP_ServiceAttributeResponse PDU
 * @return service record handle, or 0 if the operation fails
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_kvem_jsr082_bluetooth_SDDB_updateRecord(void)
{
    bt_sddbid_t retval;
    bt_record_t record;
    bt_sddbid_t old_id = (bt_sddbid_t)KNI_GetParameterAsInt(1);
    KNI_StartHandles(1);
    KNI_DeclareHandle(dataHandle);
    record.id = old_id;
    record.classes = KNI_GetParameterAsInt(2);
    KNI_GetParameterAsObject(3, dataHandle);
    record.data = JavaByteArray(dataHandle);
    record.size = KNI_GetArrayLength(dataHandle);
    if (javacall_bt_sddb_update_record(&record.id, record.classes,
            record.data, record.size) == JAVACALL_OK) {
        retval = record.id;
        if (old_id != BT_INVALID_SDDB_HANDLE) {
            bt_push_update_record(old_id, &record);
        }
        javacall_bt_stack_set_service_classes(javacall_bt_sddb_get_service_classes(0));
    } else {
        retval = BT_INVALID_SDDB_HANDLE;
    }
    KNI_EndHandles();
    KNI_ReturnInt(retval);
}

/**
 * Removes service record from the SDDB.
 *
 * @param handle hanlde of the service record to be deleted
 */
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_kvem_jsr082_bluetooth_SDDB_removeRecord(void)
{
    javacall_bt_sddb_remove_record((bt_sddbid_t)KNI_GetParameterAsInt(1));
    javacall_bt_stack_set_service_classes(javacall_bt_sddb_get_service_classes(0));
    KNI_ReturnVoid();
}

/**
 * Retrieves service record from the SDDB.
 *
 * @param handle handle of the service record to be retrieved
 * @param data byte array which will receive the data,
 *         or null for size query
 * @return size of the data read/required
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_kvem_jsr082_bluetooth_SDDB_readRecord(void)
{
    jint retval;
    bt_record_t record;
    KNI_StartHandles(1);
    KNI_DeclareHandle(dataHandle);
    KNI_GetParameterAsObject(2, dataHandle);
    record.id = (bt_sddbid_t)KNI_GetParameterAsInt(1);
    if (KNI_IsNullHandle(dataHandle)) {
        record.data = NULL;
        record.size = 0;
    } else {
        record.data = JavaByteArray(dataHandle);
        record.size = KNI_GetArrayLength(dataHandle);
    }
    if (javacall_bt_sddb_read_record(record.id, &record.classes,
            record.data, &record.size) == JAVACALL_OK) {
        retval = record.size;
    } else {
        retval = 0;
    }
    KNI_EndHandles();
    KNI_ReturnInt(retval);
}

/**
 * Retrieves service classes of a record in the SDDB.
 *
 * @param handle handle of the service record
 * @return service classes set for the record
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_kvem_jsr082_bluetooth_SDDB_getServiceClasses(void)
{
    KNI_ReturnInt(javacall_bt_sddb_get_service_classes(KNI_GetParameterAsInt(1)));
}

/**
 * Retrieves handles of all service records in the SDDB.
 *
 * @param handles array to receive handles, or null for count query
 * @return number of entries read/available
 */
KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_kvem_jsr082_bluetooth_SDDB_getRecords(void)
{
    jint retval;
    KNI_StartHandles(1);
    KNI_DeclareHandle(arrayHandle);
    KNI_GetParameterAsObject(1, arrayHandle);
    if (KNI_IsNullHandle(arrayHandle)) {
        retval = javacall_bt_sddb_get_records(NULL, 0);
    } else {
        retval = javacall_bt_sddb_get_records((bt_sddbid_t *)JavaIntArray(arrayHandle),
                KNI_GetArrayLength(arrayHandle));
    }
    KNI_EndHandles();
    KNI_ReturnInt(retval);
}
