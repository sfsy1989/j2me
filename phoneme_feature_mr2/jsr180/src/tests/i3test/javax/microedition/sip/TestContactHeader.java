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
 * RFC3261, p. 167
 * A Contact header field value provides a URI whose meaning depends on
 * the type of request or response it is in.
 *
 * A Contact header field value can contain a display name, a URI with
 * URI parameters, and header parameters.
 *
 * This document defines the Contact parameters "q" and "expires".
 * These parameters are only used when the Contact is present in a
 * REGISTER request or response, or in a 3xx response.  Additional
 * parameters may be defined in other specifications.
 *
 * When the header field value contains a display name, the URI
 * including all URI parameters is enclosed in "<" and ">".  If no "<"
 * and ">" are present, all parameters after the URI are header
 * parameters, not URI parameters.  The display name can be tokens, or a
 * quoted string, if a larger character set is desired.
 *
 * Even if the "display-name" is empty, the "name-addr" form MUST be
 * used if the "addr-spec" contains a comma, semicolon, or question
 * mark.  There may or may not be LWS between the display-name and the
 * "<".
 *
 * These rules for parsing a display name, URI and URI parameters, and
 * header parameters also apply for the header fields To and From.
 *
 * The Contact header field has a role similar to the Location header
 * field in HTTP.  However, the HTTP header field only allows one
 * address, unquoted.  Since URIs can contain commas and semicolons
 * as reserved characters, they can be mistaken for header or
 * parameter delimiters, respectively.
 *
 * The compact form of the Contact header field is m (for "moved").
 *
 * Examples:
 *
 *    Contact: "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
 *       ;q=0.7; expires=3600,
 *       "Mr. Watson" <mailto:watson@bell-telephone.com> ;q=0.1
 *    m: <sips:bob@192.0.2.4>;expires=60
 */

public class TestContactHeader extends SipHeaderBaseTest {

    /** A name of the header that will be tested */
    private final String headerName = "Contact";

    /**
     * Body of the test 1.
     *
     * Test for Contact header field: setName()/getName().
     */
    void Test1() {
	// DEBUG:        System.out.println("");
	// DEBUG:        System.out.println("*** Test1 started ***");

        SipHeader sh = createSipHeader(headerName, "\"Mr. Watson\" " +
            "<sip:watson@worcester.bell-telephone.com>");

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
     * Test for Contact header field: getValue()/getHeaderValue().
     */
    void Test2() {
        SipHeader sh;
        String val;
        String headerValue = "\"Mr. Watson\" " +
                              "<sip:watson@worcester.bell-telephone.com>";
        String headerParam = headerValue;
        String[] paramList = {
            "expires=3600",
            "q=0.7"
        };

	// DEBUG:        System.out.println("");
	// DEBUG:        System.out.println("*** Test2 started ***");

        for (int i = 0; i < paramList.length + 1; i++) {
            sh = createSipHeader(headerName, headerParam);

            if (sh != null) {
                val = sh.getValue();
                assertTrue("getValue() returned invalid parameter value: '" +
                           val + "'", val.equals(headerValue));

                val = sh.getHeaderValue();
                assertTrue("getHeaderValue() returned invalid " +
                    "parameter value: '" + val + "'", val.equals(headerParam));
            }

            headerParam += ";";

            if (i < paramList.length) {
                headerParam += paramList[i];
            }
        } // end for
    }

    /**
     * Body of the test 3.
     *
     * Test for Contact header field: setValue().
     */
    void Test3() {
	// DEBUG:        System.out.println("");
	// DEBUG:        System.out.println("*** Test3 started ***");

        SipHeader sh = createSipHeader(headerName, "\"Mr. Watson\" " +
            "<sip:watson@worcester.bell-telephone.com>");

        if (sh == null) {
            return;
        }

        // Testing setValue()...
        String headerValue = "<sips:bob@192.0.2.4>;expires=60";

        try {
            // try to set a value with a parameter
            sh.setValue(headerValue);
            fail("IllegalArgumentException was not thrown!");
        } catch (java.lang.IllegalArgumentException e) {
        } catch (Throwable e) {
            fail("" + e + " was caught");
        }

        // try to set a value without parameters
        headerValue = "<sips:bob@192.0.2.4>";
        try {
            sh.setValue(headerValue);
        } catch (Throwable e) {
            fail("" + e + " was caught");
        }

        String realValue = sh.getValue();
        assertTrue("setValue() set '" + realValue + "' instead of '" +
                   headerValue + "'.", realValue.equals(headerValue));

        // TODO: investigate this case!
        /*
        headerValue = "INVALID VALUE";
        try {
            sh.setValue(headerValue);
            fail("IllegalArgumentException was not caught!");
        } catch (java.lang.IllegalArgumentException e) {
        } catch (Throwable e) {
            fail("" + e + " was caught");
        }
        */
    }

    /**
     * Body of the test 4.
     *
     * Test for Contact header field: getParameterNames()/getParameter().
     */
    void Test4() {
	// DEBUG:        System.out.println("");
	// DEBUG:        System.out.println("*** Test4 started ***");

        SipHeader sh = createSipHeader(headerName,
                                       "<sips:bob@192.0.2.4>;expires=60");

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
                       paramList[0].equals("expires"));
        }

        // Testing getParameter()...
        String paramVal = sh.getParameter("q");
        assertTrue("getParameter() returned '" + paramVal +
            "' for the parameter 'q' that doesn't exist.", paramVal == null);

        paramVal = sh.getParameter("expires");
        assertTrue("getParameter() returned '" + paramVal +
            "' for 'expires' instead of 60.", paramVal.equals("60"));
    }

    /**
     * Body of the test 5.
     *
     * Test for Contact header field: setParameter()/removeParameter().
     */
    void Test5() {
	// DEBUG:        System.out.println("");
	// DEBUG:        System.out.println("*** Test5 started ***");

        SipHeader sh = createSipHeader(headerName,
                                       "<sips:bob@192.0.2.4>;expires=60");

        if (sh == null) {
            return;
        }

        // Testing setParameter()...
        sh.setParameter("expires", "3600");

        String paramVal = sh.getParameter("expires");
        assertTrue("getParameter() returned '" + paramVal +
                   "' instead of 3600.", paramVal.equals("3600"));

        sh.setParameter("q", "10"); // parameter doesn't exist

        paramVal = sh.getParameter("q");
        assertTrue("getParameter() returned '" + paramVal +
                   "' instead of 10.", paramVal.equals("10"));
    }

    /**
     * Body of the test 6.
     *
     * Test for Contact header field: toString().
     */
    void Test6() {
	// DEBUG:        System.out.println("");
	// DEBUG:        System.out.println("*** Test6 started ***");

        String headerParam = "<sips:bob@192.0.2.4>;expires=60";
        SipHeader sh = createSipHeader(headerName, headerParam);

        if (sh == null) {
            return;
        }

        assertTrue("Invalid string representation: '" + sh.toString() + "'",
                   sh.toString().equals(headerName + ": " + headerParam));
    }

    /**
     * Run the tests
     */
    public void runTests() {
        declare("setName()/getName()");
        Test1();

        declare("getValue()/getHeaderValue()");
        Test2();

        // declare("setValue()");
        // Test3();

        declare("getParameterNames()/getParameter()");
        Test4();

        declare("setParameter()/removeParameter()");
        Test5();

        // declare("toString()");
        // Test6();
    }

}
