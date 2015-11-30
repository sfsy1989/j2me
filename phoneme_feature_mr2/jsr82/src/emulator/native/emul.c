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
 
#include <emul.h>
#include <sni.h>
#include <btNotify.h>
#include <btStackEvent.h>
#include <midpResourceLimit.h>
#include <midpStorage.h>
#include <pcsl_network.h>
#include <pcsl_string.h>
#include <midp_thread.h>

int state = 0;
emul_data_t emul_data;

void reallocQueue() {
    emul_data.req_size += DEFAULT_QUEUE_SIZE;
    LOG1("EMUL QUEUE reallocated to %d", emul_data.req_size);
    emul_data.request = midpRealloc(emul_data.request, emul_data.req_size);
}

void ensureInitialized() {
    if (!(state && INITIALIZED)) {
        memset(&emul_data, 0, sizeof(emul_data));
        reallocQueue();
        state |= INITIALIZED;
    }
}

void queueRequest(char* req, int len) {
    ensureInitialized();
    if (emul_data.req_offset + len >=  emul_data.req_size) {
        reallocQueue();
    }
 
    memcpy(&emul_data.request[emul_data.req_offset], req, len);
    emul_data.req_offset += len;
}

bt_handle_t getNextHandle(int protocol) {
    // avoid returning handle of just used connection
    static int next = 0;
    int i;
    bt_handle_t handle = BT_INVALID_HANDLE;
    
    ensureInitialized();
    for (i = 1; i <= MAX_CONN; i++) {
        int h = (next + i) % MAX_CONN;
        accept_info_t *info = &emul_data.handled_info[h].accept;
        if (!(info->flags & IN_USE)) {
            handle = (bt_handle_t)h;
            next = h;
            resetConnInfo(handle);
            info->flags |= IN_USE | protocol;
            break;
        }
    }
    
    if (BT_INVALID_HANDLE == handle) {
        REPORT_ERROR(BTE_LOG_CH, "Connections handles resource expired");
    }
    
    return handle;
}

KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr82emul_EmulationPolling_start0() {
    jboolean retval = KNI_FALSE;

    LOG("EmulationPolling_start0()");    
    
    if (!(state & POLLING_STARTED)) {
        LOG("EmulationPolling_start0(), first call");
        
        retval = KNI_TRUE;
        ensureInitialized();
        state |= POLLING_STARTED;
    }
    
    KNI_ReturnBoolean(retval);
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_EmulationPolling_initIsolate() {
    LOG("EmulationPolling_initIsolate()");
    midpAllocateReservedResources();
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_EmulationPolling_finalizeIsolate() {
    LOG("EmulationPolling_initIsolate()");
    midpFreeReservedResources();
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_EmulationPolling_finalize() {
    int i;
    LOG("EmulationPolling_finalize()");
    for (i = 0; i < MAX_CONN; i++) {
        resetConnInfo((bt_handle_t)i);
    }
    midpFree(emul_data.request);
    emul_data.request = NULL;
    state = 0;
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_EmulationPolling_getRequest() {
    static jfieldID bufID = 0;
    static jfieldID lengthID = 0;
    int len = emul_data.req_offset > 512? 512 : emul_data.req_offset;
    
    KNI_StartHandles(3);
    KNI_DeclareHandle(thisHandle);     
    KNI_DeclareHandle(classHandle);     
    KNI_DeclareHandle(bufHandle);     
    
    if (bufID == 0) {
        KNI_GetClassPointer(classHandle);
        bufID = KNI_GetFieldID(classHandle, "requestBuf", "[B");
        lengthID = KNI_GetFieldID(classHandle, "length", "I");
    }
    
    KNI_GetThisPointer(thisHandle);
    KNI_SetIntField(thisHandle, lengthID, (jint)len);
    
    if (emul_data.req_offset > 0) {
        LOG1("EmulationPolling_getRequest(): getting %d request bytes", 
            emul_data.req_offset);
        
        KNI_GetObjectField(thisHandle, bufID, bufHandle);          
        memcpy(JavaByteArray(bufHandle), emul_data.request, len);
        if (len < emul_data.req_offset) {
            emul_data.req_offset -= len;
            memmove(emul_data.request, &emul_data.request[len], 
                emul_data.req_offset);
        } else {
            emul_data.req_offset = 0;
        }
    }
    
    KNI_EndHandles();
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_NotifierEmul_notifyAccepted() {
    int myHandle = KNI_GetParameterAsInt(1);
    int connHandle = KNI_GetParameterAsInt(2);
    accept_info_t *info = &emul_data.handled_info[myHandle].accept;
    connection_info_t *connInfo;
    
    LOG("NotifierEmul_notifyAccepted()");    
    
    // Otherwize notifier is already closed
    // IMPL_NOTE: what if it is already used by another connection or notifier... 
    if (info->flags & IN_USE) {
        if (connHandle == -1) {
            bt_notify_protocol_event(BT_EVENT_ACCEPT_COMPLETE, 
                (bt_handle_t)myHandle, BT_RESULT_FAILURE);
        } else { 
            KNI_StartHandles(1);
            KNI_DeclareHandle(addr);
            KNI_GetParameterAsObject(3, addr);
            
            info->conn_handle = connHandle;
            memcpy(info->peer_addr, JavaByteArray(addr), BT_ADDRESS_SIZE);
            info->imtu = (int)KNI_GetParameterAsInt(4);
            info->omtu = (int)KNI_GetParameterAsInt(5);
            info->status = BT_RESULT_SUCCESS;
            
            KNI_EndHandles();
            
            connInfo = &emul_data.handled_info[connHandle].conn;  
            // Copying address and MTU's
            memcpy(connInfo->peer_addr, info->peer_addr, BT_ADDRESS_SIZE);
            connInfo->omtu = info->omtu;
            connInfo->imtu = info->imtu;
            
            bt_notify_protocol_event(BT_EVENT_ACCEPT_COMPLETE, 
                (bt_handle_t)myHandle, BT_RESULT_SUCCESS);
        }
    }
  
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_jsr82emul_ConnectionEmul_getHandle() {
    int protocol = (int)KNI_GetParameterAsInt(1);
    jint handle = (jint)getNextHandle(protocol);
    KNI_ReturnInt(handle);
}

KNIEXPORT KNI_RETURNTYPE_BOOLEAN
Java_com_sun_midp_jsr82emul_ConnectionEmul_getOutData() {
    int  myHandle = (int)KNI_GetParameterAsInt(1);
    connection_info_t *info = &emul_data.handled_info[myHandle].conn;
    jboolean ret = KNI_TRUE;
    int buflen;
    
    LOG1("ConnectionEmul_getOutData(%d)", myHandle);    
    
    if (info->flags & IN_USE) {
        KNI_StartHandles(1);
        KNI_DeclareHandle(buf);
        KNI_GetParameterAsObject(2, buf);
        buflen = KNI_GetArrayLength(buf);
        
        if (info->out_len < buflen) {
            LOG1("ConnectionEmul_getOutData(%d) requests too much data", myHandle);
            ret = KNI_FALSE;
        } else {
            memcpy(JavaByteArray(buf), info->out, buflen);
            info->out_len -= buflen;
            memmove(info->out, &info->out[buflen], info->out_len);
            info->out = midpRealloc(info->out, info->out_len);
            if (info->out_len == 0) {
                info->out = NULL;
            }
        }
        
        KNI_EndHandles();
    } else {
        ret = KNI_FALSE;
    }
    
    KNI_ReturnBoolean(ret);
}
    
KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_ConnectionEmul_notifyOpen() {
    int myHandle = KNI_GetParameterAsInt(1);
    jboolean success = KNI_GetParameterAsBoolean(2);
    connection_info_t *info = &emul_data.handled_info[myHandle].conn;

    LOG1("ConnectionEmul_notifyOpen(%d)", myHandle);

    if (success == KNI_FALSE) {
        info->connect_status = BT_RESULT_FAILURE;
    } else {
        info->imtu = KNI_GetParameterAsInt(3);
        info->omtu = KNI_GetParameterAsInt(4);
        info->connect_status = BT_RESULT_SUCCESS;
    }
    
    bt_notify_protocol_event(BT_EVENT_CONNECT_COMPLETE, 
        (bt_handle_t)myHandle, info->connect_status);
  
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_ConnectionEmul_notifyReceived() {
    int myHandle = KNI_GetParameterAsInt(1);
    int buflen = KNI_GetParameterAsInt(3);
    connection_info_t *info = &emul_data.handled_info[myHandle].conn;
    
    LOG1("ConnectionEmul_notifyReceived(%d)", myHandle);
    
    // otherwise connection is already closed
    if (info->flags & IN_USE) { 
        info->flags &= ~SKIP_REQUEST;
        
        LOG1("ConnectionEmul_notifyReceived() received %d", buflen);
        
        if (buflen == CONN_FAILURE) {
            info->receive_status = BT_RESULT_FAILURE;
            
        } else {
            info->receive_status = BT_RESULT_SUCCESS;
            
            if (buflen == CONN_ENDOF_INP) {
                info->flags |= ENDOF_INP_REACHED;
            } else {
                KNI_StartHandles(1);
                KNI_DeclareHandle(buf);
                KNI_GetParameterAsObject(2, buf);
                
                APPEND_BUFFER(info->in, info->in_len, JavaByteArray(buf), buflen);
                KNI_EndHandles();
            }
                
            LOG1("ConnectionEmul_notifyReceived() info->in_len %d", info->in_len);
        }
        
        bt_notify_protocol_event(BT_EVENT_RECEIVE_COMPLETE, 
            (bt_handle_t)myHandle, info->receive_status);
    }
            
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_ConnectionEmul_notifySent() {
    int myHandle = KNI_GetParameterAsInt(1);
    connection_info_t *info = &emul_data.handled_info[myHandle].conn;
    
    // otherwise connection is already closed
    if (info->flags & IN_USE) {
        int sent = KNI_GetParameterAsInt(2);
        
        LOG1("ConnectionEmul_notifySent(%d)", myHandle);
        info->sent = sent;
        
        bt_notify_protocol_event(BT_EVENT_SEND_COMPLETE, 
            (bt_handle_t)myHandle, 
            sent == CONN_FAILURE ? BT_RESULT_FAILURE : BT_RESULT_SUCCESS);
    }
    
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_kvem_jsr082_bluetooth_SDPServer_requestPSM() {
    state |= SDP_REQ_EXPECTED;
    KNI_ReturnVoid();
}

/**
 * Opens file that keeps device and service classes.
 * @return handle for the file, <code>-1</code> if opening failed.
 */
static int openCodFile() {
    char *error;
    int handle = -1;
    pcsl_string name = PCSL_STRING_EMPTY;
    int i;
    
    if (PCSL_STRING_OK == pcsl_string_append(&name,
            storage_get_root(INTERNAL_STORAGE_ID))) {
        for (i = 0; i < BT_ADDRESS_SIZE * 2; i++) {
            char c = (emul_data.local_addr[i / 2] >> ((i % 2) * 4)) & 0xf;
            if (c <= 9) {
                c += '0';
            } else {
                c += 'a';
            }
            
            if (PCSL_STRING_OK != pcsl_string_append_char(&name, c)) {
                break;
            }
        }
    
        if (i == BT_ADDRESS_SIZE * 2) {
            handle = storage_open(&error, &name, OPEN_READ_WRITE);
            storageFreeError(error);
        }
    }
    
    pcsl_string_free(&name);
    return handle;
}

/**
 * Returns class of device and service classes saved for an 
 * emulated device with the same address previous time. 
 * It is considered to be the same device as current one.
 */
static int loadCod() {
    char *error;
    int cod = DEFAULT_COD;
    int handle = openCodFile();
    
    if (-1 != handle) {
        storageRead(&error, handle, (char*)&cod, sizeof(int));
        storageFreeError(error);
    }
    
    storageClose(&error, handle);
    storageFreeError(error);
    
    return cod;
}

/**
 * Saves class of device and service classes for the next 
 * usage of the same device. Makes nothing if an IO error
 * occured.
 */
static void saveCod(int cod) {
    char *error;
    int handle = openCodFile();
    
    if (-1 != handle) {
        storageWrite(&error, handle, (char*)&cod, sizeof(int));
        storageFreeError(error);
    }
    
    storageClose(&error, handle);
    storageFreeError(error);
}

KNIEXPORT KNI_RETURNTYPE_INT
Java_com_sun_midp_jsr82emul_DeviceEmul_initDevice() {
    LOG("DeviceEmul_initDevice()");
    
    KNI_StartHandles(1);
    KNI_DeclareHandle(buf);
    KNI_GetParameterAsObject(1, buf);
    memcpy(emul_data.local_addr, JavaByteArray(buf), BT_ADDRESS_SIZE);
    KNI_EndHandles();
    
    emul_data.device_class_base = DEFAULT_COD;
    emul_data.device_class = loadCod();
    emul_data.access_code = KNI_GetParameterAsInt(2);

    state |= DEVICE_INITED;
    midp_thread_signal(JSR82_SIGNAL, BTE_SIGNAL_HANDLE, 0);

    KNI_ReturnInt((jint)emul_data.device_class);
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_DeviceEmul_finalize() {
    LOG("DeviceEmul_deviceDiscovered()");
    saveCod(emul_data.device_class);
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_DeviceEmul_deviceDiscovered() {
    int cod = (int)KNI_GetParameterAsInt(2);
    bt_inquiry_t result;
    LOG("DeviceEmul_deviceDiscovered()");
    
    KNI_StartHandles(1);
    KNI_DeclareHandle(buf);
    KNI_GetParameterAsObject(1, buf);
    
    result.cod = cod;
    memcpy(result.bdaddr, JavaByteArray(buf), BT_ADDRESS_SIZE);
    bt_stack_on_inquiry_result(&result, 1);
    
    KNI_EndHandles();
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID
Java_com_sun_midp_jsr82emul_DeviceEmul_inquiryCompleted() {
    jboolean success = (int)KNI_GetParameterAsBoolean(1);
    LOG("DeviceEmul_inquiryCompleteted()");
    
    bt_stack_on_inquiry_complete(success? BT_BOOL_TRUE : BT_BOOL_FALSE);
    KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_sun_midp_jsr82emul_EmulationClient_getLocalIP() {
    char value[4 * 4];

    KNI_StartHandles(1);
    KNI_DeclareHandle(result);

    if (PCSL_NET_SUCCESS == pcsl_network_getLocalIPAddressAsString(value)) {
        KNI_NewStringUTF(value, result);
    } else {
        KNI_ReleaseHandle(result);
    }
    
    KNI_EndHandlesAndReturnObject(result); 
}

KNIEXPORT KNI_RETURNTYPE_OBJECT
Java_com_sun_midp_jsr82emul_EmulationClient_getServerIP() {
    char *value = getenv("JSR82_EMUL_IP");
    
    KNI_StartHandles(1);
    KNI_DeclareHandle(result);

    if (NULL != value) {
        KNI_NewStringUTF(value, result);
    } else {
        KNI_ReleaseHandle(result);
    }
    
    KNI_EndHandlesAndReturnObject(result); 
}
