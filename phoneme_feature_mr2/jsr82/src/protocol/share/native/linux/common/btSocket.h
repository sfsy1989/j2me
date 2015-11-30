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

#ifndef __BT_SOCKET_H__
#define __BT_SOCKET_H__

/**
 * @file
 * @defgroup jsr82socket BSD sockets declarations
 * @ingroup jsr82common
 * @brief #include <btSocket.h>
 * @{
 */

/**
 * @def SOCKET_ERROR
 * Defines error status for socket operation.
 *
 * IMPL_NOTE: Use SOCKET_ERROR from socketProtocol.h when it will be there.
 */
#ifndef SOCKET_ERROR
#define SOCKET_ERROR   (-1)
#endif

/**
 * @def SOCKET_OK
 * Defines success socket operation status.
 */
#ifndef SOCKET_OK
#define SOCKET_OK      (0)
#endif

/**
 * @def INVALID_SOCKET
 * Defines invalid socket handle.
 */
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif


/** @} */

#endif /* __BT_SOCKET_H__ */
