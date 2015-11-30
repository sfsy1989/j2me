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
package com.sun.kvem.jsr082.bluetooth;
import javax.bluetooth.DataElement;
import javax.bluetooth.L2CAPConnection;
import javax.bluetooth.UUID;
import java.util.Enumeration;
import java.util.Hashtable;
import java.io.IOException;

/**
 * SDPClient class provides a client side of SDP connection as described in
 * Bluetooth Specification version 1.2.
 */
class SDPClient {

    /** Max retrievable service record handles. Maybe defined via property. */
    private static final int MAX_SERVICE_RECORD_COUNT = 0x0fff;

    /** Max total size of retrievable attributes. Maybe defined via property. */
    private static final int MAX_ATTRIBUTE_BYTE_COUNT = 0xffff;

    /**
     * The lowest possible value of transaction ID.
     * The number must be positive.
     */
    private static final int firstTransactionID = 0x0001;

    /** The maximum possible value of transaction ID. */
    private static final int maxTransactionID = 0xffff;

    /** Next transaction ID. */
    private static int effectiveTransactionID = firstTransactionID;

    /** Maps Bluetooth addresses to transport connections. */
    private static Hashtable transportStorage = new Hashtable();

    /** Transport connection for this client. */
    private SDPTransport transport;

    /** Bluetooth address this client is connected to. */
    private String address = null;

    /**
     * Constructs an <code>SDPClient<code> object and opens SDP connection
     * to the remote device with the specified Bluetooth address.
     *
     * @param bluetoothAddress bluetooth address of SDP server
     */
    SDPClient(String bluetoothAddress) throws IOException {
        address = bluetoothAddress;
        synchronized (transportStorage) {
            transport = (SDPTransport)transportStorage.get(address);
            if (transport == null) {
                transport = new SDPTransport(address);
                transportStorage.put(address, transport);
            }
        }
        transport.addRef();
    }

    /**
     * Retrieves next new transaction ID.
     *
     * @return new transaction ID.
     */
    static synchronized short newTransactionID() {
        int transactionID = effectiveTransactionID++;
        if (effectiveTransactionID > maxTransactionID) {
            // strictly speaking, this is not quite safe,
            // need revisit : if we have a pending
            // transaction after 64K of subsequent calls
            effectiveTransactionID = firstTransactionID;
        }
        return (short)transactionID;
    }

    /**
     * Frees transaction ID.
     *
     * @param transactionID the ID to free.
     */
    static synchronized void freeTransactionID(short transactionID) {
        // empty in this implementation
    }

    /**
     * Closes connection of this client to the specified server.
     *
     * @throws IOException if no connection is open
     */
    void close() throws IOException {
        if (transport == null) {
            throw new IOException("Connection is already closed.");
        }
        transport.release();
        transport = null;
    }

    /**
     * Initiates ServiceSearch transaction that is used to search for
     * services that have all the UUIDs specified on a server.
     */
    void serviceSearchRequest(UUID[] uuidSet, int transactionID,
        SDPResponseListener listener) throws IOException {
        try {
            transport.serviceSearchRequest(uuidSet, transactionID, listener);
        } catch (IOException ioe) {
            transport.cancelAll(SDPResponseListener.IO_ERROR);
            throw ioe;
        }
    }

    /**
     * Initiates ServiceAttribute transaction that retrieves
     * specified attribute values from a specific service record.
     */
    void serviceAttributeRequest(int serviceRecordHandle, int[] attrSet,
        int transactionID, SDPResponseListener listener) throws IOException {
        try {
            transport.serviceAttributeRequest(serviceRecordHandle, attrSet,
                transactionID, listener);
        } catch (IOException e) {
            transport.cancelAll(SDPResponseListener.IO_ERROR);
            throw e;
        }
    }

    /**
     * Initiates ServiceSearchAttribute transaction that searches for services
     * on a server by UUIDs specified and retrieves values of specified
     * parameters for service records found.
     */
    void serviceSearchAttributeRequest(int[] attrSet, UUID[] uuidSet,
        int transactionID, SDPResponseListener listener) throws IOException {
        try {
            transport.serviceSearchAttributeRequest(attrSet, uuidSet,
                transactionID, listener);
        } catch (IOException e) {
            transport.cancelAll(SDPResponseListener.IO_ERROR);
            throw e;
        }
    }

