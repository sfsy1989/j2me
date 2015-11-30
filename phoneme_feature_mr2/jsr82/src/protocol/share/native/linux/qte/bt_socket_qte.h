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

#ifndef _BT_SOCKET_QTE_H_
#define _BT_SOCKET_QTE_H_

#include <qsocket.h>
#include <qsocketnotifier.h>

/**
 * @file
 *
 * This file defines the QBTSocket class and methods to associate 
 * bluetooth events with the main Qt event loop.
 * It also defines QT interfaces that would be used by client sockets, 
 * as well as server sockets. The QBTSocket object resides as a part of JSR-82
 * and the interfaces would be used by client sockets(which are part
 * of JSR-82) as well as server sockets and pushregistry 
 * (which would be a part of MIDP)
 */

typedef enum {
    BT_SOCKET_STATUS_IDLE = 0,
    BT_SOCKET_STATUS_PENDING_READ,
    BT_SOCKET_STATUS_PENDING_WRITE,
    BT_SOCKET_STATUS_INTERRUPTED
} bt_socket_status_t;

class QBTSocket : public QObject
{
    Q_OBJECT

public:
    QBTSocket(int fd, QObject *parent=0, const char* name=0);
    ~QBTSocket();

    QSocketNotifier* getReadNotifier()  { return readNotifier; }
    QSocketNotifier* getWriteNotifier() { return writeNotifier; }
    int              getSocketFD()      { return socketFD; }
    
    bt_socket_status_t socketStatus;
    
    /** No of readers that are blocked for read operation to this socket. */
    int readerCount; 
    
    /** No of writers that are blocked for write operation to this socket. */
    int writerCount; 
 
public slots:
    void readableSlot(int);
    void writableSlot(int);

protected:
    QSocketNotifier* readNotifier;
    QSocketNotifier* writeNotifier;
    int socketFD;
};


/**
 * Return the QBTSocket status
 *
 * @param handle platform-specific handle to the open connection
 *
 * @return bt_socket_status_t 
 */
bt_socket_status_t getBtSocketHandleStatus(void *handle); 

/**
 * Set the QBTSocket status
 *
 * @param handle platform-specific handle to the open connection
 * @param bt_socket_status_t
 *
 */
void setBtSocketHandleStatus(void *handle, bt_socket_status_t status);

/**
 * Registers a QBTSocket for read. 
 * @param handle platform-specific handle to the open connection
 */
void register_bt_socket_read(void *handle); 

/**
 * Unregisters a QBTSocket for read. 
 * @param handle platform-specific handle to the open connection
 */
void Unregister_bt_socket_read(void *handle);

/**
 * Registers a QBTSocket for write. 
 * @param handle platform-specific handle to the open connection
 */
void register_bt_socket_write(void *handle);

/**
 * Unregisters a QBTSocket for write. 
 * @param handle platform-specific handle to the open connection
 */
void Unregister_bt_socket_write(void *handle); 

/**
 * Gets the platform-specific file descriptor associated with the
 * given handle.
 *
 * @param handle platform-specific handle to the open connection
 *
 * @return the associated platform-specific file descriptor;
 *         <code>INVALID_SOCKET</code> if there was an error
 */
int getBtSocketHandleFD(void *handle);

/**
 * Creates a platform-specific handle.
 *
 * @param fd platform-specific file descriptor to be associated with
 *           the new handle
 *
 * @return the platform-specific handle; <code>NULL</code> if there was an error
 */
void* createBtSocketHandle(int fd);

/**
 * Destroys a platform-specific handle and releases any resources used
 * by the handle.
 *
 * @param handle platform-specific handle to destroy
 */
void destroyBtSocketHandle(void *handle);


/**
 * Registers a notification event with the platform-specific event loop.
 * Whenever network activity can occur in the given direction without
 * causing the system to block, a network event is posted to the event
 * queue.
 *  
 * @param handle platform-specific handle to the open connection
 * @param direction whether to notify on a read, write, or both; the value
 *                  of this parameter must be one of <code>SD_RECV</code> (for
 *                  reading), <code>SD_SEND</code> (for writing), or 
 *                  <code>SD_BOTH</code> (for both reading and writing)
 */
void addBtSocketNotifier(void *handle, int direction); 
 
#endif /* _BT_SOCKET_QTE_H_ */
