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

import javax.microedition.sip.SipClientConnection;
import gov.nist.microedition.sip.SipClientConnectionImpl;
import javax.microedition.io.Connector;
import java.io.OutputStream;
import java.io.IOException;
import com.sun.midp.i3test.TestCase;

/**
 * Tests for SipConnectionNotifier class.
 */
public class TestSipConnectionNotifier extends TestCase {
    
    /**
     * Body of the test 1.
     *
     * Try to establish connection with wrong URI format.
     */
    void Test1() {
        SipConnectionNotifier scn = null;
        try {
            // try to create SipConnection object with SCTP transport
            scn = (SipConnectionNotifier) Connector.open(
                    "sip:alice@atlanta.com;transport=sctp");
            fail("IllegalArgumentException wasn't caused");
        } catch (SipException e) {
            if (e.getErrorCode() == SipException.TRANSPORT_NOT_SUPPORTED) {
                assertTrue("OK", true);
            } else {
                fail("SipException was caused with wrong parameter");
            }
        } catch (Throwable e) {
            fail("" +e +" was caused");
        }

        try {
            // try to create SipConnection object with wrong URI
            // user part is empty when '@' is present
            scn = (SipConnectionNotifier) Connector.open("sip:@www.sun.com");
            fail("IllegalArgumentException wasn't caused");
        } catch (java.lang.IllegalArgumentException e) {
            assertTrue("IllegalArgumentException was caused", true);
            } catch (Throwable e) {
                fail("" +e +" was caused");
        }

        try {
            // try to create SipConnection object with wrong URI
            // port format is wrong
            scn = (SipConnectionNotifier) 
                Connector.open("sip:user@www.sun.com:abcd");
            fail("IllegalArgumentException wasn't caused");
        } catch (java.lang.IllegalArgumentException e) {
            assertTrue("IllegalArgumentException was caused", true);
            } catch (Throwable e) {
                fail("" +e +" was caused");
        }

        try {
            // try to create SipConnection object with wrong URI
            // parameter name is not unique
            scn = (SipConnectionNotifier) 
                Connector.open("sip:user@www.sun.com:1234;par1=val1;par1=val1");
            fail("IllegalArgumentException wasn't caused");
        } catch (java.lang.IllegalArgumentException e) {
            assertTrue("IllegalArgumentException was caused", true);
            } catch (Throwable e) {
                fail("" +e +" was caused");
        }
    }    
    
    /**
     * Create a SipConnectionNotifier and get local port and address
     */
    void Test2() {
        SipConnectionNotifier scn = null;
        try {
            // Open SIP server connection and listen to port 8888
            scn = (SipConnectionNotifier) Connector.open("sip:8888");
        } catch (Exception ex) {
            assertNull("Exception during scn open", scn);
            ex.printStackTrace();
        }
        assertNotNull("scn is null", scn);

        try {
            assertEquals("Local port is incorrect", scn.getLocalPort(), 8888);
        } catch (IOException ioe) {
            fail("IOException in getting local port or address");
        }

        try {
            scn.close();
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }

    /**
     * Create a SipConnectionNotifier with "transport=tcp"
     */
    void Test3() {
        SipConnectionNotifier scn = null;
        try {
            // Open SIP server connection and listen to port 9999
            scn = (SipConnectionNotifier) Connector.open(
                    "sip:9999;transport=tcp");
            assertNotNull("scn is null", scn);
            scn.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Create a SipConnectionNotifier with explicite "transport=udp"
     */
    void Test4() {
        SipConnectionNotifier scn = null;
        try {
            // Open SIP server connection and listen to port 4444
            scn = (SipConnectionNotifier) Connector.open(
                    "sip:8889");
            assertNotNull("scn is null", scn);
            scn.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Create a SipConnectionNotifier with explicite "sip:"
     */
    void Test5() {
        SipConnectionNotifier scn = null;
        try {
            // Open SIP server connection and listen to port 4444
            scn = (SipConnectionNotifier) Connector.open(
                    "sip:");
            assertNotNull("scn is null", scn);
            scn.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Create a SipConnectionNotifier with explicite "sip:;transport=tcp"
     */
    void Test6() {
        SipConnectionNotifier scn = null;
        try {
            // Open SIP server connection and listen to port 4444
            scn = (SipConnectionNotifier) Connector.open(
                    "sip:;transport=tcp");
            assertNotNull("scn is null", scn);
            scn.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Tests execute
     *
     */
    public void runTests() {
        declare("Wrong URI format");
        Test1();
        declare("SCN Methods");
        Test2();
        declare("TCP Transport");
        Test3();
        declare("UDP Transport");
        Test4();
        declare("sip:");
        Test5();
        declare("sip:;transport=tcp");
        Test6();
    }

}

