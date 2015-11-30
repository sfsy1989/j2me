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
import javax.microedition.sip.SipClientConnection;
import java.io.OutputStream;
import java.io.IOException;
import javax.microedition.sip.SipException;

/**
 * Test Multiple SipConnectionNotifiers on single host
 *
 */
public class TestMultiNotifiers extends TestCase implements 
        SipServerConnectionListener {

    /** Connection notifier. */
    SipConnectionNotifier scn1 = null;
    /** Connection notifier. */
    SipConnectionNotifier scn2 = null;
    /** Client connection. */
    SipClientConnection scc1 = null;
    /** Client connection. */
    SipClientConnection scc2 = null;
    /** SIP request message from client to server */
    String clientMsg = "Client : How are you?";

    /**
     * Open SipConnectionNotifier(SCN1 and SCN2) on ports 6080 and 5081.
     * Open two client connections connecting to SCN1 and SCN2 
     */
    void setup() {
        try {
            // Open SipConnectionNotifier on port 6080
            scn1 = (SipConnectionNotifier) Connector.open("sip:6080");
            assertNotNull("scn1 is null", scn1);
            scn1.setListener(this);
        } catch (Exception ex) {
            assertNull("Exception during scn1 open", scn1);
            ex.printStackTrace();
        }
          
        try { 
            // Open SipConnectionNotifier on port 6081
            scn2 = (SipConnectionNotifier) Connector.open("sip:6081");
            assertNotNull("scn2 is null", scn2);
            scn2.setListener(this);
        } catch (Exception ex) {
            assertNull("Exception during scn2 open", scn2);
            ex.printStackTrace();
        }

        try {
            // Open SIP client connection to the SCN ON PORT 6080
            scc1 = (SipClientConnection) 
                Connector.open("sip:sippy.tester@localhost:6080");
            assertNotNull("scc1 is null", scc1);
        } catch (Exception ex) {
            assertNull("Exception during scc1 open", scc1);
            ex.printStackTrace();
            cleanup();
        }
        
        try {
            // Open SIP client connection to the SCN ON PORT 6081
            scc2 = (SipClientConnection) 
                Connector.open("sip:sippy.tester@localhost:5081");
            assertNotNull("scc2 is null", scc2);
        } catch (Exception ex) {
            assertNull("Exception during scc2 open", scc2);
            ex.printStackTrace();
            cleanup();
        }

    }

    /**
     * Send a SIP INVITE message 
     * @param scc SIP client connection
     * @param snc SIP Connection Notifier
     *
     */
    public void testInviteMsg(SipClientConnection scc, 
            SipConnectionNotifier scn) {
        
        try {
            scc.initRequest("INVITE", scn);
            scc.setHeader("From", "sip:sippy.tester@localhost");
            scc.setHeader("Subject", "Hello...");
            scc.setHeader("Contact", "sip:user@"+scn.getLocalAddress() + 
                    ":" + scn.getLocalPort()); 

            // write message body
            scc.setHeader("Content-Type", "text/plain");
            scc.setHeader("Content-Length", 
                    Integer.toString(clientMsg.length()));
            OutputStream os = scc.openContentOutputStream();
            os.write(clientMsg.getBytes());
            os.close(); // close stream and send the message
       
            // System.out.println("Inviting..." + scc.getHeader("To"));
        } catch (IOException ioe) {
            ioe.printStackTrace();
            cleanup();
            fail("IOException in TestMultiNotifiers");
        } 

    }

    /**
     * Receive request from client
     *
     * This method is declared in SipServerConnectionListener
     * 
     * @param scnLocal Local SIP connection notifier
     */
    public void notifyRequest(SipConnectionNotifier scnLocal) {
        try {
            System.out.println("Local SCN port = " + scnLocal.getLocalPort());
        } catch (IOException ioe) {
            ioe.printStackTrace();
            fail("IOException in TestMultiNotifiers");
        } 

    }

    /**
     * Close all connections.
     *
     */
    void cleanup() {
        try {
            if (scc1 != null) {
                scc1.close();
            }
            if (scc2 != null) {
                scc2.close();
            }

            if (scn1 != null) {
                scn1.close();
            }
            if (scn2 != null) {
                scn2.close();
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
        declare("Test TestMultiNotifiers ");

        setup();

        testInviteMsg(scc1, scn1);        
        synchronized (this) {
            try {
                wait(500);
            } catch (InterruptedException e) {
                System.out.println("Catch interrupt");
            }
        }

        testInviteMsg(scc2, scn2);        
        synchronized (this) {
            try {
                wait(500);
            } catch (InterruptedException e) {
                System.out.println("Catch interrupt");
            }
        }

        cleanup();
    
        assertTrue("Failure in TestMultiNotifiers", true);
    }

}


