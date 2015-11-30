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
import gov.nist.microedition.sip.SipDialogImpl;
import gov.nist.siplite.header.ContactHeader;
import gov.nist.core.ParseException;
import java.io.OutputStream;
import com.sun.midp.i3test.TestCase;
/**
 * Tests for SipDialog(server side) class.
 *
 */
public class TestSipDialogServer extends TestCase {
    /** SIP dialog */
    SipDialog dialog;
    /** Client connection */
    SipClientConnection scc;
    /** Connection Notifier */
    SipConnectionNotifier scn;
   
    /**
     * Test SipDialog for INVITE 
     */ 
    public void TestInvite() {
        try {
            scn = (SipConnectionNotifier) Connector.open("sip:5060");
            scc = (SipClientConnection)
            Connector.open("sip:alice@localhost:5060");
            scc.initRequest("INVITE", scn);
            
            scc.setHeader("From", "sip:sippy.tester@localhost");
            scc.setHeader("Subject", "Hello...");
            scc.setHeader("Contact", "sip:user@"+scn.getLocalAddress() +
                    ":" + scn.getLocalPort());
            
            // write message body
            scc.setHeader("Content-Type", "text/plain");
            String clientMsg = "Hello, how are you?";
            scc.setHeader("Content-Length",
                    Integer.toString(clientMsg.length()));
            OutputStream os = scc.openContentOutputStream();
            os.write(clientMsg.getBytes());
            os.close(); // close stream and send the message
            
            notifyInviteRequest(scn);
            boolean resp = scc.receive(10000); // wait 10 secs for response
            if (resp) {
                if (scc.getStatusCode() == 200) {
                    // System.out.println("Received OK for INVITE");
                }
            }
            scn.close();
            scc.close();
        } catch (Exception ex) {
            // handle Exceptions
            System.out.println("Exception received");
            ex.printStackTrace();
        }
        assertTrue("Test1 done", true);
    }
    
    /**
     * Process INVITE request at UAS
     * @param scn SipConnectionNotifier at UAS
     */
    public void notifyInviteRequest(SipConnectionNotifier scn) {
        try {
            SipServerConnection ssc;
            // retrieve the request received
            ssc = scn.acceptAndOpen();
            if (ssc.getMethod().equals("INVITE")) {
                SipDialog dialog = ssc.getDialog();
                assertTrue("Dialog is not null", dialog == null);
                ssc.initResponse(100);
                ssc.send();
                dialog = ssc.getDialog();
                assertTrue("Dialog is null after sending 100 TRYING", 
                            dialog != null);
                assertTrue("Dialog is not in EARLY state",
                        dialog.getState() == SipDialog.EARLY);
                /* 
                ssc.initResponse(200);
                ssc.send();
                assertTrue("Dialog is null after sending 200 OK", 
                            dialog != null);
                assertTrue("Dialog is not in CONFIRMED state",
                        dialog.getState() == SipDialog.CONFIRMED);
                */
            }
        } catch (Exception ex) {
            // handle Exceptions
        }
    }
    
    /**
     * Test SipDialog for SUBSCRIBE 
     */ 
    public void TestSubscribe() {
        try {
            scn = (SipConnectionNotifier) Connector.open("sip:5060");
            scc = (SipClientConnection)
            Connector.open("sip:alice@localhost:5060");
            scc.initRequest("SUBSCRIBE", scn);
            scc.setHeader("From", "sip:UserA@host.com");
            scc.setHeader("Accept", "application/pidf+xml");
            scc.setHeader("Event", "presence");
            scc.setHeader("Expires", "950");
            String contact = new String("sip:user@"+scn.getLocalAddress() + 
                    ":"+scn.getLocalPort());
            scc.setHeader("Contact", contact);
            scc.send();
            notifySubscribeRequest(scn);
            boolean resp = scc.receive(10000); // wait 10 secs for response
            if (resp) {
                if (scc.getStatusCode() == 200) {
                    // System.out.println("Received OK for subscribe");
                    /*
                    dialog = scc.getDialog();
                    // initialize new SipClientConnection
                    scc = dialog.getNewClientConnection("SUBSCRIBE");
                    // read dialog Call-ID
                    callID = scc.getHeader("Call-ID");
                    // read remote tag
                    SipHeader sh = new SipHeader("To", scc.getHeader("To"));
                    remoteTag = sh.getParameter("tag");
                    // unSUBSCRIBE
                    scc.setHeader("Expires", "0");
                    scc.send();
                    */
                }
            } else {
                // didn't receive any response in given time
            }
            scn.close();
            scc.close();
        } catch (Exception ex) {
            // handle Exceptions
            System.out.println("Exception received");
            ex.printStackTrace();
        }
        assertTrue("Test1 done", true);
    }
    
    /**
     * Process SUBSCRIBE request at UAS
     * @param scn SipConnectionNotifier at UAS
     */
    public void notifySubscribeRequest(SipConnectionNotifier scn) {
        try {
            SipServerConnection ssc;
            // retrieve the request received
            ssc = scn.acceptAndOpen();
            if (ssc.getMethod().equals("SUBSCRIBE")) {
                SipDialog dialog = ssc.getDialog();
                assertTrue("Dialog is not null", dialog == null);
                ssc.initResponse(200);
                ssc.send();
                dialog = ssc.getDialog();
                assertTrue("Dialog is null after sending 200 OK", 
                            dialog != null);
                assertTrue("Dialog is not in CONFIRMED state",
                        dialog.getState() == SipDialog.CONFIRMED);
            }
        } catch (Exception ex) {
            // handle Exceptions
        }
    }
    
    /**
     * Tests execute
     *
     */
    public void runTests() {
        declare("INVITE request");
        TestInvite();
        declare("SUBSCRIBE request");
        TestSubscribe();
    }
    
}

