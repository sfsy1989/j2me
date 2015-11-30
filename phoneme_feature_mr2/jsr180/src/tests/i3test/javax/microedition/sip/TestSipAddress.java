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

/**
 * Tests for SipAddress class.
 *
 */
public class TestSipAddress extends TestCase {

    /** Error message returned when SipAddress cannot be created. */
    private final String errCreateMsg = "Cannot create SipAddress: ";

    /**
     * Body of the test 1.
     *
     * Test constructors of SipAddress class.
     */
    void Test1() {
        try {
            // try to create SipAddress object with null argument
            SipAddress testAddress = new SipAddress(null);
            fail("NullPointerException wasn't caused");
        } catch (java.lang.NullPointerException e) {
            // assertTrue("IllegalArgumentException was caused", true);
        } catch (Throwable e) {
            fail("" + e + " was caused");
        }

        try {
            // try to create SipAddress object with wrong scheme argument
            SipAddress testAddress = new SipAddress("http://www.sun.com");
            fail("IllegalArgumentException wasn't caused");
        } catch (java.lang.IllegalArgumentException e) {
            // assertTrue("IllegalArgumentException was caused", true);
        } catch (Throwable e) {
            fail("" + e + " was caused");
        }

        try {
            // try to create SipAddress object with empty user part
            SipAddress testAddress = new SipAddress("sip:@www.sun.com");
            fail("IllegalArgumentException wasn't caused");
        } catch (java.lang.IllegalArgumentException e) {
            // assertTrue("IllegalArgumentException was caused", true);
        } catch (Throwable e) {
            fail("" + e + " was caused");
        }

        try {
            // try to create SipAddress object with wrong port value
            SipAddress testAddress =
                new SipAddress("sip:user@www.sun.com:abcd");
            fail("IllegalArgumentException wasn't caused");
        } catch (java.lang.IllegalArgumentException e) {
            assertTrue("IllegalArgumentException was caused", true);
        } catch (Throwable e) {
            fail("" + e + " was caused");
        }

        try {
            // try to create SipAddress object and check all its parts
            SipAddress testAddress = new SipAddress("DisplName",
                "sips:user:PassW@host:1234;par1=val1;par2=val2");
            assertTrue("Scheme",
                testAddress.getScheme().equalsIgnoreCase("sips"));
            assertTrue("User",
                testAddress.getUser().equals("user:PassW"));
            assertTrue("Host",
                testAddress.getHost().equalsIgnoreCase("host"));
            assertTrue("Port",
                testAddress.getPort() == 1234);
            assertTrue("Parameter 1",
                testAddress.getParameter("par1").equalsIgnoreCase("val1"));
            assertTrue("Parameter 2",
                testAddress.getParameter("par2").equalsIgnoreCase("val2"));
            assertTrue("DisplayName",
                testAddress.getDisplayName().equalsIgnoreCase("DisplName"));
        } catch (Throwable e) {
            fail("" + e + " was caused");
        }
    }

    /**
     * Body of the test 2.
     *
     * Test checks URI parts. 
     * @see ABNF in RFC3261, p. 228
     */
    void Test2() {
        try {
            SipAddress testAddress = new SipAddress("sip:test.org:5060");
            int testPort = testAddress.getPort();
            assertEquals("Returned port number should have been 5060",
                testPort, 5060);
        } catch (Throwable e) {
            fail("\n" + e + " was caused due to server SIPURI with port=5060");
        }

        try {
            SipAddress testAddress = new SipAddress("sip:test.org");
            int testPort = testAddress.getPort();

            /*
             * Per JSR180 specs, if port number is not set, getPort()
             * should return 5060
             */
            assertEquals("Returned port number should have been 5060",
                testPort, 5060);
        } catch (Throwable e) {
            fail("\n" + e + " was caused due to non-existent port number");
        }

        try {
            /*
             * The valid Contact address "*" is accepted in SipAddress.
             * In this case all properties will be null and port number is 0.
             * Yet toString() method will return the value "*".
             */
            SipAddress testAddress = new SipAddress("*");
            int testPort = testAddress.getPort();
            assertEquals("Returned port number should have been 0",
                testPort, 0);

            String strAddr = testAddress.toString();
            assertTrue("toString() returned '" + strAddr +
                "' instead of '*'.", strAddr.equals("*"));
        } catch (Throwable e) {
            fail("\n" + e + " was caused due to wild-card (*) address");
        }
    }

