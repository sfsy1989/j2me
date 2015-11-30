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

import javax.microedition.sip.*;
import java.io.OutputStream;
import gov.nist.microedition.sip.SipConnectionNotifierImpl;
import gov.nist.microedition.sip.SipClientConnectionImpl;
import gov.nist.siplite.SipStack;
import gov.nist.siplite.message.Request;
import gov.nist.siplite.message.Response;
import gov.nist.siplite.header.WWWAuthenticateHeader;
import gov.nist.siplite.header.AuthenticationHeader;
import gov.nist.siplite.stack.ClientTransaction;
import gov.nist.siplite.stack.MessageChannel;
import gov.nist.siplite.stack.Dialog;
import gov.nist.siplite.stack.Transaction;
import gov.nist.siplite.stack.SIPServerResponseInterface;
import gov.nist.siplite.stack.SIPStackMessageFactory;
import gov.nist.siplite.stack.GetSipServerResponse;
import gov.nist.siplite.address.Address;
import gov.nist.siplite.header.*;
import gov.nist.siplite.address.URI;
import gov.nist.siplite.address.SipURI;
import gov.nist.core.ParseException;
import javax.microedition.io.*;
import com.sun.midp.i3test.TestCase;
import java.io.*;

/**
 * Tests for initializing and sending CANCEL request.
 *
 */
public class TestSipClientCancel extends TestCase {
    
    /** URI of other side of SIP session (returns in 200 OK response). */
    private String corrURI = "sip:sippy.tester@localhost:5060";
    
    /** Server tag. */
    private static final String toTag = StackConnector.generateTag();

