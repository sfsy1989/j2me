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
import java.io.IOException;

/**
 * RFC3261, p. 179; BNF: p. 232
 *
 * The Via header field indicates the path taken by the request so far
 * and indicates the path that should be followed in routing responses.
 * The branch ID parameter in the Via header field values serves as a
 * transaction identifier, and is used by proxies to detect loops.
 *
 * A Via header field value contains the transport protocol used to send
 * the message, the client's host name or network address, and possibly
 * the port number at which it wishes to receive responses.  A Via
 * header field value can also contain parameters such as "maddr",
 * "ttl", "received", and "branch", whose meaning and use are described
 * in other sections.  For implementations compliant to this
 * specification, the value of the branch parameter MUST start with the
 * magic cookie "z9hG4bK", as discussed in Section 8.1.1.7.
 *
 * Transport protocols defined here are "UDP", "TCP", "TLS", and "SCTP".
 * "TLS" means TLS over TCP.  When a request is sent to a SIPS URI, the
 * protocol still indicates "SIP", and the transport protocol is TLS.
 *
 * Via: SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87asdks7
 * Via: SIP/2.0/UDP 192.0.2.1:5060 ;received=192.0.2.207 ;branch=z9hG4bK77asjd
 *
 * The compact form of the Via header field is v.
 *
 * In this example, the message originated from a multi-homed host with
 * two addresses, 192.0.2.1 and 192.0.2.207.  The sender guessed wrong
 * as to which network interface would be used.  Erlang.bell-
 * telephone.com noticed the mismatch and added a parameter to the
 * previous hop's Via header field value, containing the address that
 * the packet actually came from.
 *
 * The host or network address and port number are not required to
 * follow the SIP URI syntax.  Specifically, LWS on either side of the
 * ":" or "/" is allowed, as shown here:
 *
 * Via: SIP / 2.0 / UDP first.example.com: 4000;ttl=16
 *   ;maddr=224.2.0.1 ;branch=z9hG4bKa7c6a8dlze.1
 *
 */

public class TestViaHeader extends SipHeaderBaseTest {

    /** A name of the header that will be tested */
    private final String headerName = "Via";

    /**
     * Body of the test 1.
     *
     * Test for Via header field: setName()/getName().
     */
    void Test1() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test1 started ***");

        SipHeader sh = createSipHeader(headerName,
            "SIP/2.0/UDP first.example.com: 4000;ttl=16; " +
            "maddr=224.2.0.1 ;branch=z9hG4bKa7c6a8dlze.1");

        if (sh == null) {
            return;
        }

        // Testing getName()...
        String ret_name = sh.getName();
        assertTrue("Invalid header value: " + ret_name,
            ret_name.equals(headerName));