    /**
     * Cancels transaction with given ID.
     */
    boolean cancelServiceSearch(int transactionID) {
        return transport.cancelServiceSearch(transactionID);
    }

    /**
     * SDP transport connection.
     */
    private static class SDPTransport {

        /** Bluetooth address of the server. */
        private String address;

        /**
         * Reference counter keeps the number of SDP connections which
         * use this transport. When this value reaches zero, the L2CAP
         * connection is closed and the transport is removed from the global
         * SDPClient.transportStorage hashtable.
         */
        private int refCount = 0;

        /** The L2CAP (logical link) connection. */
        private L2CAPConnection connection;

        /**
         * Object that performs reading from and writing to L2CAP connection.
         */
        private DataL2CAPReaderWriter rw;

        /** Maps transaction IDs to ServiceTransaction objects. */
        private Hashtable transactions = new Hashtable();

        /** Response receiver. */
        private Receiver receiver = new Receiver();

        /** Lock for synchronizing reading from connection. */
        private Object readLock = new Object();

        /** Lock for synchronizing writing to connection. */
        private Object writeLock = new Object();

        /**
         * Helper object which serializes and restores
         * <code>DataElement</code>s.
         */
        private static DataElementSerializer des = new DataElementSerializer();

        /** ID of SDP_ErrorResponse protocol data unit. */
        private static final int SDP_ERROR_RESPONSE = 0x01;

        /** ID of SDP_ServiceSearchRequest protocol data unit. */
        private static final int SDP_SERVICE_SEARCH_REQUEST = 0x02;

        /** ID of SDP_ServiceSearchResponse protocol data unit. */
        private static final int SDP_SERVICE_SEARCH_RESPONSE = 0x03;

        /** ID of SDP_ServiceAttributeRequest protocol data unit. */
        private static final int SDP_SERVICE_ATTRIBUTE_REQUEST = 0x04;

        /** ID of SDP_ServiceAttributeResponse protocol data unit. */
        private static final int SDP_SERVICE_ATTRIBUTE_RESPONSE = 0x05;

        /** ID of SDP_ServiceSearchAttributeRequest protocol data unit. */
        private static final int SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST = 0x06;

        /** ID of SDP_ServiceSearchAttributeResponse protocol data unit. */
        private static final int SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE = 0x07;

        /** L2CAP URL starting string. */
        private static final String SDP_L2CAP_URL_BEGIN = "//";

        /** L2CAP URL trailing string. */
        private static final String SDP_L2CAP_URL_END = ":0001";

        /**
         * Constricts <code>SDPTransport</code> instance that reflects transport
         * connections to the specified server.
         *
         * @param bluetoothAddress bluetooth address of the server
         */
        private SDPTransport(String bluetoothAddress) throws IOException {
            address = bluetoothAddress;
            connection = (L2CAPConnection)SDP.getL2CAPConnection(
                SDP_L2CAP_URL_BEGIN + bluetoothAddress + SDP_L2CAP_URL_END);
            rw = new DataL2CAPReaderWriter(connection);
        }

        /**
         * Increases reference counter. This object and the underlying L2CAP
         * connection will live while the counter is positive.
         */
        private synchronized void addRef() {
            refCount++;
        }

        /**
         * Decreases reference counter. If the counter becomes equal to zero,
         * L2CAP connection is closed and the transport is removed from the
         * global SDPClient.transportStorage hashtable.
         */
        private synchronized void release() {
            refCount--;
            if (refCount == 0) {
                try {
                    connection.close();
                } catch (IOException e) {
                    // just ignore it, we're done with this object anyway
                }
                receiver.cancel();
                synchronized (transportStorage) {
                    transportStorage.remove(address);
                }
            }
        }

        /**
         * Cancels a transaction with specified ID.
         */
        boolean cancelTransaction(int transactionID, int reason) {
            // key for transactions hastable is effectiveTransactionID not
            // transactionID so we need to iterate over the hashatble looking
            // for the transactionID
            Enumeration e = transactions.elements();
            while (e.hasMoreElements()) {
                ServiceTransaction trans = (ServiceTransaction)e.nextElement();
                if (trans != null && trans.transactionID == transactionID) {
                    transactions.remove(
                        new Integer(trans.effectiveTransactionID));
                    // the transaction is removed from transactions hashtable by
                    // ServiceTransaction.end() called from
                    // ServiceTransaction.cancel(). The cancel()  reports to the
                    // listener what transaction is terminated.
                    trans.cancel(reason);
                    return true;
                }
            }
            return false;
        }

