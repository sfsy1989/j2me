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
import gov.nist.microedition.sip.SipClientConnectionImpl;
import gov.nist.microedition.sip.SipServerConnectionImpl;

/**
 * Test for SUBSCRIBE request.
 *
 * Let's suppose that there are two applications A and B exchanging
 * SIP messages. Each application contains UAS and UAC. The corresponding
 * connection notifier, server connection and client connection will be
 * designated as NA, SA, CA and NB, SB, CB for A and B respectively.
 *
 * This test implements the following call flow:
 *
 * CA will send SUBSCRIBE
 * NB will receive SUBSCRIBE
 * SB will send 200 OK
 * CA will receive 200 OK
 * CB will send NOTIFY
 * NA will receive NOTIFY
 * SA will send 200 OK
 * CB will receive 200 OK
 */
public class TestSubscribeNotify extends TestCase implements
        SipClientConnectionListener, SipServerConnectionListener {

    /** Client connection of the application A */
    SipClientConnection   sccA = null;

    /** Client connection of the application B */
    SipClientConnection   sccB = null;

    /** Connection notifier of the application A */
    SipConnectionNotifier scnA = null;

    /** Connection notifier of the application B */
    SipConnectionNotifier scnB = null;

    /** Server connection of the application A */
    SipServerConnection   sscA = null;

    /** Server connection of the application B */
    SipServerConnection   sscB = null;

    /** True if a response was received by the client */
    private boolean responseReceived = false;

    /** True if NOTIFY request was received by the server */
    private boolean notifyReceived   = false;

    /** True if a response was sent by the server */
    private boolean responseSent     = false;

    /**
     * Opens connections and set up listeners.
     */
    private void setup() {
        try {
            // Open SIP server connection and listen to port 5060
            scnA = (SipConnectionNotifier)Connector.open("sip:5060");
            scnA.setListener(this);

            // Open SIP server connection and listen to port 5070
            scnB = (SipConnectionNotifier)Connector.open("sip:5070");
            scnB.setListener(this);

            // Initialize connection to 'localhost'
            sccA = (SipClientConnection)Connector.open(
                "sip:somebody@localhost:5070");
            sccA.setListener(this);
        } catch (Exception ex) { // handle Exceptions
            ex.printStackTrace();
            fail("Exception '" + ex + "' was thrown.");
        }
    }

    /**
     * Do cleanup.
     */
    private void cleanup() {
        try {
            if (sccA != null) {
                sccA.close();
            }

            if (sscA != null) {
                sscA.close();
                assertEquals("State should be TERMINATED",
                    SipServerConnectionImpl.TERMINATED,
                    ((SipServerConnectionImpl)sscA).getState());
            }

            if (sccB != null) {
                sccB.close();
            }

            if (sscB != null) {
                sscB.close();
                assertEquals("State should be TERMINATED",
                    SipServerConnectionImpl.TERMINATED,
                    ((SipServerConnectionImpl)sscB).getState());
            }

            if (scnA != null) {
                scnA.close();
            }

            if (scnB != null) {
                scnB.close();
            }
        } catch (IOException ioe) {
            assertNull("Unexpected IOException during cleanup", ioe);
            ioe.printStackTrace();
        }
    }

    /**
     * Sends 'SUBSCRIBE' request.
     * @param scc client connection
     */
    private void sendSubscribe(SipClientConnection scc) {
        try {
            // Initialize request and set From, To and Contact headers
            scc.initRequest("SUBSCRIBE", null);
            scc.setHeader("From", "sip:sippy.user@localhost");
            scc.setHeader("To", "sip:sippy.user@localhost");
            scc.setHeader("Contact", "<sip:UserB@localhost>;expires=3600");
            scc.setHeader("Expires", "3");

            try {
                scc.send();
                fail("SipException is not thrown when Event header is absent.");
            } catch (SipException ex) {
                // ok, SipException was generated
            }

            scc.setHeader("Event", "presence.test");
            scc.send();
        } catch (Exception ex) { // handle Exceptions
            ex.printStackTrace();
            fail("sendSubscribe(): exception '" + ex + "' was thrown.");
        }
    }

    /**
     * Waits for a response.
     */
    private void waitResponseReceived() {
        synchronized (this) {
            try {
                wait(2000);
            } catch (Exception e) {
                fail("Exception was thrown while waiting for response: " + e);
            }

            assertTrue("Response was not received!", responseReceived);
            responseReceived = false;

            // System.out.println(">>> waitResponseReceived(): ok!");
        }
    }

    /**
     * Sends NOTIFY.
     * @param ssc server connection
     * @param scc client connection
     */
    private void sendNotify(SipServerConnection ssc, SipClientConnection scc) {
        try {
            SipDialog dialog = ssc.getDialog();
            assertNotNull("sendNotify(): dialog is null!", dialog);

            scc = dialog.getNewClientConnection("NOTIFY");
            sccB = scc;
            assertNotNull("sendNotify(): scc is null!", scc);

            assertEquals("State should be INITIALIZED",
                SipClientConnectionImpl.INITIALIZED,
                ((SipClientConnectionImpl)scc).getState());

            scc.setHeader("Subscription-State", "active");
            scc.setListener(this);
            scc.send();
        } catch (Exception ex) { // handle Exceptions
            ex.printStackTrace();
            fail("sendNotify(): exception '" + ex + "' was thrown.");
        }
    }

    /**
     * Waits for NOTIFY.
     */
    private void waitNotifyReceived() {
        synchronized (this) {
            try {
                wait(2000);
            } catch (Exception e) {
                fail("Exception was thrown while waiting for NOTIFY: " + e);
            }

            assertTrue("NOTIFY was not received!", notifyReceived);
            notifyReceived = false;

            // System.out.println(">>> waitNotifyReceived(): ok!");
        }
    }

    /**
     * Sends a response.
     * @param scc client connection
     */
    private void waitResponseSent(SipClientConnection scc) {
        synchronized (this) {
            try {
                wait(2000);
            } catch (Exception e) {
                fail("Exception was thrown while waiting " +
                        "when the response will bew sent: " + e);
            }

            assertTrue("Response was not sent!", responseSent);
            responseSent = false;

            // System.out.println(">>> waitResponseSent(): ok!");
        }
    }

    /**
     * Test for SUBSCRIBE/NOTIFY.
     */
    public void testSubscribe() {
        try {
            sccA.initRequest("NOTIFY", null);
        } catch (IllegalArgumentException iae) {
            // ok, IAE was thrown when NOTIFY on attempt to sent NOTIFY
            // outside of a dialog.
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("sendNotify(): exception '" + ex +
                    "' instead of IAE was thrown.");
        }

        /*
         * RFC 3265, p. 4:
         *
         * A typical flow of messages would be:
         *
         * Subscriber          Notifier
         *     |-----SUBSCRIBE---->|     Request state subscription
         *     |<-------200--------|     Acknowledge subscription
         *     |<------NOTIFY------|     Return current state information
         *     |--------200------->|
         *     |<------NOTIFY------|     Return current state information
         *     |--------200------->|
         *
         */

        sendSubscribe(sccA);
        waitResponseReceived();

        sendNotify(sscB, sccB);
        waitNotifyReceived();
        waitResponseSent(sccA);
        waitResponseReceived();
    }

    /**
     * Accept a new connection request and process the request from client.
     * Send "OK" to client.
     *
     * This method is declared in SipServerConnectionListener
     * The code was copied from TestSipOriginatingInvite
     *
     * @param scnLocal Local SIP connection notifier
     */
    public void notifyRequest(SipConnectionNotifier scnLocal) {
        SipServerConnection ssc;

        // System.out.println(">>> Request received!");

        try {
            // block and wait for incoming request.
            // SipServerConnection is establised and returned
            // when new request is received.
            ssc = scnLocal.acceptAndOpen();
            assertNotNull("ssc is null", ssc);

            assertEquals("State should be REQUEST_RECEIVED",
                    SipServerConnectionImpl.REQUEST_RECEIVED,
                    ((SipServerConnectionImpl)ssc).getState());

            if (ssc.getMethod().equalsIgnoreCase("SUBSCRIBE")) {
                // write message body to respond back to client
                ssc.initResponse(200);

                assertEquals("State should be INITIALIZED",
                    SipServerConnectionImpl.INITIALIZED,
                    ((SipServerConnectionImpl)ssc).getState());

                sscB = ssc;
                ssc.send();
            } else if (ssc.getMethod().equalsIgnoreCase("NOTIFY")) {
                notifyReceived = true;

                synchronized (this) {
                    notifyAll();
                }

                // send a response
                ssc.initResponse(200);

                assertEquals("State should be INITIALIZED",
                    SipServerConnectionImpl.INITIALIZED,
                    ((SipServerConnectionImpl)ssc).getState());

                sscA = ssc;
                ssc.send();

                // wake up the thread waiting until a response is sent
                responseSent = true;

                synchronized (this) {
                    notifyAll();
                }
            } else {
                fail("Unexpected method: " + ssc.getMethod());
            }
        } catch (SipException sipex) {
            sipex.printStackTrace();
            fail("Unexpected SipException: " + sipex);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected Exception: " + e);
        }
    }

    /**
     * Notifier
     * @param scc client connection
     */
    public void notifyResponse(SipClientConnection scc) {
        try {
            // retrieve the response received
            scc.receive(0);

            // System.out.println(">>> Response received: " +
            //     scc.getStatusCode());

            if (scc.getStatusCode() == 200) {
               // handle 200 OK response
            } else {
               // handle possible error responses
               fail("Response code: " + scc.getStatusCode());
            }
        } catch (Exception ex) {
            // handle Exceptions
            fail("Exception '" + ex + "' was thrown.");
        }

        responseReceived = true;

        synchronized (this) {
            notifyAll();
        }
    }

    /**
     * Run the tests.
     */
    public void runTests() {
        setup();

        declare("Test for SUBSCRIBE request");
        testSubscribe();

        cleanup();
    }
}
