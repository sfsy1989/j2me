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
import javax.microedition.sip.SipConnectionNotifier;
import javax.microedition.sip.SipServerConnection;
import javax.microedition.sip.SipClientConnection;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import javax.microedition.sip.SipException;

/**
 * Tests for end-to-end SIP client-server communication
 *
 */
public class TestSipCommunication extends TestCase implements Runnable {

    /** Connection notifier. */
    SipConnectionNotifier scn = null;
    /** Server connection. */
    SipServerConnection ssc = null;
    /** Client connection. */
    SipClientConnection sc = null;
    /** Communication transport. */
    String transport = "udp";

    /** Server flag. */
    boolean serverEstablished = false;

    /** Thread that is blocked for acceptAndOpen. */
    Thread t1;

    /**
     * Open the connection and start the thread to call the run
     * method.  It waits until the InputStream is open before returning.
     */
    void setup() {
        serverEstablished = false;
        try {
            // Open SIP server connection and listen to port 5060
            scn = (SipConnectionNotifier) 
                Connector.open("sip:;transport="+transport);
        } catch (Exception ex) {
            assertNull("Exception during scn open", scn);
            ex.printStackTrace();
        }
        assertNotNull("scn is null", scn);

        try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) Connector.open("sip:" +
                                                      scn.getLocalAddress() +
                                                      ":" +
                                                      scn.getLocalPort() +
                                                      ";transport=" +
                                                      transport);
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
            ex.printStackTrace();
            cleanup();
        }

        assertNotNull("sc is null", sc);

        // Launch a thread which would be blocked for scn.acceptAndOpen()
        t1 = new Thread(this);
        t1.start();
    }

    /**
     * Accept a new connection request and process the request from client.
     */
    public void run() {
        try {
            // block and wait for incoming request.
            // SipServerConnection is establised and returned
            // when new request is received.
            ssc = scn.acceptAndOpen();

            assertNotNull("ssc is null", ssc);
            serverEstablished = true;
        } catch (SipException sipex) {
            assertNull("Unexpected SipException", sipex);
            sipex.printStackTrace();
            cleanup();
        } catch (IOException ioe) {
            assertNull("Unexpected IOException", ioe);
            ioe.printStackTrace();
            cleanup();
        } finally {
            synchronized (this) {
                notifyAll();
            }
        }
    }

    /**
     * Send request and receive a responce.
     */
    public void testSipCommunication() {
        boolean result = false;
        try {
            result = sendMsgFromClient("Hello SIP-World");
        } catch (SipException sipex) {
            assertNull("Unexpected SipException", sipex);
            sipex.printStackTrace();
        } catch (IOException ioe) {
            assertNull("Unexpected IOException", ioe);
            ioe.printStackTrace();
        } 

        if (result) {
            result = processClientRequest();
        } 

        if (result) {
            result = receiveMsgFromServer();
        } 

        assertTrue("SIP client is not communicating to server", result);
    }

    /**
     * Send a SIP message from user agent client to user agent server
     * SipConnectionNotifier queues the incoming requests if the 
     * instance of SipServerConnection is not available yet
     *
     * @param msg message to be sent
     * @return true if sc is non-null and message is sent successfully
     *         false otherwise
     */
    boolean sendMsgFromClient(String msg) throws SipException, IOException {
        if (sc != null) {
	    sc.initRequest("MESSAGE", null);
            sc.setHeader("From", "sip:sippy.tester@"+scn.getLocalAddress());
            sc.setHeader("Subject", "testing...");

            // write message body
            sc.setHeader("Content-Type", "text/plain");
            sc.setHeader("Content-Length", Integer.toString(msg.length()));
            OutputStream os = sc.openContentOutputStream();
            os.write(msg.getBytes());
            os.close(); // close stream and send the message
        
            return true;
        }

        return false;
    }

    /**
     * Process the client request at server and receive the message
     *
     * @return true if the client request is received at server
     *         false if the request is not received
     *
     */
    boolean processClientRequest() {
        boolean requestProcessed = false;

        StringBuffer receivedMsgStr = new StringBuffer();

        // Wait for the request to receive at server
        synchronized (this) {
            while (!serverEstablished) {
                try {
                    wait(2000);
                } catch (InterruptedException e) {
                    // Ignore interrupt
                }
            } 
        }

        if (serverEstablished) {
            try {
                // what was the SIP method
                String method = ssc.getMethod();
                if (method.equals("MESSAGE")) {
                    // read the content of the MESSAGE
                    String contentType = ssc.getHeader("Content-Type");
                    if ((contentType != null) && 
                        contentType.equals("text/plain")) {
                        InputStream is = ssc.openContentInputStream();
                        int ch;
                        // read content
                        ch = is.read();
                        while ((ch = is.read()) != -1) {
                            receivedMsgStr.append(ch);
                        }
                    }
                    // initialize SIP 200 OK and send it back
                    ssc.initResponse(200);
                    ssc.send();
                    requestProcessed = true;
                }
            } catch (Exception ex) {
                // handle Exceptions
                ex.printStackTrace();
            } finally {
            }
        }
        return requestProcessed;
    }

    /**
     * Receive the message and extract status code at server
     *
     * @return true if the status code is OK
     *         false otherwise
     *
     */
    boolean receiveMsgFromServer() {
        boolean receivedOKResponse = false;
        try {

           // wait maximum 15 seconds for response
           boolean ok = sc.receive(15000);

           if (ok) {  // response received
               if (sc.getStatusCode() == 200) {
                    receivedOKResponse = true;
               }
           }
        } catch (Exception ex) {
            // handle Exceptions
            ex.printStackTrace();
        } finally {
        } 
        return receivedOKResponse;
    }


    /**
     * Close all connections.
     *
     */
    void cleanup() {
        try {
            if (sc != null) {
                sc.close();
            }

            if (ssc != null) {
                ssc.close();
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

        declare("Test TestSipCommunication on " + transport);
        setup();
        testSipCommunication();        
        cleanup();
        assertTrue("OK", true); // temporary

        transport = "tcp";
        declare("Test TestSipCommunication on " + transport);
        setup();
        testSipCommunication();        
        cleanup();
        assertTrue("OK", true); // temporary

    }

}


