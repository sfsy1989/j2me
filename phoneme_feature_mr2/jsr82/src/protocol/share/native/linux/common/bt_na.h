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

#ifndef _BT_NA_H_
#define _BT_NA_H_

#include <btCommon.h>

/**
 * @file
 * @defgroup jsr82notification Bluetooth Notification Adapter
 * @ingroup jsr82
 * @brief #include <bt_na.h>
 * @{
 *
 * Note: This interface duplicates <code>pcsl_network_na.h</code>.
 * We can not use PCSL version of the interface because it's private
 * and we also can not make this PCSL interface pubic since it is
 * BSD specific not platform independent.
 *
 * Notification adapter is an abstraction layer that implements certain
 * platform specific functions which are used internally by the generic BSD
 * implementation of Bluetooth networking interfaces. This layer is specifically
 * created to isolate the platform specific implementation from a generic
 * BSD implementation. So, for all BSD compatible platforms, only interfaces
 * defined in this file need to be ported without modifying the generic BSD
 * implementation of Bluetooth networking interfaces. Note that this abstraction
 * layer is transparent for generic BSD layer. Notification adapter object
 * is uniquely identified by a socket descriptor. A "handle" will actually
 * be a reference to this notification adapter object.
 *
 * This file declares notification adapter functions which should be
 * implemented for a particular platform. These functions are used by
 * btL2CAPConnection.c and btRFCOMMConnecton.c
 */



/**
 *  Note: We are guaranteed by Protocol Java code that there are no 2
 *  threads performing the same action on the socket, i.e. only one
 *  thread can be registered for reading, so well for writing.
 */

/**
  * @def SD_RECV
  *
  * Notify on a read.
  *
  * Used by <code>NotifySocketStatusChanged</code> and
  * <code>addBtSocketNotifier</code> functions.
  */
#define SD_RECV   0

/**
  * @def SD_SEND
  *
  * Notify on a write.
  *
  * Used by <code>NotifySocketStatusChanged</code> and
  * <code>addBtSocketNotifier</code> functions.
  */
#define SD_SEND   1

/**
  * @def SD_NONE
  *
  * Used by <code>addBtSocketNotifier</code> function.
  */
#define SD_NONE  -1

/**
  * @def SD_BOTH
  *
  * Used by <code>addBtSocketNotifier</code> function.
  */
#define SD_BOTH   2

#ifdef __cplusplus
extern "C" {
#endif

/**
  * We need this method to unblock threads waiting for socket being destroyed.
  *
  * Note: this method is implemented in MIDP workspace.
  */
void NotifySocketStatusChanged(long handle, int waitingFor);

/**
 * Creates a notification adapter object for this fd.
 * @param fd BSD socket descriptor
 *
 * @return Platform specific handle which is actually
 *        a reference pointer to this notification adapter object
 */
void* bt_na_create(int fd);

/**
 * Extracts a file descriptor from a handle
 * @param handle Platform specific handle that represents an
 *          open connection
 *
 * @return fd BSD Socket descriptor
 */
int bt_na_get_fd(void *handle);

/**
 * Requests a callback when <code>bt_*_read_start()</code>  occurs on this
 * handle AND it returns with <code>BT_RESULT_WOULDBLOCK</code>
 * @param handle Platform specific handle that represents an
 *          open connection
 */
void bt_na_register_for_read(void *handle);

/**
 * Requests a callback when <code>bt_*_write_start()</code> occurs on this
 * handle AND it returns with  <code>BT_RESULT_WOULDBLOCK</code>
 * @param handle Platform specific handle that represents an
 *          open connection
 */
void bt_na_register_for_write(void *handle);

/**
 * Cancels a callback requested for when a blocked read
 * activity is finished
 * @param handle Platform specific handle that represents an
 *          open connection
 */
void bt_na_unregister_for_read(void *handle);

/**
 * Cancels a callback requested for when a blocked write activity is finished.
 * @param handle Platform specific handle that represents an
 *          open connection
 */
void bt_na_unregister_for_write(void *handle);

/**
 * Checks the status of a notification adapter object for any pending
 * read or write operations
 * @param handle Platform specific handle that represents an
 *          open connection
 *
 * @return  <code>BT_RESULT_SUCCESS</code>
 *                          if operations are still allowed on this resource or
 *               <code> BT_RESULT_INTERRUPTED</code>
 *                          if the resource has been closed.
 */
bt_result_t bt_na_get_status(void *handle);

/**
 * Destroys the notification adapter for this handle. There is no explicit
 * need to unregister the handle before destroying it.
 * @param handle Platform specific handle that represents an
 *          open connection
 */
void bt_na_destroy(void *handle);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BT_NA_H_ */
