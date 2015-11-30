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
import javax.microedition.io.Connector;
import gov.nist.siplite.message.Request;
import gov.nist.siplite.message.Response;
import gov.nist.siplite.address.Address;
import gov.nist.siplite.address.SipURI;
import gov.nist.siplite.header.ToHeader;
import gov.nist.siplite.header.ContactHeader;
import gov.nist.siplite.stack.ClientTransaction;
import gov.nist.siplite.SipStack;
import gov.nist.siplite.stack.SIPServerResponseInterface;
import gov.nist.siplite.stack.GetSipServerResponse;
import gov.nist.core.ParseException;
import gov.nist.siplite.header.ExpiresHeader;
import com.sun.midp.i3test.TestCase;
/**
 * Tests for SipConnection class.
 *
 */
public class TestSipDialogClient extends TestCase {
    
    /** URI of other side of SIP session. */
    private String corrURI = "sip:sippy.tester@localhost:5060";
    
    /** Refresh ID for refreshable requests. */
    private int refreshID = 0;
    
    /** Server tag. */
    private static final String toTag = StackConnector.generateTag();

    /**
     * Body of the test 1.
     *
     * INVITE request - walk along states of SipDialog object.
     * @param transport the name of transport protocol
     */
    void Test1(String transport) {
        /** Client connection. */
        SipClientConnection sc = null;
        StubSipRefreshListener refreshListener = null;

        // create a dialog
		try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) 
	        Connector.open(corrURI + ";transport=" + transport);
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
        }

        // Dialog instance is not visible (no provisional respomse)
        // (JSR180, 1.0.1, p. 42)
        assertTrue("Dialog is visible on initialization", 
            sc.getDialog() == null);
       
        // Move to EARLY state
        try {
            sc.initRequest("INVITE", null);
            refreshListener = new StubSipRefreshListener();
            refreshID = sc.enableRefresh(refreshListener);
            sc.send(); // send a message
            // Receiving provisional response
            sendResponse(sc, Response.TRYING);
        } catch (Exception ex) {
            fail("Exception during moving to EARLY state");
        }

        // dialog must be visible
        SipDialog testDialog = sc.getDialog();
        assertTrue("Dialog is null", testDialog != null);
        // dialog must be in EARLY state
        assertTrue("Dialog instance has wrong state", 
	        testDialog.getState() == SipDialog.EARLY);

        // Move to CONFIRMED state
        sendResponse(sc, Response.OK);
        // dialog must be visible
        testDialog = sc.getDialog();
        assertTrue("Dialog is null", testDialog != null);
        // dialog must be in CONFIRMED state
        assertTrue("Dialog instance has wrong state", 
        testDialog.getState() == SipDialog.CONFIRMED);
        
		// close the connection
		try {
		    sc.close();
        } catch (Exception ex) {
            fail("Exception during closing SipClientConnection");
        }
        
        // Send a message inside dialog
        SipClientConnection scl = null;
        try {
            scl = testDialog.getNewClientConnection("MESSAGE");
            scl.send();
            sendResponse(scl, Response.MULTIPLE_CHOICES);
        } catch (Exception ex) {
            fail("Exception during sending request inside a dialog");
            ex.printStackTrace();
        }
        // dialog must be in CONFIRMED state
        assertTrue("Dialog instance has wrong state", 
        testDialog.getState() == SipDialog.CONFIRMED);
        
        // close the connection
        try {
            scl.close();
        } catch (Exception ex) {
            fail("Exception during closing SipClientConnection");
        }
        
        // Move to TERMINATED state
        try {
		    scl = testDialog.getNewClientConnection("BYE");
			scl.send();
            sendResponse(scl, Response.OK);
        } catch (Exception ex) {
            fail("Exception during moving to TERMINATED state");
            ex.printStackTrace();
        }
        // dialog must not be visible - see JSR180, method getDialog
        testDialog = scl.getDialog();
        assertTrue("Dialog is not null", testDialog == null);
        
        // close the connection
        try {
            scl.close();
        } catch (Exception ex) {
            fail("Exception during closing SipClientConnection");
        }
        
    }

    /**
     * Body of the test 2.
     *
     * SUBSCRIBE request - walk along states of SipDialog object.
     * @param transport the name of transport protocol
     */
    void Test2(String transport) {
        /** Client connection. */
        SipClientConnection sc = null;
        StubSipRefreshListener refreshListener = null;

        // create a dialog
        try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) 
            Connector.open("sip:sippy.tester@localhost:5060;transport="
            + transport);
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
        }

        // Dialog instance is not visible (no response)
        // (JSR180, 1.0.1, p. 42)
        assertTrue("Dialog is visible on initialization", 
            sc.getDialog() == null);
       
        // Move to EARLY state
        try {
            sc.initRequest("SUBSCRIBE", null);
            refreshListener = new StubSipRefreshListener();
            refreshID = sc.enableRefresh(refreshListener);
            // SUBSCRIBE request - Event header is mandatory (RFC 3265, 7.2)
            sc.setHeader("Event", "12345");
            sc.send(); // send a message
            sendResponse(sc, Response.TRYING);
        } catch (Exception ex) {
            fail("Exception during moving to EARLY state");
        }

        // dialog must be unvisible even after provisional response
        // (SUBSCRIBE dialog)
        assertTrue("Dialog is not visible", sc.getDialog() != null);
        assertTrue("Dialog instance has wrong state", 
        sc.getDialog().getState() == SipDialog.EARLY);

        // Move to CONFIRMED state
        sendResponse(sc, Response.OK);
        // dialog must be visible
        SipDialog testDialog = sc.getDialog();
        assertTrue("Dialog is null", testDialog != null);
        // dialog must be in CONFIRMED state
        assertTrue("Dialog instance has wrong state", 
        testDialog.getState() == SipDialog.CONFIRMED);
        
        // close the connection
        try {
            sc.close();
        } catch (Exception ex) {
            fail("Exception during closing SipClientConnection");
        }
        
        // Send a message inside dialog
        SipClientConnection scl = null;
        try {
            scl = testDialog.getNewClientConnection("MESSAGE");
            scl.send();
            sendResponse(scl, Response.MULTIPLE_CHOICES);
        } catch (Exception ex) {
            fail("Exception during sending request inside a dialog");
            ex.printStackTrace();
        }
        // dialog must be in CONFIRMED state
        assertTrue("Dialog instance has wrong state", 
        testDialog.getState() == SipDialog.CONFIRMED);
        
        // close the connection
        try {
            scl.close();
        } catch (Exception ex) {
            fail("Exception during closing SipClientConnection");
        }
        
        // Move to TERMINATED state
        try {
            scl = testDialog.getNewClientConnection("SUBSCRIBE");
            scl.setHeader("Event", "12345");
            // unsubscribe
            scl.setHeader("Expires", "0");
            scl.send();
            sendResponse(scl, Response.OK);
        } catch (Exception ex) {
            fail("Exception during moving to TERMINATED state");
            ex.printStackTrace();
        }
        // dialog must not be visible - see JSR180, method getDialog
        testDialog = scl.getDialog();
        assertTrue("Dialog is not null", testDialog == null);
        
        // close the connection
        try {
            scl.close();
        } catch (Exception ex) {
            fail("Exception during closing SipClientConnection");
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
            
            // When response is 200 OK change "To" header (RFC 3261, 17.1.1.3)
            if (respCode == Response.OK) {
                Address toAddress = StackConnector
                    .addressFactory.createAddress(corrURI);
                ToHeader toHeader = null;
                try {
                    toHeader = StackConnector
                        .headerFactory.createToHeader(
                        toAddress, null);
                } catch (ParseException ex) {
                    fail("Problem during the creation"
                    + " of the ToHeader");
                }
                response.setTo(toHeader);
            }

        // When response is 2xx and request is refreshable,
            // response must contain "expires" header (RFC 3265, 7.1)
	    if ((respCode / 100 == 2) && (refreshID != 0) && 
            (response.getHeader("Expires") == null)) {
		ExpiresHeader expHeader = new ExpiresHeader();
		expHeader.setExpires(1000);
		response.setHeader(expHeader);
            }

            // RFC 3261 8.2.6.2 Response must have To tag
            ToHeader toHeader = response.getTo();
            if (!toHeader.hasTag()) {
                // To header has no tag - add server tag
                toHeader.setTag(toTag);
                response.setTo(toHeader);
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
          
	    ClientTransaction clientTransaction = 
	        ((SipClientConnectionImpl)sc).getClientTransaction();
	    
	    SipStack sipStack = ((SipClientConnectionImpl)sc).getSipStack();
	    
	    SIPServerResponseInterface ssc = 
	        GetSipServerResponse.newSIPServerResponse(sipStack, response, 
		clientTransaction.getMessageChannel());
	    
	    ssc.processResponse(response, 
	        clientTransaction.getMessageChannel());
	    // receive a response
	    assertTrue("Response wasn't received", sc.receive(10000));	
	} catch (Throwable e) {
            fail("sendResponse " +e +" was caused");
	    e.printStackTrace();
	}
    }
       
    /**
     * Tests execute
     *
     */
    public void runTests() {
        String arrProt[] = {"UDP", "TCP"};
        for (int i = 0; i < arrProt.length; i++) {
            declare("SipDialog - INVITE " + arrProt[i]);
                Test1(arrProt[i]);
            declare("SipDialog - SUBSCRIBE " + arrProt[i]);
                Test2(arrProt[i]);
        }
    }

}

