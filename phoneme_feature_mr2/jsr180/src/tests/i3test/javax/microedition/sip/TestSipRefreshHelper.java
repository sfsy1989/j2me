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

import javax.microedition.io.Connector;
import com.sun.midp.i3test.TestCase;
import gov.nist.microedition.sip.SipServerConnectionImpl;

import java.io.OutputStream;

/**
 * Tests for SipRefreshHelper class.
 * sendRegister() example was taken from JSR180 spec.
 */
public class TestSipRefreshHelper extends TestCase
        implements SipClientConnectionListener, SipServerConnectionListener,
            SipRefreshListener {

    /** Refresh identifier */
    int refreshID = 0;

    /** Refresh status */
    int refreshStatus = 0;

    /** Refresh helper */
    SipRefreshHelper refHelper = null;

    /**
     * Sends 'REGISTER' request.
     */
    public void sendRegister() {
        SipClientConnection scc = null;
        SipConnectionNotifier scn = null;

        try {
            // Open SIP server connection and listen to port 5060
            scn = (SipConnectionNotifier)Connector.open("sip:5060");
            scn.setListener(this);

            // Initialize connection to the registrar host.com
            scc = (SipClientConnection)Connector.open("sip:localhost");
            scc.setListener(this);

            // Initialize request and set From, To and Contact headers
            scc.initRequest("REGISTER", null);
            scc.setHeader("From", "sip:sippy.user@localhost");
            scc.setHeader("To", "sip:sippy.user@localhost");
            scc.setHeader("Contact",
                          "<sip:UserB@192.168.200.201>;expires=3600");
            scc.addHeader("Contact",
                          "<mailto:UserB@biloxi.com>;expires=4294967295");

            refreshID = scc.enableRefresh(this);
            assertTrue("Invalid refreshID!", refreshID != 0);

            // System.out.println(">>> id = " + refreshID);

            scc.send();

            refHelper = SipRefreshHelper.getInstance();
            assertNotNull("refHelper is null!", refHelper);

            // -----------------------------
            //  do something else for a while
            // ------------------------------
            synchronized (this) {
                try {
                    wait(1200);
                } catch (Exception e) {
                    System.out.println(">>> Exception!!! " + e);
                }
            }

            // System.out.println(">>> (1) status = " + refreshStatus);

            // check that refresh was successful
            assertEquals("Invalid refresh status (1)!", 200, refreshStatus);

            // update REGISTER, with new "mailto:" Contact and no content
            String c[] = { "<mailto:UserB@company.com>" };
            refHelper.update(refreshID, c, null, 0, 6000);

            // -----------------------------
            //  do something else for a while
            // ------------------------------
            synchronized (this) {
                try {
                    wait(1200);
                } catch (Exception e) {
                    System.out.println(">>> Exception!!! " + e);
                }
            }

            // System.out.println(">>> (2) status = " + refreshStatus);

            // check that refresh is still ok
            assertEquals("Invalid refresh status (2)!", 200, refreshStatus);

            // stop REGISTER refresh altogether
            refHelper.stop(refreshID);
        } catch (Exception ex) { // handle Exceptions
            ex.printStackTrace();
            fail("Exception '" + ex + "' was thrown.");
        }
    }

    /**
     * Accept a new connection request and process the request from client.
     * Send "OK" to client
     *
     * This method is declared in SipServerConnectionListener
     * The code was copied from TestSipOriginatingInvite
     *
     * @param scnLocal Local SIP connection notifier
     */
    public void notifyRequest(SipConnectionNotifier scnLocal) {
        SipServerConnection ssc = null;

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

            if (ssc.getMethod().equals("REGISTER")) {
                /*
                 * RFC 3261, p. 66:
                 *
                 * The registrar returns a 200 (OK) response. The response MUST
                 * contain Contact header field values enumerating all current
                 * bindings. Each Contact value MUST feature an "expires"
                 * parameter indicating its expiration interval chosen by the
                 * registrar. The response SHOULD include a Date header field.
                 */

                int i, index;
                String[] contacts = ssc.getHeaders("Contact");
                assertNotNull("contact is null", contacts);
                assertTrue("Invalid Contact header length: " + contacts.length,
                    contacts.length > 0);

                String[] newContacts = new String[contacts.length];
                assertNotNull("newContact is null", newContacts);

                for (i = 0; i < contacts.length; i++) {
                    index = contacts[i].indexOf(';');

                    // System.out.println(">>> " + contacts[i]);

                    if (index == -1) {
                        newContacts[i] = contacts[i];
                    } else {
                        newContacts[i] = contacts[i].substring(0, index);
                    }

                    newContacts[i] += ";expires=1";
                }

                // write message body to respond back to client
                ssc.initResponse(200);

                assertEquals("State should be INITIALIZED",
                    SipServerConnectionImpl.INITIALIZED,
                    ((SipServerConnectionImpl)ssc).getState());

                // remove the default Contact header
                ssc.removeHeader("Contact");

                // DEBUG
                /*
                contacts = ssc.getHeaders("Contact");
                for (i = 0; i < contacts.length; i++) {
                    System.out.println(">>> " + contacts[i]);
                }
                */
                //

                ssc.send();
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
    }

    /**
     * Refresh callback.
     * @param ID refresh ID
     * @param statusCode refresh status
     * @param reasonPhrase
     */
    public void refreshEvent(int ID, int statusCode, String reasonPhrase) {
        // System.out.println(">>> Refresh event, code " + statusCode);

        refreshStatus = statusCode;

        if (statusCode == 0) {
            // stopped refresh
            // fail("stopped refresh!");
        } else if (statusCode == 200) {
            // successful refresh
        } else {
            // failed request
            fail("failed request, status: " + statusCode);
        }
    }

    /**
     * Run the tests.
     */
    public void runTests() {
        declare("SipRefreshHelper test: sendRegister()");
        sendRegister();
    }
}
