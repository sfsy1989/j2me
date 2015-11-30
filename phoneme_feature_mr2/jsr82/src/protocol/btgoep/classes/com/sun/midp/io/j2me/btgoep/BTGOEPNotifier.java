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
package com.sun.midp.io.j2me.btgoep;

import javax.microedition.io.StreamConnectionNotifier;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.Connection;
import com.sun.kvem.jsr082.obex.ObexTransportNotifier;
import com.sun.kvem.jsr082.obex.ObexTransport;
import java.io.IOException;
import javax.bluetooth.*;
import com.sun.kvem.jsr082.bluetooth.ServiceRecordImpl;

/**
 * Provides underlying stream notifier to shared obex implementation.
 */
public class BTGOEPNotifier implements ObexTransportNotifier {

    /** Keeps notifier for transport layer */
    private StreamConnectionNotifier notifier;

    /** Keeps OBEX UUID for service record construction. */
    static final DataElement DE_OBEX_UUID =
        new DataElement(DataElement.UUID, new UUID(0x0008)); 

    /**
     * Create BTGOEP Notifier
     * @param notifier notifier for transport layer
     * @exception IOException if an error occured while service record
     * creation
     */
    protected BTGOEPNotifier(StreamConnectionNotifier notifier)
        throws IOException {
        this.notifier = notifier;
        try {
            ServiceRecord servRec = LocalDevice.getLocalDevice().
                getRecord(notifier);
            DataElement protocolList = null;
            synchronized (servRec) {
                protocolList = servRec.getAttributeValue(
                            ServiceRecordImpl.PROTOCOL_DESCRIPTOR_LIST);
                DataElement p = new DataElement(DataElement.DATSEQ);
                p.addElement(DE_OBEX_UUID); // obex UUID
                protocolList.addElement(p);
                servRec.setAttributeValue(
                          ServiceRecordImpl.PROTOCOL_DESCRIPTOR_LIST,
                          protocolList);
            }
        }
        catch (BluetoothStateException e) {
            throw new IOException(e.getMessage());
        }
    }

    /**
     * Accepts client connection to the service this notifier is assigned to.
     *
     * @return connection to a client just accepted on transport layer.
     * @exception IOException if an error occured on transport layer. 
     */
    public ObexTransport acceptAndOpen() throws IOException {
        return createTransportConnection(
                (StreamConnection)(notifier.acceptAndOpen()));
    }

    /**
     * Closes this notifier on the transport layer
     * @exception IOException if an error occured on transport layer
     */
    public void close() throws IOException {
        notifier.close();
    }

    /**
     * Create btgoep transport connection.
     * @param sock transport connection
     * @return BTGOEP Connection 
     */
    protected BTGOEPConnection createTransportConnection(
            StreamConnection sock) throws IOException {
        return new BTGOEPConnection(sock);
    }

    /**
     * Get transport connection noifier
     * @return transport notifier 
     */
    public Connection getUnderlyingConnection() {
        return notifier;
    }
}
