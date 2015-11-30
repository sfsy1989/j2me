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
 * 
 * This file implements the bluetooth notification adapter functions for a 
 * generic BSD platform
 */

#include <bt_generic.h>
#include <bt_na.h>
#include <pcsl_memory.h>

#define SOCKET_HANDLE(handle)   ((SocketHandle*)handle)

/** List of socket handles registered for read/write checks */
static SocketHandle* rootSocketHandle = NULL;

/** Search for handle instance in the sockets list and return its reference */
static SocketHandle** getSocketHandleReference(SocketHandle* handle) {
    SocketHandle** ptr;
    for (ptr = &rootSocketHandle; *ptr != NULL; ptr = &((*ptr)->next)) {
        if (*ptr == handle) return ptr;
    }
    return NULL;
}

/** Add handle to the list */
static void addSocketHandle(SocketHandle* handle) {
    SocketHandle **ptr = getSocketHandleReference(handle);
    if (ptr == NULL) {
        handle->next = rootSocketHandle;
        rootSocketHandle = handle;
    }
}

/** Remove handle from the list */
static void removeSocketHandle(SocketHandle* handle) {
    SocketHandle **ptr = getSocketHandleReference(handle);
    if (ptr != NULL) {
        *ptr = handle->next;
    }
}

/**
 * See bt_generic.h for definition.
 */
const SocketHandle* GetRegisteredBtSocketHandles() {
    return (const SocketHandle*)rootSocketHandle;
}

/**
 * See bt_na.h for definition.
 */
void* bt_na_create(int fd) {
    SocketHandle* handle =
        (SocketHandle*)pcsl_mem_malloc(sizeof(SocketHandle));

    if (handle != NULL) {
        handle->fd = fd;
        handle->check_flags = 0;
        handle->status = BT_RESULT_SUCCESS;
        handle->next = NULL;
    }

    return (void*)handle;
}

/**
 * See bt_na.h for definition.
 */
int bt_na_get_fd(void *handle) {
    if (handle != NULL) {
        return SOCKET_HANDLE(handle)->fd;
    }
    return -1;
}

/**
 * See bt_na.h for definition.
 */
void bt_na_register_for_read(void *handle) {
    if (handle != NULL) {
        SocketHandle *sh = SOCKET_HANDLE(handle);
        sh->check_flags |= CHECK_READ;
        addSocketHandle(sh);
    }
}

/**
 * See bt_na.h for definition.
 */
void bt_na_register_for_write(void *handle) {
    if (handle != NULL) {
        SocketHandle *sh = SOCKET_HANDLE(handle);
        sh->check_flags |= CHECK_WRITE;
        addSocketHandle(sh);
    }
}

/**
 * See bt_na.h for definition.
 */
void bt_na_unregister_for_read(void *handle) {
    if (handle != NULL) {
        SocketHandle *sh = SOCKET_HANDLE(handle);
        sh->check_flags &= ~CHECK_READ;
        if (sh->check_flags == 0) {
            removeSocketHandle(sh);
        }
    }
}

/**
 * See bt_na.h for definition.
 */
void bt_na_unregister_for_write(void *handle) {
    if (handle != NULL) {
        SocketHandle *sh = SOCKET_HANDLE(handle);
        sh->check_flags &= ~CHECK_WRITE;
        if (sh->check_flags == 0) {
            removeSocketHandle(sh);
        }
    }
}

/**
 * See bt_na.h for definition.
 */
bt_result_t bt_na_get_status(void *handle) {
    if (handle != NULL) {
        return SOCKET_HANDLE(handle)->status;
    }
    return BT_RESULT_INTERRUPTED;
}

/**
 * See bt_na.h for definition.
 */
void bt_na_destroy(void *handle) {
    if (handle != NULL) {
        SocketHandle *sh = SOCKET_HANDLE(handle);
        removeSocketHandle(SOCKET_HANDLE(handle));

        /* Still registered readers/writers should be unblocked */
        if (sh->check_flags != 0) {
            sh->status = BT_RESULT_INTERRUPTED;
            if (sh->check_flags & CHECK_READ) {
                NotifySocketStatusChanged((long)handle, SD_RECV);
            }
            if (sh->check_flags & CHECK_WRITE) {
                NotifySocketStatusChanged((long)handle, SD_SEND);
            }
        }
        /* It's up to caller to guarantee this handle will be used no more */
        pcsl_mem_free(handle);
    }
}
