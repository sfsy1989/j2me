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
import gov.nist.microedition.sip.SipConnectionNotifierImpl;
import gov.nist.microedition.sip.SipClientConnectionImpl;
import gov.nist.siplite.message.Request;
import gov.nist.siplite.header.*;
import gov.nist.siplite.address.*;
import gov.nist.core.*;
import com.sun.midp.i3test.TestCase;

/**
 * Tests for SetHeader method of SipClientConnection class.
 *
 */
public class TestSipClientSetHeader extends TestCase {
    
    /**
     * Body of the test 1.
     * Checks that on "send" method the "tag" parameter is added
     * to "From" header and the "contact" header contains the
     * correct port value.
     * 
     * @param method string representation of the requests method
     */
    void Test1(String method) {
        /** Client connection. */
        SipClientConnection sc = null;
		SipConnectionNotifier scn = null;
        try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) 
	        Connector.open("sip:sippy.tester@localhost:5060");
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
            ex.printStackTrace();
        }
        assertNotNull("sc is null", sc);
	
		// State of client connection must be "Created"
		assertEquals("State is not CREATED", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.CREATED);
	
	
		// Move to INITIALIZED state
		try {
	    	sc.initRequest(method, null);
            sc.setHeader("Content-Type", "text/plain");
        } catch (Throwable e) {
            fail("" +e +" was caused");
		}
	
		assertEquals("State is not INITIALIZED", 
	    	((SipClientConnectionImpl)sc).getState(),
	    	SipClientConnectionImpl.INITIALIZED);
	

		try {
	    	scn = (SipConnectionNotifier) Connector.open("sip:5090");
            	sc.setHeader("From", "sip:sippy.tester@localhost"); // no tag
            	sc.setHeader("Subject", "Hello...");
            	sc.setHeader("Contact", "sip:user@"+scn.getLocalAddress() +
                   ":" + scn.getLocalPort());
	    	sc.send();
        } catch (Throwable e) {
            fail("" +e +" was caused");
		}
	
		Request request = ((SipClientConnectionImpl)sc).getRequest();
	
		try {
	    	// On "send" method "tag" parameter must be added
	    	FromHeader fromHeader = request.getFromHeader();
	    	assertNotNull("No \"From\" header in request", fromHeader);
	    	assertNotNull("No \"Tag\" parameter in \"From\" header",
	        fromHeader.getTag());
		    // "Contact" header must be correct
	    	ContactList contacts = request.getContactHeaders();
	    	assertEquals("Wrong number of contacts", contacts.size(), 1);
	    	// port number must be 5090
	    	ContactHeader contact = (ContactHeader)contacts.elementAt(0);
	    	assertEquals("Wrong port number", 
	        	contact.getHostPort().getPort(), 5090);
        } catch (Throwable e) {
            fail("" +e +" was caused");
		}

		// close connection
		try {
	    	sc.close();
	    	scn.close();
        } catch (Throwable e) {
            fail("" +e +" was caused");
		}
    }
    
    
    /**
     * Body of the test 2.
     *
     * Check the transforming TEL URI to SIP URI.
     * @param method string representation of the requests method
     */
    void Test2(String method) {
        /** Client connection. */
        SipClientConnection sc = null;
        try {
            // Open SIP client connection to the local SIP server
            sc = (SipClientConnection) 
	        Connector.open("sip:sippy.tester@localhost:5060");
        } catch (Exception ex) {
            assertNull("Exception during sc open", sc);
            ex.printStackTrace();
        }
        assertNotNull("sc is null", sc);
	
		// State of client connection must be "Created"
		assertEquals("State is not CREATED", 
	    ((SipClientConnectionImpl)sc).getState(),
	    SipClientConnectionImpl.CREATED);
	
	
		// Move to INITIALIZED state
		try {
	    	sc.initRequest(method, null);
        } catch (Throwable e) {
            fail("" +e +" was caused");
		}
	
		assertEquals("State is not INITIALIZED", 
	    	((SipClientConnectionImpl)sc).getState(),
	    	SipClientConnectionImpl.INITIALIZED);
	
		// Set header with the "tel" scheme
		try {
            sc.setHeader("From", "tel:+1234567;postd=9876;isub=3746");
        	assertTrue("OK", true);
		} catch (Throwable e) {
            fail("" +e +" was caused");
			e.printStackTrace();
		}
		
		Request request = ((SipClientConnectionImpl)sc).getRequest();
	
		try {
	    	// On "send" method "tag" parameter must be added
	    	FromHeader fromHeader = request.getFromHeader();
	    	assertNotNull("No \"From\" header in request", fromHeader);
			Address telAddress = fromHeader.getAddress();
			TelURL telURL = (TelURL)(telAddress.getURI());
            assertTrue("Wrong scheme", telURL.getScheme().equals("tel"));
            assertTrue("Wrong isub", telURL.getIsdnSubAddress().equals("3746"));
			
        } catch (Throwable e) {
            fail("" +e +" was caused");
			e.printStackTrace();
		}
	
		// Set header with the "tel" scheme with correct isdn-subaddress
		// RFC 2806, 2.2
		// isdn-subaddress       = ";isub=" 1*phonedigit
		// phonedigit            = DIGIT / visual-separator
		// visual-separator      = "-" / "." / "(" / ")"
		try {
            sc.setHeader("From", "tel:+1234567;isub=01234567890-.()");
		    assertTrue("OK", true);
		} catch (Throwable e) {
            fail("" +e +" was caused");
			e.printStackTrace();
		}
		// Set header with the "tel" scheme with wrong isdn-subaddress
		try {
            sc.setHeader("From", "tel:+1234567;isub=3A46");
        	System.err.println("Test: 1");
			fail("IllegalArgumentException weren't be caused");
		} catch (IllegalArgumentException e) {
		    assertTrue("OK", true);
		} catch (Throwable e) {
            fail("" +e +" was caused");
			e.printStackTrace();
		}
	
		// Set header with the "tel" scheme with correct post-dial
		// RFC 2806, 2.2
		// post-dial             = ";postd=" 1*(phonedigit /
		//                         dtmf-digit / pause-character)
		// dtmf-digit            = "*" / "#" / "A" / "B" / "C" / "D"
		// pause-character       = one-second-pause / wait-for-dial-tone
		// one-second-pause      = "p"
		// wait-for-dial-tone    = "w"
		try {
            sc.setHeader("From", "tel:+1234567;postd=01234567890-.()pw*#ABCD");
		    assertTrue("OK", true);
		} catch (Throwable e) {
            fail("" +e +" was caused");
			e.printStackTrace();
		}
		
		// Set header with the "tel" scheme with wrong post-dial
		try {
            sc.setHeader("From", "tel:+1234567;postd=9e8w76;isub=3746");
        	System.err.println("Test: 2");
        	fail("IllegalArgumentException weren't be caused");
		} catch (IllegalArgumentException e) {
		    assertTrue("OK", true);
		} catch (Throwable e) {
            fail("" +e +" was caused");
			e.printStackTrace();
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
		declare("Test1");
    	Test1("INVITE");
		declare("Test2");
    	Test2("INVITE");
    }

}

