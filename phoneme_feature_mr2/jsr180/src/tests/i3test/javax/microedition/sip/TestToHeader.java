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
 * RFC3261, p. 178
 *
 * The To header field specifies the logical recipient of the request.
 *
 * The optional "display-name" is meant to be rendered by a human-user
 * interface.  The "tag" parameter serves as a general mechanism for
 * dialog identification.
 *
 * See Section 19.3 for details of the "tag" parameter.
 *
 * Comparison of To header fields for equality is identical to
 * comparison of From header fields.  See Section 20.10 for the rules
 * for parsing a display name, URI and URI parameters, and header field
 * parameters.
 *
 * The compact form of the To header field is t.
 *
 * The following are examples of valid To header fields:
 *
 *    To: The Operator <sip:operator@cs.columbia.edu>;tag=287447
 *    t: sip:+12125551212@server.phone2net.com
 *
 */

public class TestToHeader extends SipHeaderBaseTest {

    /** A name of the header that will be tested */
    private final String headerName = "To";

    /**
     * Body of the test 1.
     *
     * Test for To header field: setName()/getName().
     */
    void Test1() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test1 started ***");

        // Testing the constructor...
        // testConstructorNegative(headerName, "Invalid Value");

        SipHeader sh = createSipHeader(headerName, "The Operator" +
            " <sip:operator@cs.columbia.edu>;tag=287447");

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
     * Test for To header field: getValue()/getHeaderValue().
     */
    void Test2() {
        SipHeader sh;
        String val;
        String headerValue1 = "The Operator <sip:operator@cs.columbia.edu>";
        String headerValue2 = "<sip:+12125551212@server.phone2net.com>";
        String headerParam1 = headerValue1;
        String headerParam2 = headerValue2;
        String[] paramList = {
            "tag=287447",
            "generic=sample_value"
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
     * Test for To header field: getParameterNames()/getParameter().
     */
    void Test4() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test4 started ***");

        SipHeader sh = createSipHeader(headerName, "The Operator" +
            " <sip:operator@cs.columbia.edu>;tag=287447");

        if (sh == null) {
            return;
        }

        // Testing getParameterNames()...
        String[] paramList = sh.getParameterNames();
        if (paramList == null) {
            fail("getParameterNames() returned null!");
        } else {
            assertTrue("getParameterNames() returned " + paramList.length +
                       " parameters instead of 1.", paramList.length == 1);

            assertTrue("Invalid parameter name: " + paramList[0],
                       paramList[0].equals("tag"));
        }

        // Testing getParameter()...
        String paramVal = sh.getParameter("q");
        assertTrue("getParameter() returned '" + paramVal +
            "' for the parameter 'q' that doesn't exist.", paramVal == null);

        paramVal = sh.getParameter("tag");
        assertTrue("getParameter() returned '" + paramVal +
            "' for 'tag' instead of 287447.", paramVal.equals("287447"));
    }

    /**
     * Body of the test 5.
     *
     * Test for To header field: setParameter()/removeParameter().
     */
    void Test5() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test5 started ***");

        SipHeader sh = createSipHeader(headerName, "The Operator" +
            " <sip:operator@cs.columbia.edu>;tag=287447");

        if (sh == null) {
            return;
        }

        // Testing setParameter()...
        sh.setParameter("tag", "12345");

        String paramVal = sh.getParameter("tag");
        assertTrue("getParameter() returned '" + paramVal +
                   "' instead of 12345.", paramVal.equals("12345"));

        sh.setParameter("q", "10"); // parameter doesn't exist

        paramVal = sh.getParameter("q");
        assertTrue("getParameter() returned '" + paramVal +
                   "' instead of 10.", paramVal.equals("10"));
    }

    /**
     * Run the tests
     */
    public void runTests() {
        String headerParam = "The Operator " +
            "<sip:operator@cs.columbia.edu>;tag=287447";

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
