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

import javax.bluetooth.RemoteDevice;
import javax.bluetooth.UUID;
import javax.bluetooth.DataElement;
import java.util.Enumeration;
import java.util.Hashtable;

/**
 * This class saves information about every service descovery request
 * to provide functionality of DiscoveryAgent using multiple requests
 * via SDPClient (Service Descovery Protocol) by ServiceSelector
 * and ServiceSearcher classes.
 */
abstract class ServiceSearcherBase implements SDPResponseListener {
    /** Mask to determine an attribute ID out of range. */
    private static final int MASK_OVERFLOW = 0xffff0000;

    /** RemoteDevice whose response to be listened. */
    RemoteDevice btDev;

    /**
     * The UUIDs from SDP_ServiceSearchRequest or
     * SDP_ServiceSearchAttrbuteRequest.
     *
     * @see SDPClient#serviceSearchRequest
     * @see SDPClient#serviceSearchAttributeRequest
     */
    UUID[] uuidSet;

    /**
     * Attributes list from SDP_ServiceSearchAttrbuteRequest.
     *
     * @see SDPClient#serviceSearchAttributeRequest
     */
    int[] attrSet;

    /**
     * Creates ServiceSearcherBase and save all required info in it.
     */
    ServiceSearcherBase(int[] attrSet, UUID[] uuidSet, RemoteDevice btDev) {
        Object checkObj = new Object();
        Hashtable dupChecker = new Hashtable();
        Enumeration set;

        // checking on null
        if (uuidSet == null) {
            throw new NullPointerException("null instance of UUID set");
        }

        if (btDev == null) {
            throw new NullPointerException("null instance of RemoteDevice");
        }

        // check UUID size
        if (uuidSet.length == 0) {
            throw new IllegalArgumentException("zero UUID set size");
        }

        // attrSet checking
        if (attrSet != null) {

            // check attrSet size
            if (attrSet.length == 0) {
                throw new IllegalArgumentException("zero attrSet size");
            } else if (attrSet.length > ServiceRecordImpl.RETRIEVABLE_MAX) {
                throw new IllegalArgumentException("attrSet size exceeding");
            }

            for (int i = 0; i < attrSet.length; i++) {

                // check that attribute ID is valid
                if ((attrSet[i] & MASK_OVERFLOW) != 0) {
                    throw new IllegalArgumentException("illegal attrSet");
                }

                // check attribute ID duplication
                if (dupChecker.put(new Integer(attrSet[i]), checkObj) != null) {
                    throw new IllegalArgumentException(
                            "duplicated attribute ID");
                }
            }
        }

        // adds defaults attribute IDs
        for (int i = 0; i <= 0x0004; i++) {
            dupChecker.put(new Integer(i), checkObj);
        }

        // creates attrSet to save attrIDs in the carrier
        this.attrSet = new int[dupChecker.size()];
        set = dupChecker.keys();

        for (int i = 0; set.hasMoreElements(); i++) {
            this.attrSet[i] = ((Integer) set.nextElement()).intValue();
        }
        dupChecker.clear();

        // uuidSet checking
        for (int i = 0; i < uuidSet.length; i++) {

            // check that UUID is not null instance
            if (uuidSet[i] == null) {
                throw new NullPointerException("null instance of UUID");
            }

            // check UUID duplication
            if (dupChecker.put(uuidSet[i], checkObj) != null) {
                throw new IllegalArgumentException("duplicated UUID");
            }
        }

        // creates uuidSet to save UUIDs in the carrier
        this.uuidSet = new UUID[dupChecker.size()];
        set = dupChecker.keys();

        for (int i = 0; set.hasMoreElements(); i++) {
            this.uuidSet[i] = (UUID) set.nextElement();
        }

        this.btDev = btDev;
    }

    /**
     * Informs this listener about errors during Service Discovering process.
     *
     * @param errorCode error code recieved from server or generated locally
     * due to transaction terminating.
     *
     * @param info detail information about the error
     *
     * @param transactionID ID of transaction response recieved within.
     */
    abstract public void errorResponse(int errorCode, String info,
        int transactionID);

    /**
     * Informs this listener about found services records.
     *
     * @param handleList service records handles returned by server within
     * SDP_ServiceSearchResponse.
     *
     * @param transactionID ID of transaction response recieved within.
     */
    abstract public void serviceSearchResponse(int[] handleList,
        int transactionID);

    /**
     * Informs this listener about found attributes of specified service record.
     *
     * @param attrIDs list of attributes whose values requested from server
     * within SDP_ServiceAttributesRequest.
     *
     * @param attributeValues values returned by server within
     * SDP_ServiceAttributesResponse.
     *
     * @param transactionID ID of transaction response recieved within.
     */
    abstract public void serviceAttributeResponse(int[] attrIDs,
            DataElement[] attributeValues, int transactionID);

    /**
     * Informs this listener about attributes of fisrt found service record.
     */
    abstract public void serviceSearchAttributeResponse(int[] attrIDs,
            DataElement[] attributeValues, int transactionID);
}
