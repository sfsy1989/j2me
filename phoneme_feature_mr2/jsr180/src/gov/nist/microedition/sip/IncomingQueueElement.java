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

package gov.nist.microedition.sip;

import gov.nist.siplite.message.Response;
import gov.nist.siplite.stack.ClientTransaction;

/**
 * Element of queue of incoming responses
 * and associated client transactions.
 *
 */
class IncomingQueueElement {
    /** Incoming response. */
    private Response response;
    /** New client transaction. */
    private ClientTransaction clientTransaction;
    
    /** 
     * Constructor.
     * @param response incoming response
     * @param clientTransaction new client transaction
     */
    protected IncomingQueueElement(Response response,
        ClientTransaction clientTransaction) {
        this.response = response;
        this.clientTransaction = clientTransaction;
    }
    
    /** 
     * Checking if client transaction member is not null.
     * @return true when client transaction member is not null
     */
    protected boolean containsClientTransaction() {
        return clientTransaction != null;
    }
    
    /** 
     * Gets response.
     * @return response
     */
    protected Response getResponse() {
        return response;
    }
    
    /** 
     * Gets client transaction.
     * @return client transaction
     */
    protected ClientTransaction getClientTransaction() {
        return clientTransaction;
    }
    
    /** 
     * Sets client transaction.
     * @param clientTransaction new client transaction
     */
    protected void setClientTransaction(ClientTransaction clientTransaction) {
        this.clientTransaction = clientTransaction;
    }
}
