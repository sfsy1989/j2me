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

package javax.microedition.sip;

import com.sun.midp.i3test.TestCase;
import javax.microedition.io.Connector;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import javax.microedition.sip.*;
import gov.nist.microedition.sip.SipServerConnectionImpl;

/**
 * Tests for end-to-end SIP client-server communication
 *
 */
public class TestPrack extends TestCase implements 
        SipClientConnectionListener, SipServerConnectionListener {

    /** Connection notifier. */
    SipConnectionNotifier scn = null;
    /** Server connection. */
    // SipServerConnection ssc = null;
    /** Client connection. */
    SipClientConnection scc = null;
    /** Boolean flag to indicate that test was successful */
    boolean done = false;
    /** SIP request message from client to server */
    String clientMsg = "Client : How are you?";
    /** SIP response message from server to client */
    String serverMsg = "Server : Fine";

    /**
     * Open the connection and start the thread to call the run
     * method.  It waits until the InputStream is open before returning.
     */
    void setup() {
        try {
            // Open SIP server connection and listen to port 6789
            scn = (SipConnectionNotifier) Connector.open("sip:6789");
        } catch (Exception ex) {
            assertNull("Exception during scn open", scn);
            ex.printStackTrace();
        }

        assertNotNull("scn is null", scn);

        try {
            scn.setListener(this);
        
            // Open SIP client connection to the local SIP server
            scc = (SipClientConnection) 
                Connector.open("sip:sippy.tester@localhost:6789");
        
            scc.setListener(this);
        } catch (Exception ex) {
            assertNull("Exception during sc open", scc);
            ex.printStackTrace();
            cleanup();
        }

        assertNotNull("sc is null", scc);

    }

    /**
     * Send a SIP message from user agent client to user agent server
     * SipConnectionNotifier queues the incoming requests if the 
     * instance of SipServerConnection is not available yet
     *
     */
    public void testPrack() {
        
        try {
            scc.initRequest("INVITE", scn);
            // scc.setHeader("From", "sip:sippy.tester@localhost");
            scc.setHeader("From", "sip:alice@some.server");
            scc.setHeader("To", "sip:bob@some.other.server");
            scc.setHeader("Subject", "Hello...");
            scc.setHeader("Contact", "sip:user@"+scn.getLocalAddress() + 
                    ":" + scn.getLocalPort()); 

            // write message body
            scc.setHeader("Content-Type", "text/plain");
            scc.setHeader("Content-Length", 
                    Integer.toString(clientMsg.length()));
            scc.setHeader("Require", "event, path, 100rel");
            OutputStream os = scc.openContentOutputStream();
            os.write(clientMsg.getBytes());
            os.close(); // close stream and send the message
       
            // System.out.println("Inviting..." + scc.getHeader("To"));
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cleanup();
            fail("IOException in testPrack");
        } 

    }

    /**
     * Accept a new connection request and process the request from client.
     * Send "OK" to client
     *
     * This method is declared in SipServerConnectionListener
     * 
     * @param scnLocal Local SIP connection notifier
     */
    public void notifyRequest(SipConnectionNotifier scnLocal) {
        try {
            // block and wait for incoming request.
            // SipServerConnection is establised and returned
            // when new request is received.
            SipServerConnection ssc = scnLocal.acceptAndOpen();
            assertNotNull("ssc is null", ssc);

            if (ssc.getMethod().equals("INVITE")) {
                ssc.initResponse(180);
                ssc.setHeader("RSeq", "1");
                ssc.send();
            } else if (ssc.getMethod().equals("PRACK")) {
                ssc.initResponse(200);
                ssc.send();
            }
            ssc.close();
        } catch (SipException sipex) {
            assertNull("Unexpected SipException", sipex);
            sipex.printStackTrace();
            cleanup();
        } catch (IOException ioe) {
            assertNull("Unexpected IOException", ioe);
            ioe.printStackTrace();
            cleanup();
        }
    }

    
    /**
     * Received the response from user agent server
     *
     * This method is declared in SipClientConnectionListener
     * 
     * @param sccLocal Local SIP Client (UAC)
     *
     */
    public void notifyResponse(SipClientConnection sccLocal) {
        try {

           boolean ok = sccLocal.receive(0);

           if (ok) {  // response received
            
               if (sccLocal.getStatusCode() == 180) {
                    // scc.initPrack();
                    // scc.initRequest("PRACK", scn);    
                    SipClientConnection sccPrack = 
                            sccLocal.getDialog().getNewClientConnection("PRACK");
                    // scc.setHeader("Content-Type", "text/plain");
                    String rSeq = sccLocal.getHeader("RSeq");
                    sccPrack.setHeader("RAck", rSeq + " " +
                                    sccLocal.getHeader("CSeq"));
                    sccPrack.setListener(this);
                    sccPrack.send();
                    synchronized (this) {
                        done = true;
                        notify();
                    }
               //} else if (sccLocal.getStatusCode() == 200) {
                    // System.out.println("ACK received at server");
               }
           }
        } catch (Exception ex) {
            // handle Exceptions
            ex.printStackTrace();
        } 
    }

    /**
     * Close all connections.
     *
     */
    void cleanup() {
        // System.out.println("cleanup called");
        try {
            if (scc != null) {
                scc.close();
            }

            if (scn != null) {
                scn.close();
            }
        } catch (IOException ioe) {
            assertNull("Unexpected IOException during cleanup", ioe);
            ioe.printStackTrace();
        } 
    }

    /**
     * Tests execute.
     *
     */
    public void runTests() {
        declare("Test TestPrack ");

        setup();
        testPrack();        

        synchronized (this) {
            while (!done) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    System.out.println("Catch interrupt");
                }
            }
        }

        cleanup();
    
        assertTrue("Failure in TestPrack", done);
    }

}


