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

#ifndef __BT_NOTIFY_H_
#define __BT_NOTIFY_H_

#include <btCommon.h>

/**
 * @file
 * @defgroup jsr82notify Notification functions for L2CAP and RFCOMM protocols
 * @ingroup jsr82protocol
 * @brief #include <btNotify.h>
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A callback function to be called for notification of non-blocking
 * protocol related events.
 * The platform will invoke the call back in platform context for
 * connection related occurrence.
 *
 * @param type type of indication: Either
 *     - <code>BT_EVENT_CONNECT_COMPLETE</code>
 *     - <code>BT_EVENT_ACCEPT_COMPLETE</code>
 *     - <code>BT_EVENT_SEND_COMPLETE</code>
 *     - <code>BT_EVENT_RECEIVE_COMPLETE</code>
 *
 * @param handle related to the notification
 * @param operation_result operation result: Either
 *      - <code>BT_RESULT_OK</code> if operation completed successfully,
 *      - otherwise, <code>BT_RESULT_FAILURE</code>
 */
void bt_notify_protocol_event(bt_callback_type_t type, bt_handle_t handle,
  bt_result_t operation_result);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __BT_NOTIFY_H_ */