    /**
     * Body of the test 3.
     *
     * Test SipAddress methods: setDisplayName()/getDisplayName().
     */
    void Test3() {
        SipAddress sa = null;

        try {
            sa = new SipAddress("sip:test.org:5090");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        // Testing set/getDisplayName()
        String name = sa.getDisplayName();
        assertTrue("getDisplayName() returned '" + name +
                   "' instead of null.", name == null);

        String testName = "Test Name";
        sa.setDisplayName(testName);
        name = sa.getDisplayName();
        assertTrue("getDisplayName() returned '" + name +
                   "' after setDisplayName().", name.equals(testName));

        sa.setDisplayName("");
        name = sa.getDisplayName();
        assertTrue("getDisplayName() returned '" + name +
                   "' after setDisplayName(\"\").", name == null);
    }

    /**
     * Body of the test 4.
     *
     * Test SipAddress methods: setScheme()/getScheme().
     */
    void Test4() {
        SipAddress sa = null;

        try {
            sa = new SipAddress("sip:test.org:5090");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        // Testing set/getScheme()
        String scheme = sa.getScheme();
        assertTrue("getScheme() returned '" + scheme +
                   "' instead of 'sip'.", scheme.equals("sip"));

        // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        try {
            // TODO: maybe create a test for unknown scheme?
            sa = new SipAddress("Test Name <tel:123456>");
        } catch (Exception e) {
            fail("Cannot create SipAddress: " + e);
        }

        // TODO: implement changing of object's type, not only a scheme's name!
        sa.setScheme("sips");
        scheme = sa.getScheme();
        assertTrue("getScheme() returned '" + scheme +
                   "' instead of 'sips'.", scheme.equals("sips"));
    }

    /**
     * Body of the test 5.
     *
     * Test SipAddress methods: setUser()/getUser().
     */
    void Test5() {
        SipAddress sa = null;

        try {
            sa = new SipAddress("sip:alice:secretword@atlanta.com;" +
                                "transport=tcp");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        String user = sa.getUser();
        assertTrue("getuser() returned '" + user +
                   "' instead of 'alice'.", user.equals("alice:secretword"));

        String testUser = "User_Bob";
        try {
            sa.setUser(testUser);
        } catch (Exception e) {
            fail("setUser() has thrown an exception:" + e);
        }

        user = sa.getUser();
        assertTrue("getUser() returned '" + user + "' instead of '" +
                   testUser + "'.", user.equals(testUser));
    }

    /**
     * Body of the test 6.
     *
     * Test SipAddress methods: setURI()/getURI().
     */
    void Test6() {
        SipAddress sa = null;

        try {
            sa = new SipAddress("tel:123456");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        // Testing set/getURI()
        String uri = sa.getURI();
        assertTrue("getURI() returned '" + uri +
                   "' instead of 'tel:123456'.", uri.equals("tel:123456"));

        // Check that the URI parameters are ignored
        String testUri = "sip:test.org:5060";
        try {
            sa.setURI(testUri + ";param1=test");
        } catch (Exception e) {
            fail("setURI() has thrown an exception: " + e);
        }

        uri = sa.getURI();
        assertTrue("getURI() returned '" + uri + "' instead of '" +
                   testUri + "'.", uri.equals(testUri));

        // Try to set an invalid URI.
        testUri = "invalid uri";
        try {
            sa.setURI(testUri);
            fail("setURI(): IAE was not thrown.");
        } catch (IllegalArgumentException iae) {
        } catch (Exception e) {
            fail("setURI() has thrown '" + e + "' instead of IAE.");
        }
    }

    /**
     * Body of the test 7.
     *
     * Test SipAddress methods: setHost()/getHost() & setPort()/getPort().
     */
    void Test7() {
        SipAddress sa = null;

        try {
            sa = new SipAddress("sip:alice@atlanta.com:5070;transport=tcp");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        String testHost = "atlanta.com";
        String host = sa.getHost();
        assertTrue("getHost() has returned '" + host +
                   "' instead of '" + testHost + "'", host.equals(testHost));

        int port = sa.getPort();
        assertTrue("getPort() has returned '" + port +
            "' instead of 5070.", port == 5070);

        String val;
        String hostNames[] = {
            "test.com",
            "10.0.0.1",
            "www.test.spb.com"
        };

        for (int i = 0; i < hostNames.length; i++) {
            try {
                sa.setHost(hostNames[i]);
            } catch (Exception e) {
                fail("setHost(" + hostNames[i] +
                    ") has thrown an exception: " + e);
            }

            val = sa.getHost();
            assertTrue("getHost() has returned '" + val + "' instead of '"
                + hostNames[i] + "'.", val.equals(hostNames[i]));
        } // end for

        String wrongNames[] = {
            null,
            ""
        };

        for (int i = 0; i < wrongNames.length; i++) {
            try {
                sa.setHost(wrongNames[i]);
                fail("setHost("+wrongNames+") didn't throw IAE");
            } catch (Exception e) {
            }
        } // end for

        // Testing setPort()/ getPort()...
        //
        // setPort() sets the port number of the SIP address.
        // Valid range is 0-65535, where 0 means that the port number
        // is removed from the address URI.
        //
        // getPort() returns the port number of the SIP address.
        // If port number is not set, return 5060.
        // If the address is wildcard "*" return 0.
        try {
            sa.setPort(5090);
        } catch (Exception e) {
            fail("setPort(5090) has thrown an exception: " + e);
        }

        port = sa.getPort();
        assertTrue("getPort() has returned '" + port +
            "' instead of 5090.", port == 5090);

        // Check that setPort(0) removes the port number from the URI.
        try {
            sa.setPort(0);
        } catch (Exception e) {
            fail("setPort(0) has thrown an exception: " + e);
        }

        port = sa.getPort();
        assertTrue("getPort() has returned '" + port +
            "' instead of 5060.", port == 5060);

        // Trying to set an invalid port number...
        try {
            sa.setPort(70000);
            fail("setPort(70000) didn't throw IAE.");
        } catch (IllegalArgumentException iae) {
        } catch (Exception e) {
            fail("setPort(70000) has thrown '" + e + "' instead of IAE.");
        }

        // Check that the default port number is 5060.
        try {
            sa = new SipAddress("sip:alice@atlanta.com;transport=tcp");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        port = sa.getPort();
        assertTrue("getPort() has returned '" + port +
            "' instead of 5060.", port == 5060);

        // Check that if SipAddress is '*', getPort() returns 0.
        try {
            sa = new SipAddress("*");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        port = sa.getPort();
        assertTrue("getPort() has returned '" + port +
            "' instead of 0.", port == 0);
    }

    /**
     * Body of the test 8.
     *
     * Test SipAddress methods: setParameter()/getParameter()/
     *                          removeParameter()/getParameterNames().
     */
    void Test8() {
        SipAddress sa = null;

        try {
            sa = new SipAddress("sip:test.org:5090");
        } catch (Exception e) {
            fail(errCreateMsg + e);
        }

        String val;
        String paramList[][] = {
            // parameter's name, value to set, value that
            // must be returned by getParameter()
            {"param1", "value1", "value1"},
            {"param1", "value_2", "value_2"},
            {"param2", "test%20value", "test%20value"},
            {"param3", null, ""}
        };
        int paramNum = 3;

        // Testing set/getParameter()...
        // setParameter() sets the named URI parameter to the specified value.
        // If the value is null the parameter is interpreted as a parameter
        // without value. Existing parameter will be overwritten, otherwise
        // the parameter is added.
        for (int i = 0; i < paramList.length; i++) {
            try {
                sa.setParameter(paramList[i][0], paramList[i][1]);
            } catch (Exception e) {
                fail("setParameter(" + paramList[i][0] + ", " +
                    paramList[i][1] + ") has thrown an exception: " + e);
            }

            val = sa.getParameter(paramList[i][0]);
            assertTrue("getParameter() has returned '" + val + "' instead of '"
                + paramList[i][2] + "'.", val.equals(paramList[i][2]));
        } // end for

        // getParameter() returns empty string for parameters without value
        // and null if the parameter is not defined.
        val = sa.getParameter("unexisting");
        assertTrue("getParameter() has returned '" + val +
            "' instead of 'null'.", val == null);

        // Testing getParameterNames()...
        // It returns a string array of parameter names or null if the address
        // does not have any parameters.
        String name;
        String[] paramNames = sa.getParameterNames();
        assertTrue("getParameterNames() has returned " + paramNames.length +
                   " parameters instead of " + paramNum + ".",
                   paramNames.length == paramNum);

        for (int i = 0; i < paramNames.length; i++) {
            name = "param" + (i + 1);

            boolean isValid = false;

            for (int j = 0; j < paramNames.length; j++) {
                if (paramNames[j].equals(name)) {
                    isValid = true;
                    break;
                }
            }

            // Check that getParameterNames() has returned the correct names.
            assertTrue("getParameterNames() has returned '" + name +
                "' parameter but it doesn't exist.", isValid);
        }

        // Testing removeParameter()...
        for (int i = 0; i < paramNames.length; i++) {
            name = "param" + (i + 1);
            sa.removeParameter(name);

            val = sa.getParameter(name);
            assertTrue("getParameter() has returned '" + val +
                "' instead of 'null'.", val == null);
        }

        paramNames = sa.getParameterNames();
        int len = (paramNames != null) ? paramNames.length : 0;

        assertTrue("getParameterNames() has returned " + len +
                   " parameter(s) instead of null.", paramNames == null);
    }

    /**
     * Body of the test 9.
     *
     * Test SipAddress methods: toString().
     */
    void Test9() {
        SipAddress sa = null;
        String testUriReordered =
            "sips:alice@atlanta.com?priority=urgent&subject=project%20x";
        String[] testUris = {
            "sips:alice@atlanta.com?subject=project%20x&priority=urgent",
            "Alice <sip:alice@atlanta.com>",
            "sip:alice@atlanta.com",
            "sip:alice:secretword@atlanta.com;transport=tcp",
            "The Name <sip:alice:secretword@atlanta.com:6000;transport=tcp>",
            "sip:+1-212-555-1212:1234@gateway.com;user=phone",
            "sips:1212@gateway.com",
            "sip:alice@192.0.2.4",
            "sip:atlanta.com;method=REGISTER?to=alice%40atlanta.com",
            "sip:alice;day=tuesday@atlanta.com"
        };

        // toString() returns a fully qualified SIP address,
        // with display name, URI and URI parameters.
        // If display name is not specified only a SIP URI is returned.
        // If the port is not explicitly set (to 5060 or other value)
        // it will be omitted from the address URI in returned String.
        for (int i = 0; i < testUris.length; i++) {
            try {
                sa = new SipAddress(testUris[i]);
            } catch (Exception e) {
                fail(errCreateMsg + e);
            }

            String strAddr = sa.toString();
            boolean isValid;

            if (i == 0) {
                // The first test URL has two parameters. Their order
                // is not defined in the spec, so they can be reordered.
                isValid = strAddr.equals(testUris[i]) ||
                          strAddr.equals(testUriReordered);
            } else {
                isValid = strAddr.equals(testUris[i]);
            }

            assertTrue("toString() has returned '" + strAddr +
                       "' instead of '" + testUris[i] + "'.", isValid);
        }

        // "*" address was tested in Test2().
    }

    /**
     * Run the tests
     */
    public void runTests() {
        declare("Constructor SipAddress test");
        Test1();

        declare("URI parsing test");
        Test2();

        declare("setDisplayName()/getDisplayName()");
        Test3();

        declare("setScheme()/getScheme()");
        Test4();

        declare("setUser()/getUser()");
        Test5();

        declare("setURI()/getURI()");
        Test6();

        declare("setHost()/getHost() & setPort()/getPort()");
        Test7();

        declare("setParameter()/getParameter()/removeParameter()/" +
                "getParameterNames()");
        Test8();

        declare("toString()");
        Test9();
    }
}