        /**
         * Cancels all current transactions. Called in case of I/O failure
         * in the underlying L2CAP connection.
         */
        synchronized void cancelAll(int reason) {
            // force actual release of resources
            refCount = 1;
            release();
            Enumeration e = transactions.elements();
            while (e.hasMoreElements()) {
                ServiceTransaction trans = (ServiceTransaction)e.nextElement();
                trans.listener.errorResponse(reason, "",
                    trans.transactionID);
            }
            transactions.clear();
        }

        /**
         * Starts ServiceSearch transaction.
         */
        void serviceSearchRequest(UUID[] uuidSet, int transactionID,
                SDPResponseListener listener) throws IOException {
            new ServiceSearchTransaction(transactionID, listener,
                    uuidSet).start();
        }

        /**
         * Starts ServiceAttribute transaction.
         */
        void serviceAttributeRequest(int serviceRecordHandle,
                int[] attrSet, int transactionID, SDPResponseListener listener)
                throws IOException {
            new ServiceAttributeTransaction(transactionID, listener,
                    serviceRecordHandle, attrSet).start();
        }

        /**
         * Starts ServiceSearchAttribute transaction.
         */
        void serviceSearchAttributeRequest(int[] attrSet, UUID[] uuidSet,
                int transactionID, SDPResponseListener listener)
                throws IOException {
            new ServiceSearchAttributeTransaction(transactionID, listener,
                    attrSet, uuidSet).start();
        }

        /**
         * Cancels a transaction with specified ID with reason
         * <code>SDPResponseListener.TERMINATED</code>.
         */
        boolean cancelServiceSearch(int transactionID) {
            return cancelTransaction(transactionID,
                    SDPResponseListener.TERMINATED);
        }

        /**
         * Gets next SDP server response, if any, and passes it to the
         * corresponding listener. If a response is received, the transaction
         * it belongs to is stopped.
         *
         * @throws IOException if an I/O error occurs
         */
        void processResponse() throws IOException {
            ServiceTransaction trans;
            synchronized (readLock) {
                byte pduID = rw.readByte();
                short transID = rw.readShort();
                short length = rw.readShort();
                // System.out.println("received pdu " + pduID +
                //         " trans " + transID);

                trans = (ServiceTransaction)transactions.get(
                        new Integer(transID));

                if (trans == null) {
                    // System.out.println("not found");
                    // transaction we are not aware of; skip this pdu
                    rw.readBytes(length);
                    return;
                }
                if (pduID == SDP_ERROR_RESPONSE) {
                    trans.error(length);
                    return;
                }
                trans.readParameters(length);
            }
            trans.process();
        }

        /**
         * SDP response receiver.
         */
        private class Receiver implements Runnable {
            /** A counter to identify if the receiver has clients. */
            private int startCounter = 0;

            /** Identifies if receiving is cancelled. */
            private boolean canceled = false;

            /**
             * Identifies if receiving thread is running (false) or not (true).
             */
            private boolean stopped = true;

            /**
             * Starts receiving thread if it is not running.
             */
            synchronized void start() {
                if (startCounter == 0) {
                    if (stopped) {
                        stopped = false;
                        new Thread(this).start();
                    }
                }
                startCounter++;
            }

            /** Decrements number of receiving thread clients. */
            synchronized void stop() {
                startCounter--;
            }

            /** Cancels receiving responses. */
            synchronized void cancel() {
                canceled = true;
            }

            /**
             * The <code>run()</code> method.
             *
             * @see java.lang.Runnable
             */
            public void run() {
                while (true) {
                    synchronized (this) {
                        if ((startCounter <= 0) || canceled) {
                            startCounter = 0;
                            stopped = true;
                            break;
                        }
                    }
                    try {
                        processResponse();
                    } catch (IOException ioe) {
                        synchronized (this) {
                            if (startCounter <= 0 || canceled) {
                                startCounter = 0;
                                stopped = true;
                                break;
                            }
                        }
                        cancelAll(SDPResponseListener.IO_ERROR);
                    }
                }
            }
        }

        /**
         * This abstract class provides base functionality for all SDP
         * transactions.
         */
        private abstract class ServiceTransaction {

