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
import gov.nist.siplite.header.ToHeader;
import gov.nist.siplite.header.CallIdHeader;
import gov.nist.siplite.header.FromHeader;
import gov.nist.siplite.header.ContactHeader;
import gov.nist.siplite.header.ExpiresHeader;
import gov.nist.siplite.address.URI;
import gov.nist.siplite.address.SipURI;
import gov.nist.core.ParseException;
import javax.microedition.io.*;
import com.sun.midp.i3test.TestCase;
import java.io.*;

/**
 * Tests for SipConnection class.
 *
 */
public class TestSipClientConnection extends TestCase {
    
    /** URI of other side of SIP session. */
    private String corrURI = "sip:sippy.tester@localhost:5060";
    
    /** Server tag. */
    private static final String toTag = StackConnector.generateTag();
    
    /** Flag of creating a dialog. */
    private boolean isDialogCreated = false;
    
    /** Refresh ID for refreshable requests. */
    private int refreshID = 0;

    /**
     * Body of the test 1.
     *
     * Walk along states of SipClientConnection object.
     * @param method string representation of the requests method
     * @param transport the name of transport protocol
     */
    void Test1(String method, String transport) {
        /** Client connection. */
        SipClientConnection sc = null;
	StubSipRefreshListener refreshListener = null;
	
	/**
	 * Dialog is created in case of INVITE method (RFC 3261, 12.1)
	 * or SUBSCRIBE method (RFC 3265, 3.3.4) after provisional requests.
	 */
	isDialogCreated = method.equals("INVITE") || 
	    method.equals("SUBSCRIBE");
        try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) 
	        Connector.open(corrURI + ";transport=" + transport);
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
        }
        assertNotNull("sc is null", sc);
	
	// State of client connection must be "Created"
	assertEquals("State is not CREATED", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.CREATED);
	
	// Check the CREATED state
	checkCreatedState(sc);
	
	// Move to INITIALIZED state
	try {
	    sc.initRequest(method, null);
	    refreshListener = new StubSipRefreshListener();
	    refreshID = sc.enableRefresh(refreshListener);
            sc.setHeader("Content-Type", "text/plain");

	    // SUBSCRIBE request - Event header is mandatory (RFC 3265, 7.2)
	    if (method.equals("SUBSCRIBE")) {
		sc.setHeader("Event", "12345");
	    }

	} catch (Throwable e) {
            fail("INITIALIZED " +e +" was caused");
	}
	
	assertEquals("State is not INITIALIZED", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.INITIALIZED);
	
	// Check the INITIALIZED state after CREATED state
	checkInitializedState(sc, SipClientConnectionImpl.CREATED);
	
	// Move to STREAM_OPEN state
	String msg = "SipClientConnection class testing.";
	OutputStream os = null;
	try {
            sc.setHeader("Content-Length", Integer.toString(msg.length()));
            os = sc.openContentOutputStream();
        } catch (Throwable e) {
            fail("STREAM " +e +" was caused");
	}
	
	assertEquals("State is not STREAM_OPEN", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.STREAM_OPEN);
	
	// Check the STREAM_OPEN state
	checkStreamOpenState(sc, false);
        
	// write a message
	try {
	    os.write(msg.getBytes());
        } catch (Throwable e) {
            fail("write a message " +e +" was caused");
	}
        
	// Move to PROCEEDING state
	try {
	    os.close(); // close stream and send the message
        } catch (Throwable e) {
            fail("PROCEEDING " +e +" was caused");
	}
	
	assertEquals("State is not PROCEEDING", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.PROCEEDING);
	
	// check PROCEEDING state before and after provisional response
	checkComplexProceedingState(sc, method);
	
	// Move to COMPLETED state
        sendResponse(sc, Response.OK);
       
	assertEquals("State is not COMPLETED", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.COMPLETED);
	
	// Check the COMPLETED state
        if (!method.equals("INVITE")) {
	    // Transaction instanse must be in completed state
	    // (RFC 3261, 17.1.4)
	    checkCompletedState(sc, isDialogCreated, 
		Transaction.TERMINATED_STATE, Transaction.COMPLETED_STATE);
	} else {
	    // Transaction instanse must be in terminated state
	    // (RFC 3261, 17.1.1.2)
	    checkCompletedState(sc, true, Transaction.TERMINATED_STATE,
	    Transaction.TERMINATED_STATE);
	    // move to INITIALIZED state by init ACK
            Request request = null;
            CallIdHeader callIdHeader = null;
            FromHeader fromHeader = null;
            URI requestURI = null;
            ToHeader toHeader = null;
            try {
                 
                /**
                 * RFC 3261, 17.1.1.3:
                 * The ACK request constructed by the client transaction 
                 * MUST contain values for the Call-ID, From, 
                 * and Request-URI that are equal to the values of 
                 * those header fields in the request
                 */
                // save previous request data
                request = ((SipClientConnectionImpl)sc).getRequest();
                callIdHeader = request.getCallId();
                fromHeader = request.getFromHeader();
                requestURI = request.getRequestURI();
	        sc.initAck();
                // get the ACK request
                request = ((SipClientConnectionImpl)sc).getRequest();
                toHeader = request.getTo();
                assertTrue("ACK: callId is wrong", 
                    callIdHeader.toString().equals(
                    request.getCallId().toString()));
                assertTrue("ACK: From header is wrong", 
                    fromHeader.toString().equals(
                    request.getFromHeader().toString()));
                assertTrue("ACK: requestURI is wrong", 
                    requestURI.toString().equals(
                    request.getRequestURI().toString()));
                /**
                 * RFC 3261, 17.1.1.3:
                 * The To header field in the ACK MUST equal 
                 * the To header field in the response being acknowledged.
                 */
                Address toAddress = StackConnector
                    .addressFactory.createAddress(corrURI);
                ToHeader toHeader1 = StackConnector
                        .headerFactory.createToHeader(
                        toAddress, null);
                toHeader1.setTag(toTag);
                assertTrue("ACK: To header is wrong", 
                    toHeader.toString().equals(toHeader1.toString()));
	    } catch (Throwable e) {
	        fail("Exception was caused");
	    }
	
            assertEquals("State is not INITIALIZED", 
	        ((SipClientConnectionImpl)sc).getState(),
	        SipClientConnectionImpl.INITIALIZED);
		
	    // return to COMPLETED state by sending ACK
	    try {
	        sc.send();
	    } catch (Throwable e) {
	        fail("Exception was caused");
	    }
            
	    
	} 
	
	assertEquals("State is not COMPLETED", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.COMPLETED);
	
    // close connection
	try {
	    sc.close();
        } catch (Throwable e) {
            fail("" +e +" was caused");
	}
	
	assertEquals("State is not TERMINATED", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.TERMINATED);
    
	}
    
    /**
     * Check the state CREATED of SipClientConnection object.
     *
     * @param sc SipClientConnection object in CREATED state
     */
    private void checkCreatedState(SipClientConnection sc) {
	// Try to call methods other that initRequest and setListener
	
	// enableRefresh:
        disMethod_enableRefresh(sc);
	
	// initAck:
        disMethod_initAck(sc);
	
	// initCancel:
	disMethod_initCancel(sc);
	
	// receive:
        disMethod_receive(sc);
	
	// setCredentials:
	disMethod_setCredentials(sc);
	
	// setRequestURI:
	disMethod_setRequestURI(sc);
	
	// addHeader:
	disMethod_addHeader(sc);
	
	// openContentInputStream:
	disMethod_openContentInputStream(sc);
	
	// openContentOutputStream:
	disMethod_openContentOutputStream(sc);
	
	// removeHeader:
	disMethod_removeHeader(sc);
	
	// send:
	disMethod_send(sc);
	
	// setHeader:
	disMethod_setHeader(sc);
	
	// Dialog must be unaccessable
	assertTrue("Dialog object is visible on CREATED state",
	    sc.getDialog() == null);
	    
	// Client transaction must be null
	assertTrue("Client transaction is not null on CREATED state",
	    ((SipClientConnectionImpl)sc).getClientTransaction() == null);
    }
    
    /**
     * Check the state INITIALIZED of SipClientConnection object.
     *
     * @param sc SipClientConnection object in INITIALIZED state
     * @param prevState the previous state
     */
    private void checkInitializedState(SipClientConnection sc,
        int prevState) {
	
	// initAck:
        disMethod_initAck(sc);
	
	// initCancel:
	disMethod_initCancel(sc);
	
	// initRequest:
	disMethod_initRequest(sc);
	
	// receive:
        disMethod_receive(sc);
	
	// openContentInputStream:
	disMethod_openContentInputStream(sc);
	
	// Check dialog and transaction
	if (prevState == SipClientConnectionImpl.CREATED) {
	    // Dialog must be unaccessable
	    assertTrue("Dialog object is visible on INITIALIZED state",
	        sc.getDialog() == null);
	    
	    // Client transaction must be null
	    assertTrue("Client transaction is not null on INITIALIZED state",
	        ((SipClientConnectionImpl)sc).getClientTransaction() == null);
	}
    }
    
    /**
     * Check the state STREAM_OPEN of SipClientConnection object.
     *
     * @param sc SipClientConnection object in STREAM_OPEN state
     * @param isDialog true when dialog is created, else - false
     */
    private void checkStreamOpenState(SipClientConnection sc,
        boolean isDialog) {
	
	// enableRefresh:
        disMethod_enableRefresh(sc);
	
	// initAck:
        disMethod_initAck(sc);
	
	// initCancel:
	disMethod_initCancel(sc);
	
	// initRequest:
	disMethod_initRequest(sc);
	
	// receive:
        disMethod_receive(sc);
	
	// setCredentials:
	disMethod_setCredentials(sc);
	
	// setRequestURI:
	disMethod_setRequestURI(sc);
	
	// addHeader:
	disMethod_addHeader(sc);
	
	// openContentInputStream:
	disMethod_openContentInputStream(sc);
	
	// openContentOutputStream:
	disMethod_openContentOutputStream(sc);
	
	// removeHeader:
	disMethod_removeHeader(sc);
	
	// setHeader:
	disMethod_setHeader(sc);
    	
	// Check dialog and transaction
	if (!isDialog) {
	    // Dialog must be unaccessable
	    assertTrue("Dialog object is visible on STREAM_OPEN state",
	        sc.getDialog() == null);
	    
	    // Client transaction must be null
	    assertTrue("Client transaction is not null on STREAM_OPEN state",
	        ((SipClientConnectionImpl)sc).getClientTransaction() == null);
	}

    }
    
    /**
     * Complex check the PROCEEDing state (before and after
     * provisional response.
     *
     * @param sc SipClientConnection object in CREATED state
     * @param method the method of request
     */
    private void checkComplexProceedingState(SipClientConnection sc,
        String method) {
	
	// Check the PROCEEDING state
	// no provisional request - dialog must be unvisible
	// (RFC 3261, 12.1)
        if (method.equals("INVITE")) {
	    // INVITE: transaction instanse must be in calling state
	    // (RFC 3261, 17.1.1.2)
	    checkProceedingState(sc, false, 0, Transaction.CALLING_STATE);
	} else {
	    // non-INVITE: transaction instanse must be in trying state
	    // (RFC 3261, 17.1.4)
	    checkProceedingState(sc, false, 0, Transaction.TRYING_STATE);
	}
	// Receiving provisional response
	sendResponse(sc, Response.TRYING);
	
	assertEquals("State is not PROCEEDING", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.PROCEEDING);
	
	// Check the PROCEEDING state
	// provisional request - dialog must be in early state
	// (JSR180)
	// Transaction instanse must be in proceeding state
	// (RFC 3261, 17.1.1.2, 17.1.4)
	checkProceedingState(sc, isDialogCreated,
	    SipDialog.EARLY, Transaction.PROCEEDING_STATE);

	// Receiving any provisional response when transaction is in
	// proceedin state doesn't change its state (both INVITE
	// and non-INVITE, RFC3261, 17.1.1.2, 17.1.4)
	sendResponse(sc, Response.TRYING);
	assertEquals("State is not PROCEEDING", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.PROCEEDING);
	checkProceedingState(sc, isDialogCreated,
	    SipDialog.EARLY, Transaction.PROCEEDING_STATE);

    // Move to UNAUTHORIZED state
        sendResponse(sc, Response.UNAUTHORIZED);
    
    assertEquals("State is not UNAUTHORIZED", 
        ((SipClientConnectionImpl)sc).getState(),
        SipClientConnectionImpl.UNAUTHORIZED);
    
    // Check the UNAUTHORIZED state
    checkUnautorizedState(sc);
    
    // Move to back to PROCEEDING state
    try {
        sc.setCredentials("name", "pass", "realm"); // re-send request
        } catch (Throwable e) {
            fail("" +e +" was caused");
    }
    
    
    assertEquals("State is not PROCEEDING", 
        ((SipClientConnectionImpl)sc).getState(),
        SipClientConnectionImpl.PROCEEDING);

    }
    
    /**
     * Check the state PROCEEDING of SipClientConnection object.
     *
     * @param sc SipClientConnection object in CREATED state
     * @param isDialogVisible true when dialog is visible, else - false
     * @param dialogState required state of Dialog object
     * @param transState required state of Transaction object
     */
    private void checkProceedingState(SipClientConnection sc,
        boolean isDialogVisible, int dialogState, int transState) {
	
	// enableRefresh:
        disMethod_enableRefresh(sc);
	
	// initAck:
        disMethod_initAck(sc);
	
	// initRequest:
	disMethod_initRequest(sc);
	
	// setCredentials:
	disMethod_setCredentials(sc);
	
	// setRequestURI:
	disMethod_setRequestURI(sc);
	
	// addHeader:
	disMethod_addHeader(sc);
	
	// openContentOutputStream:
	disMethod_openContentOutputStream(sc);
	
	// removeHeader:
	disMethod_removeHeader(sc);
	
	// send:
	disMethod_send(sc);
	
	// setHeader:
	disMethod_setHeader(sc);
    	
	// Check dialog state
	if (!isDialogVisible) { // dialog must be null
	    assertTrue("Dialog object is visible on PROCEEDING state",
	        sc.getDialog() == null);
	} else { // check state
	    SipDialog dialog = sc.getDialog();
	    assertTrue("Dialog object is not visible on PROCEEDING state",
	        dialog != null);
	    assertTrue("Proceeding - Dialog has wrong state", 
	        (int)dialog.getState() == dialogState);
	}
	    
	// Check client transaction
	ClientTransaction clientTransaction = 
	    ((SipClientConnectionImpl)sc).getClientTransaction();
	assertTrue("Client transaction is null on PROCEEDING state",
	    clientTransaction != null);
	assertTrue("Client transaction has wrong state",
	    clientTransaction.getState() == transState);
    }
    
    /**
     * Check the state COMPLETED of SipClientConnection object.
     *
     * @param sc SipClientConnection object in COMPLETED state
     * @param isDialogVisible true when dialog is visible, else - false
     * @param transactionStateReliable client tramsaction state
     * for reliable protocols
     * @param transactionStateUnReliable client tramsaction state
     * for unreliable protocols
     */
    private void checkCompletedState(SipClientConnection sc,
        boolean isDialogVisible, int transactionStateReliable,
	int transactionStateUnReliable) {
	
	// enableRefresh:
        disMethod_enableRefresh(sc);
	
	// initCancel:
	disMethod_initCancel(sc);
	
	// initRequest:
	disMethod_initRequest(sc);
	
	// setCredentials:
	disMethod_setCredentials(sc);
	
	// setRequestURI:
	disMethod_setRequestURI(sc);
	
	// addHeader:
	disMethod_addHeader(sc);
	
	// openContentOutputStream:
	disMethod_openContentOutputStream(sc);
	
	// removeHeader:
	disMethod_removeHeader(sc);
	
	// send:
	disMethod_send(sc);
	
	// setHeader:
	disMethod_setHeader(sc);
	    
	if (!isDialogVisible) { // dialog must be null
	    assertTrue("Dialog object is visible on COMPLETED state",
	        sc.getDialog() == null);
	} else { // check state
	    // Dialog must be visible and be in confirmed state
	    SipDialog dialog = sc.getDialog();
	    assertTrue("Dialog object is not visible on COMPLETED state",
	        dialog != null);
	    assertTrue("Completed - Dialog has wrong state", 
	        (int)dialog.getState() == SipDialog.CONFIRMED);
	}
	
	// Check client transaction
	ClientTransaction clientTransaction = 
	    ((SipClientConnectionImpl)sc).getClientTransaction();
	assertTrue("Client transaction is null on COMPLETED state",
	    clientTransaction != null);
	assertTrue("COMPLETED: Client transaction has wrong state",
	    clientTransaction.isReliable() ? 
	    clientTransaction.getState() == transactionStateReliable :
	    clientTransaction.getState() == transactionStateUnReliable);
    }
    
    /**
     * Check the state UNAUTHORIZED of SipClientConnection object.
     *
     * @param sc SipClientConnection object in UNAUTHORIZED state
     */
    private void checkUnautorizedState(SipClientConnection sc) {
	// Try to call methods other that initRequest and setListener
	
	// enableRefresh:
        disMethod_enableRefresh(sc);
	
	// initAck:
        disMethod_initAck(sc);
	
	// initCancel:
	disMethod_initCancel(sc);
	
	// initRequest:
	disMethod_initRequest(sc);
	
	// receive:
        disMethod_receive(sc);
	
	// setRequestURI:
	disMethod_setRequestURI(sc);
	
	// addHeader:
	disMethod_addHeader(sc);
	
	// openContentInputStream:
	disMethod_openContentInputStream(sc);
	
	// openContentOutputStream:
	disMethod_openContentOutputStream(sc);
	
	// removeHeader:
	disMethod_removeHeader(sc);
	
	// send:
	disMethod_send(sc);
	
	// setHeader:
	disMethod_setHeader(sc);
	    
	// Dialog must be unvisible (terminated state)    
	assertTrue("Dialog object must be unvisible on UNAUTHORIZED state",
	    sc.getDialog() == null);
    }
       
    /**
     * Disabled method enableRefresh.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_enableRefresh(SipClientConnection sc) {
	StubSipRefreshListener refresh = new StubSipRefreshListener();
	try {
	    sc.enableRefresh(refresh);
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method initAck.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_initAck(SipClientConnection sc) {
	try {
	    sc.initAck();
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method initCancel.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_initCancel(SipClientConnection sc) {
	try {
	    sc.initCancel();
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method receive.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_receive(SipClientConnection sc) {
	try {
	    sc.receive(10000);
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method setCredentials.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_setCredentials(SipClientConnection sc) {
	try {
	    sc.setCredentials("name", "password", "realm");
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method setRequestURI.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_setRequestURI(SipClientConnection sc) {
	try {
	    sc.setRequestURI("sip:sippy.tester@localhost:5060");
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method addHeader.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_addHeader(SipClientConnection sc) {
	try {
	    sc.addHeader("name", "value");
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method openContentInputStream.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_openContentInputStream(SipClientConnection sc) {
	try {
	    sc.openContentInputStream();
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method openContentOutputStream.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_openContentOutputStream(SipClientConnection sc) {
	try {
	    sc.openContentOutputStream();
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method removeHeader.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_removeHeader(SipClientConnection sc) {
	try {
	    sc.removeHeader("Header");
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method send.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_send(SipClientConnection sc) {
	try {
	    sc.send();
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method setHeader.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_setHeader(SipClientConnection sc) {
	try {
	    sc.setHeader("Header", "value");
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
	}
    }
       
    /**
     * Disabled method initRequest.
     *
     * @param sc input SipClientConnection object
     */
    private void disMethod_initRequest(SipClientConnection sc) {
	try {
	    sc.initRequest("MESSAGE", null);
	    fail("No SipException was caused");
	} catch (SipException e) {
	    assertEquals("Wrong error code", e.getErrorCode(),
	        SipException.INVALID_STATE);
	} catch (Throwable e) {
	    fail("Wrong exception was caused");
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
	    if ((respCode / 100 == 2) && (refreshID != 0)) {
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
	}
    }
       
    /**
     * Tests execute
     *
     */
    public void runTests() {
        String arrProt[] = {"UDP", "TCP"};
        String arrMethods[] = {"INVITE", "MESSAGE", "SUBSCRIBE",
            "OPTIONS", "REGISTER", "INFO", "PUBLISH"};
        for (int i = 0; i < arrMethods.length; i++) {
            declare("SipClientConnection - " + arrMethods[i]);
            for (int j = 0; j < arrProt.length; j++) {
                Test1(arrMethods[i], arrProt[j]);
            }
        }
    }

}

/**
 * Stub class implements SipRefreshListener
 * for testing SipClientConnection.
 */
class StubSipRefreshListener implements SipRefreshListener {
    /**
     * Empty callback function refreshEvent.
     *
     * @param refreshID ID of refresh action
     * @param statusCode status of refresh task
     * @param reasonPhrase additional text message
     */
    public void refreshEvent(int refreshID,
                         int statusCode,
                         java.lang.String reasonPhrase) {
    }
}

