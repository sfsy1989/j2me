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
public class TestSipClientInitNotifyReq extends TestCase {
    

    /**
     * Body of the test 1.
     *
     * Try to initialization NOTIFY request out of dialog.
     * @param transport the name of transport protocol
     */
    void Test1(String transport) {
        String method = "NOTIFY";
        /** Client connection. */
        SipClientConnection sc = null;
        try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) 
            Connector.open("sip:sippy.tester@localhost:5060;transport="
                + transport);
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
            ex.printStackTrace();
        }
        assertNotNull("sc is null", sc);

        // Move to INITIALIZED state
        try {
            sc.initRequest(method, null);
            fail("IllegalArgumentException was not caused");
        } catch (IllegalArgumentException e) {
            assertTrue("OK", true);
        } catch (Throwable e) {
            fail("INITIALIZED " +e +" was caused");
        }

        // close connection
        try {
            sc.close();
        } catch (Throwable e) {
            fail("" +e +" was caused");
        }
    }
       
    /**
     * Tests execute
     *
     */
    public void runTests() {
    String arrProt[] = {"UDP", "TCP"};
    declare("Initialization NOTIFY request out of dialog");
        for (int i = 0; i < arrProt.length; i++) {
            Test1(arrProt[i]);
        }
    }


}