            /** PDU ID (see Bluetooth Specification 1.2 Vol 3 page 131) */
            byte pduID;
            /** Transcation ID used to identify this transaction. */
            int transactionID;
            /** Effective transaction ID. */
            int effectiveTransactionID;
            /** Length of all parameters. */
            long parameterLength;
            /** Continuation state used with partial responses. */
            byte[] continuationState;
            /** Listener to report request result to. */
            SDPResponseListener listener;

            /**
             * Class constructor.
             *
             * @param pduID protocol data unit ID
             * @param transactionID transaction ID of the first request
             * @param listener listener object which will receive
             *                 completion and error notifications
             */
            ServiceTransaction(int pduID, int transactionID,
                SDPResponseListener listener) {
                this.pduID = (byte)pduID;
                this.transactionID = transactionID;
                effectiveTransactionID = newTransactionID();
                this.listener = listener;
            }

            /**
             * Updates the effective transaction ID with a new value.
             */
            final void updateTransactionID() {
                transactions.remove(new Integer(effectiveTransactionID));
                effectiveTransactionID = newTransactionID();
                transactions.put(new Integer(effectiveTransactionID), this);
            }

            /**
             * Starts this transaction.
             *
             * @throws IOException when an I/O error occurs
             */
            final void start() throws IOException {
                addRef();
                receiver.start();

                transactions.put(new Integer(effectiveTransactionID), this);

                try {
                    synchronized (writeLock) {
                        rw.writeByte(pduID);
                        rw.writeShort((short)effectiveTransactionID);
                        rw.writeShort((short)(parameterLength + 1));
                        writeParameters();
                        rw.writeByte((byte)0x00);
                        rw.flush();
                    }
                } catch (IOException e) {
                    end();
                    throw e;
                }
            }

            /**
             * Re-submits the original request with continuation state
             * data received with the incomplete response.
             *
             * @throws IOException when an I/O error occurs
             */
            final void proceed() throws IOException {
                updateTransactionID();
                synchronized (writeLock) {
                    rw.writeByte(pduID);
                    rw.writeShort((short)effectiveTransactionID);
                    rw.writeShort((short)(parameterLength + 1 +
                            continuationState.length));
                    writeParameters();
                    rw.writeByte((byte)continuationState.length);
                    rw.writeBytes(continuationState);
                    rw.flush();
                }
            }

            /**
             * Extracts continuation state parameter.
             *
             * @return true if the continuation state is present,
             *         false otherwise
             * @throws IOException when an I/O error occurs
             */
            final boolean readContinuationState() throws IOException {
                byte infoLength = rw.readByte();
                if (infoLength == 0) {
                    continuationState = null;
                    return false;
                }
                continuationState = rw.readBytes(infoLength);
                return true;
            }

            /**
             * Terminates this transaction and reports error to the listener.
             *
             * @param reason error code which will be reported
             */
            final void cancel(int reason) {
                end();
                listener.errorResponse(reason, "", transactionID);
            }

            /**
             * Ends this transaction by unregistering it in the outer class.
             */
            final void end() {
                transactions.remove(new Integer(effectiveTransactionID));
                receiver.stop();
                release();
            }

            /**
             * Reads error PDU, ends this transaction and reports listener the
             * error code retrieved.
             *
             * @param length length of PDU's parameters
             */
            final void error(int length) throws IOException {
                short errorCode = rw.readShort();
                byte[] infoBytes = rw.readBytes(length - 2);
                end();
                listener.errorResponse(errorCode, new String(infoBytes),
                        transactionID);
            }

            /**
             * Processes this transaction by either re-submitting the original
             * request if the last response was incomplete, or providing the
             * listener with the results if the transaction was completed.
             *
             * @throws IOException when an I/O error occurs
             */
            final void process() throws IOException {
                if (continuationState != null) {
                    try {
                        proceed();
                    } catch (IOException e) {
                        end();
                        throw e;
                    }
                } else {
                    end();
                    complete();
                }
            }

            /**
             * Writes transaction-specific parameters into the PDU.
             *
             * @throws IOException when an I/O error occurs
             */
            abstract void writeParameters() throws IOException;

            /**
             * Reads transaction-specific parameters from the PDU.
             *
             * @param length length of PDU's parameters
             * @throws IOException when an I/O error occurs
             */
            abstract void readParameters(int length) throws IOException;

            /**
             * Completes the transaction by calling corresponding listener's
             * method with the data retrieved.
             */
            abstract void complete();

        }

