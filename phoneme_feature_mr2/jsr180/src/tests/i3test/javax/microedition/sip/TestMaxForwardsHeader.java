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
 * RFC3261, p. 39; BNF: p. 230
 *
 * Max-Forwards = "Max-Forwards" HCOLON 1*DIGIT
 *
 * The Max-Forwards header field serves to limit the number of hops a
 * request can transit on the way to its destination.  It consists of an
 * integer that is decremented by one at each hop.  If the Max-Forwards
 * value reaches 0 before the request reaches its destination, it will
 * be rejected with a 483(Too Many Hops) error response.
 *
 * A UAC MUST insert a Max-Forwards header field into each request it
 * originates with a value that SHOULD be 70.  This number was chosen to
 * be sufficiently large to guarantee that a request would not be
 * dropped in any SIP network when there were no loops, but not so large
 * as to consume proxy resources when a loop does occur.  Lower values
 * should be used with caution and only in networks where topologies are
 * known by the UA.
 */

public class TestMaxForwardsHeader extends SipHeaderBaseTest {

    /** A name of the header that will be tested */
    private final String headerName = "Max-Forwards";

    /**
     * Body of the test 1.
     *
     * Test for Max-Forwards header field: setName()/getName().
     */
    void Test1() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test1 started ***");

        // Testing the constructor...
        // testConstructorNegative(headerName, "Invalid Value");

        SipHeader sh = createSipHeader(headerName, "100");

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
     * Test for Max-Forwards header field: getValue()/getHeaderValue().
     */
    void Test2() {
        String val;
        String headerValue = "10";

        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test2 started ***");

        SipHeader sh = createSipHeader(headerName, headerValue);

        if (sh != null) {
            val = sh.getValue();
            assertTrue("getValue() returned invalid value: '" +
                       val + "'", val.equals(headerValue));

            val = sh.getHeaderValue();
            assertTrue("(1) getHeaderValue() returned invalid " +
                "value: '" + val + "'", val.equals(headerValue));

            // Test if the value can be changed.
            headerValue = "70";
            sh.setValue(headerValue);

            val = sh.getHeaderValue();
            assertTrue("(2) getHeaderValue() returned invalid " +
                "value: '" + val + "'", val.equals(headerValue));
        }
    }

    /**
     * Body of the test 4.
     *
     * Test for Max-Forwards header field: getParameterNames()/getParameter().
     */
    void Test4() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test4 started ***");

        SipHeader sh = createSipHeader(headerName, "70");

        if (sh == null) {
            return;
        }

        // Testing getParameterNames()...
        String[] paramList = sh.getParameterNames();

        if (paramList != null) {
            fail("getParameterNames() should return null!");
        }

        // Testing getParameter()...
        String paramVal = sh.getParameter("ttl");
        assertTrue("getParameter() returned '" + paramVal +
            "' for the parameter 'ttl' that doesn't exist.", paramVal == null);
    }

    /**
     * Body of the test 5.
     *
     * Test for Max-Forwards header field: setParameter()/removeParameter().
     */
    void Test5() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test5 started ***");

        SipHeader sh = createSipHeader(headerName, "70");

        if (sh == null) {
            return;
        }

        // Testing setParameter()...
        try {
            sh.setParameter("test", "1");
        } catch (Exception e) {
            fail(e + " was thrown.");
        }

        try {
            sh.removeParameter("test");
        } catch (Exception e) {
            fail("removeParameter(): " + e + " was thrown!");
        }

        assertTrue(true); // to avoid error message from the test framework
    }

    /**
     * Run the tests
     */
    public void runTests() {
        declare("setName()/getName()");
        Test1();

        declare("getValue()/getHeaderValue()");
        Test2();

        declare("setValue()");
        testSetValue(headerName, "70");

        declare("getParameterNames()/getParameter()");
        Test4();

        declare("setParameter()/removeParameter()");
        Test5();

        declare("toString()");
        testToString(headerName, "70");
    }
}