        // Testing setName()...
        try {
           sh.setName(headerName);
        } catch (java.lang.IllegalArgumentException e) {
            fail("setName(" + headerName + ") failed (IAE): " + e);
        } catch (Throwable e) {
            fail("setName(" + headerName + ") failed: " + e);
        }
    }

    /**
     * Body of the test 2.
     *
     * Test for Via header field: getValue()/getHeaderValue().
     */
    void Test2() {
        SipHeader sh;
        String val;
        String headerValue1 = "SIP/2.0/UDP first.example.com:4000";
        String headerValue2 = "SIP/2.0/UDP 192.0.2.1:5060";
        String headerParam1 = headerValue1;
        String headerParam2 = headerValue2;
        String[] paramList = {
            "ttl=16",
            "maddr=224.2.0.1",
            "branch=z9hG4bKa7c6a8dlze.1"
        };

        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test2 started ***");

        for (int i = 0; i < paramList.length + 1; i++) {
            sh = createSipHeader(headerName, headerParam1);

            if (sh != null) {
                val = sh.getValue();
                assertTrue("getValue() returned invalid parameter value: '" +
                           val + "'", val.equals(headerValue1));

                val = sh.getHeaderValue();
                assertTrue("(1) getHeaderValue() returned invalid " +
                    "parameter value: '" + val + "'", val.equals(headerParam1));

                // Test if the value can be changed.
                sh.setValue(headerValue2);

                val = sh.getHeaderValue();
                assertTrue("(2) getHeaderValue() returned invalid " +
                    "parameter value: '" + val + "'", val.equals(headerParam2));
            }

            headerParam1 += ";";
            headerParam2 += ";";

            if (i < paramList.length) {
                headerParam1 += paramList[i];
                headerParam2 += paramList[i];
            }
        } // end for
    }

    /**
     * Body of the test 4.
     *
     * Test for Via header field: getParameterNames()/getParameter().
     */
    void Test4() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test4 started ***");

        SipHeader sh = createSipHeader(headerName,
            "SIP/2.0/UDP 192.0.2.1:5060 ;received=192.0.2.207;" +
            "branch=z9hG4bK77asjd");

        if (sh == null) {
            return;
        }

        // Testing getParameterNames()...
        String[] paramList = sh.getParameterNames();

        if (paramList == null) {
            fail("getParameterNames() returned null!");
        } else {
            assertTrue("getParameterNames() returned " + paramList.length +
                       " parameters instead of 2.", paramList.length == 2);

            boolean isValid0 = paramList[0].equals("received") ||
                               paramList[0].equals("branch");

            assertTrue("Invalid parameter name: " + paramList[0],
                       isValid0);

            boolean isValid1 = paramList[1].equals("received") ||
                               paramList[1].equals("branch");

            assertTrue("Invalid parameter name: " + paramList[1],
                       isValid1);
        }

        // Testing getParameter()...
        String paramVal = sh.getParameter("ttl");
        assertTrue("getParameter() returned '" + paramVal +
            "' for the parameter 'ttl' that doesn't exist.", paramVal == null);

        paramVal = sh.getParameter("received");
        assertTrue("getParameter() returned '" + paramVal + "' for 'received'" +
            " instead of '192.0.2.207'.", paramVal.equals("192.0.2.207"));
    }

    /**
     * Body of the test 5.
     *
     * Test for Via header field: setParameter()/removeParameter().
     */
    void Test5() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test5 started ***");

        SipHeader sh = createSipHeader(headerName,
            "SIP/2.0/UDP first.example.com: 4000;" +
            "maddr=224.2.0.1 ;branch=z9hG4bKa7c6a8dlze.1");

        if (sh == null) {
            return;
        }

        // Testing setParameter()...
        sh.setParameter("maddr", "192.0.0.1");

        String paramVal = sh.getParameter("maddr");
        assertTrue("getParameter() returned '" + paramVal +
                   "' instead of '192.0.0.1'.", paramVal.equals("192.0.0.1"));

        sh.setParameter("ttl", "10"); // parameter doesn't exist

        paramVal = sh.getParameter("ttl");
        assertTrue("getParameter() returned '" + paramVal +
                   "' instead of 10.", paramVal.equals("10"));

        // RFC 3261, p. 232: ttl = 1*3DIGIT ; 0 to 255
        /*
           These checks are disabled according to the clarification
           for SipHeader received from the Expert Group.

        try {
            sh.setParameter("ttl", "300");
            fail("Setting 'ttl=300': IAE was not thrown.");
        } catch (IllegalArgumentException iae) {
        } catch (Exception e) {
            fail(e + " was thrown instead of IAE.");
        }

        try {
            sh.setParameter("ttl", "invalid");
            fail("Setting 'ttl=invalid': IAE was not thrown.");
        } catch (IllegalArgumentException iae) {
        } catch (Exception e) {
            fail(e + " was thrown instead of IAE.");
        }

        // RFC 3261, p. 232:
        // via-received = "received" EQUAL (IPv4address / IPv6address)
        try {
            sh.setParameter("received", "invalid");
            fail("Setting 'received=invalid': IAE was not thrown.");
        } catch (IllegalArgumentException iae) {
        } catch (Exception e) {
            fail(e + " was thrown instead of IAE.");
        }
        */
    }

    /**
     * Run the tests
     */
    public void runTests() {
        String headerParam =
            "SIP/2.0/UDP first.example.com:4000;ttl=16; " +
            "maddr=224.2.0.1 ;branch=z9hG4bKa7c6a8dlze.1";

        declare("setName()/getName()");
        Test1();

        declare("getValue()/getHeaderValue()");
        Test2();

        declare("setValue()");
        testSetValue(headerName, headerParam);

        declare("getParameterNames()/getParameter()");
        Test4();

        declare("setParameter()/removeParameter()");
        Test5();

        declare("toString()");
        testToString(headerName, headerParam);
    }
}