        /**
         * Provides ServiceSearch transaction functionality.
         */
        class ServiceSearchTransaction extends ServiceTransaction {

            /** ServiceSearchPattern (BT Spec 1.2 Vol 3 page 135). */
            DataElement serviceSearchPattern;
            /** Acquired service record handles. */
            int[] handleList;
            /** Current position in the handleList. */
            int offset;

            /**
             * Constructs ServiceSearchTransaction object.
             */
            ServiceSearchTransaction(int transactionID,
                    SDPResponseListener listener, UUID[] uuidSet) {
                super(SDP_SERVICE_SEARCH_REQUEST, transactionID, listener);
                serviceSearchPattern = new DataElement(DataElement.DATSEQ);
                for (int i = 0; i < uuidSet.length; i++) {
                    serviceSearchPattern.addElement(new DataElement(
                            DataElement.UUID, uuidSet[i]));
                }
                parameterLength = rw.getDataSize(serviceSearchPattern) + 2;
            }

            /**
             * Writes transaction-specific parameters into the PDU.
             *
             * @throws IOException when an I/O error occurs
             */
            void writeParameters() throws IOException {
                rw.writeDataElement(serviceSearchPattern);
                rw.writeShort((short)MAX_SERVICE_RECORD_COUNT);
            }

            /**
             * Reads transaction-specific parameters from the PDU.
             *
             * @param length length of PDU's parameters
             * @throws IOException when an I/O error occurs
             */
            void readParameters(int length) throws IOException {
                int totalServiceRecordCount = rw.readShort();
                int currentServiceRecordCount = rw.readShort();
                if (handleList == null && totalServiceRecordCount > 0) {
                    handleList = new int[totalServiceRecordCount];
                }
                for (int i = 0; i < currentServiceRecordCount; i++) {
                    handleList[offset] = rw.readInteger();
                    offset++;
                }
                readContinuationState();
            }

            /**
             * Completes the transaction by calling corresponding listener's
             * method with the data retrieved.
             */
            void complete() {
                listener.serviceSearchResponse(handleList,
                        transactionID);
            }
        }

        /**
         * Provides ServiceAttribute transaction functionality.
         */
        class ServiceAttributeTransaction extends ServiceTransaction {

            /** ServiceRecordHandle (BT Spec 1.2 Vol 3 page 138). */
            int serviceRecordHandle;
            /** AttributeIDList (BT Spec 1.2 Vol 3 page 139). */
            DataElement attributeIDList;
            /** AttributeList (BT Spec 1.2 Vol 3 page 140). */
            byte[] attributes;

            /**
             * Constructs ServiceAttributeTransaction object.
             */
            ServiceAttributeTransaction(int transactionID,
                    SDPResponseListener listener, int recordHandle,
                    int[] attrSet) {
                super(SDP_SERVICE_ATTRIBUTE_REQUEST, transactionID, listener);
                serviceRecordHandle = recordHandle;
                attributeIDList = new DataElement(DataElement.DATSEQ);
                for (int i = 0; i < attrSet.length; i++) {
                    attributeIDList.addElement(new DataElement(
                            DataElement.U_INT_2, attrSet[i]));
                }
                parameterLength = rw.getDataSize(attributeIDList) + 6;
            }

            /**
             * Writes transaction-specific parameters into the PDU.
             *
             * @throws IOException when an I/O error occurs
             */
            void writeParameters() throws IOException {
                rw.writeInteger(serviceRecordHandle);
                rw.writeShort((short)MAX_ATTRIBUTE_BYTE_COUNT);
                rw.writeDataElement(attributeIDList);
            }

            /**
             * Reads transaction-specific parameters from the PDU.
             *
             * @param length length of PDU's parameters
             * @throws IOException when an I/O error occurs
             */
            void readParameters(int length) throws IOException {
                short byteCount = rw.readShort();
                byte[] data = rw.readBytes(byteCount);
                if (attributes == null) {
                    attributes = data;
                } else {
                    byte[] temp = attributes;
                    attributes = new byte[temp.length + byteCount];
                    System.arraycopy(temp, 0, attributes, 0, temp.length);
                    System.arraycopy(data, 0, attributes, temp.length,
                            byteCount);
                }
                readContinuationState();
            }

