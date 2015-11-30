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
 * 
 * This source file is specific for Qt-based configurations.
 */

/**
 * @file
 * 
 * This file implements the bluetooth notification adapter functions for a 
 * Linux-QTE platform
 */

#include <bt_na.h>
#include <bt_socket_qte.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * See bt_na.h for definition.
 */
void* bt_na_create(int fd) {
    return createBtSocketHandle(fd);
}

/**
 * See bt_na.h for definition.
 */
int bt_na_get_fd(void *handle) {
    return getBtSocketHandleFD(handle);
}

/**
 * See bt_na.h for definition.
 */
void bt_na_register_for_read(void *handle) {
    register_bt_socket_read(handle);
}

/**
 * See bt_na.h for definition.
 */
void bt_na_unregister_for_read(void *handle) {
    Unregister_bt_socket_read(handle);
}

/**
 * See bt_na.h for definition.
 */
void bt_na_register_for_write(void *handle) {
    register_bt_socket_write(handle);
}

/**
 * See bt_na.h for definition.
 */
void bt_na_unregister_for_write(void *handle) {
    Unregister_bt_socket_write(handle);
}

/**
 * See bt_na.h for definition.
 */
bt_result_t bt_na_get_status(void *handle) {
    bt_socket_status_t status;
    
    status = getBtSocketHandleStatus(handle);
    if (status == BT_SOCKET_STATUS_INTERRUPTED) {
        return BT_RESULT_INTERRUPTED;
    }

    return BT_RESULT_SUCCESS;
}


/**
 * See bt_na.h for definition.
 */
void bt_na_destroy(void *handle) {

    /*
     * Throw an interrupted IO Exception if this socket is already blocked
     * for read or write operation by any other thread
     */
    if (((QBTSocket*)handle)->readerCount > 0) {
        setBtSocketHandleStatus(handle, BT_SOCKET_STATUS_INTERRUPTED);
        /* Make an up-call to unblock the thread */
        NotifySocketStatusChanged((long)handle, SD_RECV);
    }
    if (((QBTSocket*)handle)->writerCount > 0) {
        setBtSocketHandleStatus(handle, BT_SOCKET_STATUS_INTERRUPTED);
        /* Make an up-call to unblock the thread */
        NotifySocketStatusChanged((long)handle, SD_SEND);
    }

    destroyBtSocketHandle(handle);
}

#ifdef __cplusplus
}
#endif