    /**
     * Body of the test 1.
     *
     * Create CANCEL request and check its fields.
     */
    void Test1() {
        /** Method of original request */
        String method = "INVITE";
        /** Client connection. */
        SipClientConnection sc = null;
        /**
         * Dialog is created in case of INVITE method (RFC 3261, 12.1)
         * or SUBSCRIBE method (RFC 3265, 3.3.4) after provisional requests.
         */
        try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) 
            Connector.open(corrURI);
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
        }
        assertNotNull("sc is null", sc);
        try {
            sc.initRequest(method, null);
        } catch (Throwable e) {
            fail("INITIALIZED " +e +" was caused");
        }
    
        // Move to PROCEEDING state
        try {
            sc.send();
        } catch (Throwable e) {
            fail("PROCEEDING " +e +" was caused");
        }

        // new SipClientConnection instance with CANCEL request
        SipClientConnection scCancel = null;
        // try to create scCancel before provisional response
        try {
            scCancel = sc.initCancel();
            fail("No SipException was thrown");
        } catch (SipException e) {
            assertTrue("Error code is wrong", 
                e.getErrorCode() == SipException.INVALID_STATE);
        } catch (Throwable e) {
            fail("PROCEEDING " +e +" was caused");
        }
        
        // receive provisional response
        sendResponse(sc, Response.TRYING);
        
        // try to create scCancel after provisional response
        // and before final response
        try {
            scCancel = sc.initCancel();
        } catch (Throwable e) {
            fail("PROCEEDING " +e +" was caused");
        }
        
        // check that scCancel != null
        assertTrue("CANCEL connection is null", scCancel != null);
        // check that the state of scCancel is INITIALIZED
        // (JSR 180, initCancel method of SipClientConnection)
        assertEquals("State is not INITIALIZED", 
            ((SipClientConnectionImpl)scCancel).getState(),
            SipClientConnectionImpl.INITIALIZED);
        
        // check CANCEL request fields
        // (RFC 3261, 9.1)
        assertTrue("RequestURI is wrong",
            sc.getRequestURI().equalsIgnoreCase(
            scCancel.getRequestURI()));
        assertTrue("Header Call-ID is wrong",
            sc.getHeader("Call-ID").equalsIgnoreCase(
            scCancel.getHeader("Call-ID")));
        assertTrue("Header To is wrong",
            sc.getHeader("To").equalsIgnoreCase(
            scCancel.getHeader("To")));
        assertTrue("Header From is wrong",
            sc.getHeader("From").equalsIgnoreCase(
            scCancel.getHeader("From")));
        // the numeric part of CSeq MUST be equal
        // (RFC 3261, 9.1)
        Request originalReq = ((SipClientConnectionImpl)sc).getRequest();
        Request cancelReq = ((SipClientConnectionImpl)scCancel).getRequest();
        CSeqHeader originalCS = (CSeqHeader)originalReq.getHeader("CSeq");
        CSeqHeader cancelCS = (CSeqHeader)cancelReq.getHeader("CSeq");
        assertTrue("Header CSeq is wrong",
            originalCS.getSequenceNumber() == cancelCS.getSequenceNumber());
        // A CANCEL constructed by a client MUST have only a single Via header
        // field value matching the top Via value in the request 
        // being cancelled. (RFC 3261, 9.1)
        // TBD (the situation with methods setHeader(), getHeader(), 
        // getHeaders() and removeHeader() is not clear yet
        
        // close connections
        try {
            scCancel.close();
        } catch (Throwable e) {
            fail("" +e +" was caused");
        }
        
        // receive final response
        sendResponse(sc, Response.OK);
        
        // try to create scCancel after final response
        try {
            scCancel = sc.initCancel();
            fail("No SipException was thrown");
        } catch (SipException e) {
            assertTrue("Error code is wrong", 
                e.getErrorCode() == SipException.INVALID_STATE);
        } catch (Throwable e) {
            fail("PROCEEDING " +e +" was caused");
        }
        
        // close connections
        try {
            sc.close();
            scCancel.close();
        } catch (Throwable e) {
            fail("" +e +" was caused");
        }
    
    }
    
    /**
     * Send a response with given code.
     *
     * @param sc SipClientConnection object
     * @param respCode response code
     */
    private void sendResponse(SipClientConnection sc, int respCode) {

        // Send a response
        Request request = null;
        Response response = null;

        try {
            request = ((SipClientConnectionImpl)sc).getRequest();
            // create a response for request
            response = request.createResponse(respCode);
            
            // RFC 3261 8.2.6.2 Response must have To tag
            ToHeader toHeader = response.getTo();
            if (!toHeader.hasTag()) {
                // To header has no tag - add server tag
                toHeader.setTag(toTag);
                response.setTo(toHeader);
            }

            if (respCode == Response.UNAUTHORIZED) {
                WWWAuthenticateHeader authHeader = new WWWAuthenticateHeader();
                authHeader.setParameter(AuthenticationHeader.REALM, "realm");
                response.setHeader(authHeader);
            }
            
            // add Contact header when request is not "MESSAGE"
            if ((respCode == Response.OK) && 
                (!request.getMethod().equals("MESSAGE"))) {
                SipURI contactURI = StackConnector
                            .addressFactory
                            .createSipURI("sippy.tester", // name
                            "localhost:5060");
                ContactHeader contactHeader =
                            StackConnector
                            .headerFactory
                            .createContactHeader(
                            StackConnector
                            .addressFactory
                            .createAddress(contactURI));
                response.addHeader(contactHeader);
            }
          
            ((SipClientConnectionImpl)sc).clearResponse();
            ClientTransaction clientTransaction = 
                ((SipClientConnectionImpl)sc).getClientTransaction();

            SipStack sipStack = ((SipClientConnectionImpl)sc).getSipStack();

            SIPServerResponseInterface ssc = 
                GetSipServerResponse.newSIPServerResponse(sipStack, response,
                clientTransaction.getMessageChannel());

            ssc.processResponse(response, 
                clientTransaction.getMessageChannel());
            while (((SipClientConnectionImpl)sc).getResponse() == null) {
                Thread.yield();
            }
        } catch (Throwable e) {
            fail("sendResponse " +e +" was caused");
        }
    }
       
    /**
     * Tests execute
     *
     */
    public void runTests() {
        declare("Test CANCEL request");
        Test1();
    }

}

