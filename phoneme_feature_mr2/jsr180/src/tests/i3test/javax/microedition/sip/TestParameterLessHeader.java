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
 * This class is used to test TestParameterLessHeader class.
 */
public class TestParameterLessHeader extends SipHeaderBaseTest {

    /**
     * Array of the headers that can not have any parameters.
     * See Header.java.
     */
    private static final String[] parameterLessHeaders = {
        "Authentication-Info",
        "Allow",
        "In-Reply-To",
        "Priority",
        "Mime-Version",
        "Server",
        "Timestamp",
        "User-Agent",
        "Min-Expires",
        "Subject",
        // This header has the corresponding class and parser.
        // It should not be tested as "parameterless", pl. refer
        // to the comments in Header.java.
        // "Content-Length",
        "Content-Language",
        "Warning",
        "Content-Encoding",
        "Organization",
        "Unsupported",
        "Require",
        "Supported",
        "Proxy-Require"
    };

    /**
     * Body of the test 1.
     *
     * Test for "parameter-less" headers.
     */
    void Test1() {
        SipHeader sh;
        String    headerName;

        for (int i = 0; i < parameterLessHeaders.length; i++) {
            headerName = parameterLessHeaders[i];

            sh = createSipHeader(headerName, "INVITE;test=10");
            assertTrue("Cannot create '" + headerName + "' header!",
                       sh != null);

            // Ensure that the header was not parsed
            assertTrue("Header '" + headerName + "' was parsed!",
                       sh.getValue().equals(sh.getHeaderValue()));

            String[] paramNames = sh.getParameterNames();
            assertTrue("'" + headerName + "' header must not have " +
                       "any parameters!", paramNames == null);

            String param = sh.getParameter("test");
            assertTrue("'" + headerName + "' header has not parameter 'test'!",
                       param == null);

            try {
                sh.setParameter("test", "1");
                fail("IAE was not thrown!");
            } catch (IllegalArgumentException iae) {
                // System.out.println(headerName + " OK!");
            } catch (Exception e) {
                fail("Exception was caught: " + e);
            }
        } // end for()
    }

    /**
     * Run the tests
     */
    public void runTests() {
        declare("ParameterLessHeader class test");
        Test1();
    }

}
