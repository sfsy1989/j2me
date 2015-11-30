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
//import javax.microedition.sip.SipConnectionNotifier;
import javax.microedition.sip.SipServerConnection;
import javax.microedition.sip.SipClientConnection;

//import gov.nist.siplite.message.*;
import com.sun.midp.i3test.TestCase;
import gov.nist.siplite.header.*;

/**
 * Test for support of short forms of headers names.
 */
public class TestShortHeaderNames extends SipHeaderBaseTest {

    /** Compact forms of headers names */
    private static String[] shortNames = {
        "i", "m", "e", "l", "c", "f", "s", "k", "t", "v", "o", "u", "a"
    };

    /** Full headers names */
    private static String[] fullNames = {
        Header.CALL_ID, Header.CONTACT, Header.CONTENT_ENCODING,
        Header.CONTENT_LENGTH, Header.CONTENT_TYPE, Header.FROM,
        Header.SUBJECT, Header.SUPPORTED, Header.TO, Header.VIA,
        Header.EVENT, Header.ALLOW_EVENTS, Header.ACCEPT_CONTACT
    };

    /** Headers values to set */
    private static String[] headerValues = {
        "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@biloxi.com",
        "\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>",
        "gzip",
        "12345",
        "application/sdp",
        "\"A. G. Bell\" <sip:agb@bell-telephone.com>",
        "Test of short header's name",
        "100rel",
        "<sip:+12125551212@server.phone2net.com>",
        "SIP/2.0/UDP erlang.bell-telephone.com:5060",
        "presence.test",
        "presence.test",
        "*"
    };

    /**
     * Opens a new SIP client connection and initializes Init request.
     * @return newly opened SipClientConnection
     */
    private SipClientConnection openClientConnection() {
        // Open SipClientConnection.
        SipClientConnection scc = null;

        try {
            scc = (SipClientConnection)
                Connector.open("sip:sippy.tester@localhost:5090");
        } catch (Exception ex) {
            fail("Exception during Connector.open(): " + ex);
        }

        assertNotNull("scc is null", scc);

        try {
            // Put the connection into the appropriate state
            // to call setHeader().
            scc.initRequest("INVITE", null);
        } catch (Exception ex) {
            fail("Exception during scc.initRequest(): " + ex);
        }

        return scc;
    }

    /**
     * Tests get/set/removeHeader() methods of SipConnection.
     * @param sc SIP commection to test
     */
    private void testGetSetHeader(SipConnection sc) {
        assertNotNull("sc is null", sc);

        try {
            String[] headerNamesToAdd, headerNamesToRemove;

            for (int j = 0; j < 2; j++) {
                if (j == 0) {
                    // Trying to add a header using its short name
                    // and then trying to remove it using its full name.
                    headerNamesToAdd = shortNames;
                    headerNamesToRemove = fullNames;
                } else {
                    // Use full header's name to add the header,
                    // then use its short name to remove the header.
                    headerNamesToAdd = fullNames;
                    headerNamesToRemove = shortNames;
                }

                // Test set/getHeader().
                for (int i = 0; i < headerNamesToAdd.length; i++) {
                    sc.setHeader(headerNamesToAdd[i], headerValues[i]);

                    // Check that getHeader() returns correct values
                    // both for short and long form of header's name.
                    String val = sc.getHeader(headerNamesToAdd[i]);
                    assertTrue("Invalid '" + headerNamesToAdd[i] +
                        "' header's value: '" + val + "'",
                        headerValues[i].equalsIgnoreCase(val));

                    String name = sc.getHeader(headerNamesToRemove[i]);
                    assertTrue("Invalid '" + headerNamesToRemove[i] +
                        "' header's value: '" + val + "'",
                        headerValues[i].equalsIgnoreCase(val));

                    // Remove the header and check that getHeader()
                    // will return null.
                    sc.removeHeader(headerNamesToRemove[i]);

                    val = sc.getHeader(headerNamesToAdd[i]);
                    assertNull("getHeader(\"" + headerNamesToAdd[i] +
                        "\") returned non-null value!", val);

                    val = sc.getHeader(headerNamesToRemove[i]);
                    assertNull("getHeader(\"" + headerNamesToRemove[i] +
                        "\") returned non-null value!", val);
                } // end for i
            } // end for j
        } catch (Exception ex) {
            fail("Exception during set/getHeader(): " + ex);
        }
    }

    /**
     * Body of the test 1: creating SipHeader with short header's name test.
     */
    void Test1() {
        SipHeader sh;

        for (int i = 0; i < shortNames.length; i++) {
            sh = createSipHeader(shortNames[i], headerValues[i]);

            if (sh == null) {
                return;
            }

            assertEquals("Invalid header name: " + sh.getName(),
                shortNames[i], sh.getName());

            assertEquals("Invalid header value!",
                headerValues[i], sh.getValue());
        } // end for
    }

    /**
     * Body of the test 2: SipClientConnection methods test.
     */
    void Test2() {
        SipClientConnection scc = openClientConnection();
        testGetSetHeader(scc);

        try {
            if (scc != null) {
                scc.close();
            }
        } catch (Exception ex) {
        }
    }

    /**
     * Body of the test 3: SipServerConnection methods test.
     */
    void Test3() {
        SipServerConnection ssc = null;
        SipClientConnection scc = openClientConnection();

        // Open SipServerConnection.
        try {
            SipConnectionNotifier scn =
                (SipConnectionNotifier)Connector.open("sip:5090");
            scc.send();

            ssc = scn.acceptAndOpen();
            ssc.initResponse(200);
        } catch (Exception ex) {
            fail("Exception during ssc open: " + ex);
        }

        testGetSetHeader(ssc);
    }

    /**
     * Run the tests.
     */
    public void runTests() {
        declare("Creating SipHeader with short header's name test");
        Test1();

        declare("SipClientConnection methods test");
        Test2();

        // Disabled until SipServerConnection.get/setHeader() will be modified.

        // declare("SipServerConnection methods test");
        // Test3();
    }
}
