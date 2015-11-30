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
public class TestRegisterTCP extends TestCase implements 
        SipClientConnectionListener, SipServerConnectionListener {

    /** Connection notifier. */
    SipConnectionNotifier scn = null;
    /** Connection notifier. */
    SipConnectionNotifier scnAssociated = null;
    /** Server connection. */
    // SipServerConnection ssc = null;
    /** Client connection. */
    SipClientConnection scc = null;
    /** Boolean flag to indicate that test was successful */
    boolean done = false;

    /**
     * Open the connection and start the thread to call the run
     * method.  It waits until the InputStream is open before returning.
     */
    void setup() {
        try {
            // Open SIP server connection and listen to port 9090
            // scn = (SipConnectionNotifier) Connector.open("sip:9090");
            scn = (SipConnectionNotifier) Connector.open(
                "sip:9090;transport=tcp");
            // scnAssociated = (SipConnectionNotifier) 
            //      Connector.open("sip:;transport=tcp");
        } catch (Exception ex) {
            assertNull("Exception during scn open", scn);
            ex.printStackTrace();
        }

        assertNotNull("scn is null", scn);

        try {
            scn.setListener(this);
        
            // Open SIP client connection to the local SIP server
            scc = (SipClientConnection) 
                Connector.open("sip:sippy.tester@localhost:9090;transport=tcp");
        
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
    public void testSendRegister() {
        
        try {
            // scc.initRequest("REGISTER", scnAssociated);
            scc.initRequest("REGISTER", null);
            scc.send();

        } catch (IOException ioe) {
            ioe.printStackTrace();
            cleanup();
            fail("IOException in testSendRegister");
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

            if (ssc.getMethod().equals("REGISTER")) {
                ssc.initResponse(200);
                ssc.send();
            }
            ssc.close();
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
               if (sccLocal.getStatusCode() == 200) {
                    synchronized (this) {
                        done = true;
                        notify();
                    }
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

            if (scnAssociated != null) {
                scnAssociated.close();
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
        declare("TestRegisterTCP ");

        setup();
        testSendRegister();        

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
    
        assertTrue("Failure in TestRegisterTCP", done);
    }

}