            /**
             * Completes the transaction by calling corresponding listener's
             * method with the data retrieved.
             */
            void complete() {
                DataElement attrList;
                try {
                    attrList = des.restore(attributes);
                } catch (IOException e) {
                    listener.serviceAttributeResponse(null, null,
                            transactionID);
                    return;
                }
                int size = attrList.getSize() / 2;
                if (size == 0) {
                    listener.serviceAttributeResponse(null, null,
                            transactionID);
                    return;
                }
                Enumeration elements = (Enumeration)attrList.getValue();
                int[] attrIDs = new int[size];
                DataElement[] attrValues = new DataElement[size];
                for (int i = 0; elements.hasMoreElements(); i++) {
                    attrIDs[i] = (int)((DataElement)
                            elements.nextElement()).getLong();
                    attrValues[i] = ((DataElement)
                            elements.nextElement());
                }
                listener.serviceAttributeResponse(attrIDs, attrValues,
                        transactionID);
            }
        }

        /**
         * Provides ServiceSearchAttribute transaction functionality.
         */
        class ServiceSearchAttributeTransaction extends ServiceTransaction {

            /** ServiceSearchPattern (BT Spec 1.2 Vol 3 page 142). */
            DataElement uuidData;
            /** AttributeIDList (BT Spec 1.2 Vol 3 page 142). */
            DataElement attrData;
            /** AttributeLists (BT Spec 1.2 Vol 3 page 143). */
            byte[] attributes;

            /**
             * Constructs ServiceSearchAttributeTransaction object.
             */
            ServiceSearchAttributeTransaction(int transactionID,
                    SDPResponseListener listener, int[] attrSet,
                    UUID[] uuidSet) {
                super(SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST, transactionID,
                        listener);
                attrData = new DataElement(DataElement.DATSEQ);
                uuidData = new DataElement(DataElement.DATSEQ);
                for (int i = 0; i < attrSet.length; i++) {
                    attrData.addElement(new DataElement(DataElement.U_INT_2,
                            attrSet[i]));
                }
                for (int i = 0; i < uuidSet.length; i++) {
                    uuidData.addElement(new DataElement(DataElement.UUID,
                            uuidSet[i]));
                }
                parameterLength = rw.getDataSize(attrData) +
                        rw.getDataSize(uuidData) + 2;
            }

            /**
             * Writes transaction-specific parameters into the PDU.
             *
             * @throws IOException when an I/O error occurs
             */
            void writeParameters() throws IOException {
                rw.writeDataElement(uuidData);
                rw.writeShort((short)MAX_ATTRIBUTE_BYTE_COUNT);
                rw.writeDataElement(attrData);
            }

            /**
             * Reads transaction-specific parameters from the PDU.
             *
             * @param length length of PDU's parameters
             * @throws IOException when an I/O error occurs
             */
            void readParameters(int length) throws IOException {
                short byteCount = rw.readShort();
                byte[] data = rw.readBytes(byteCount);
                if (attributes == null) {
                    attributes = data;
                } else {
                    byte[] temp = attributes;
                    attributes = new byte[temp.length + byteCount];
                    System.arraycopy(temp, 0, attributes, 0, temp.length);
                    System.arraycopy(data, 0, attributes, temp.length,
                                     byteCount);
                }
                readContinuationState();
            }

            /**
             * Completes the transaction by calling corresponding listener's
             * method with the data retrieved.
             */
            void complete() {
                DataElement attrList;
                try {
                    Enumeration attributeLists = (Enumeration)des.
                            restore(attributes).getValue();
                    if (attributeLists.hasMoreElements()) {
                        attrList = (DataElement)attributeLists.nextElement();
                    } else {
                        listener.serviceSearchAttributeResponse(null, null,
                                transactionID);
                        return;
                    }
                } catch (IOException e) {
                    listener.serviceSearchAttributeResponse(null, null,
                            transactionID);
                    return;
                }
                int size = attrList.getSize() / 2;
                if (size == 0) {
                    listener.serviceSearchAttributeResponse(null, null,
                            transactionID);
                    return;
                }
                Enumeration elements = (Enumeration)attrList.getValue();
                int[] attrIDs = new int[size];
                DataElement[] attrValues = new DataElement[size];
                for (int i = 0; elements.hasMoreElements(); i++) {
                    attrIDs[i] = (int)((DataElement)
                            elements.nextElement()).getLong();
                    attrValues[i] = ((DataElement)
                            elements.nextElement());
                }
                listener.serviceSearchAttributeResponse(attrIDs, attrValues,
                        transactionID);
            }
        }

    }
}
