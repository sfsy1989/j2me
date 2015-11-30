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
 * Simple pattern matching tests 
 * to emulate native push filter handling.
 */
public class TestPushUtils extends TestCase {
    void test1() {
	String buf = "abc\nFrom: <abc.com> def\ndef";
	String filter = "*";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "foo";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "*.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "???.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	buf = "abc\nFrom:  abc.com def\ndef";
	filter = "*";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "foo";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "*.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "???.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	buf = "abc\nFrom  :  <abc.com> def\ndef";
	filter = "*";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "foo";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "*.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "???.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));
	

	buf = "abc\nF  :  <abc.com> def\ndef";
	filter = "*";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "foo";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "*.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "???.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));


	buf = "abc\nf:  <abc.com> def\ndef";
	filter = "*";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "foo";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "*.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "???.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	buf = "INVITE sip:sippy.tester@localhost:5090 SIP/2.0\n"
	    + "Call-ID: 467269204a-2130459815@129.145.167.130\n"
	    + "CSeq: 1 INVITE\n"
	    + "To: <sip:sippy.tester@localhost:5090>\n"
	    + "Via: SIP/2.0/UDP\n"
	    + "129.145.167.130:5090;branch=z9hG4bK3131323230353731383931393232373833313238333735343136313739333937\n"
	    + "Max-Forwards: 70\n"
	    + "From: <sip:sippy.tester@localhost>;tag=1017508678\n"
	    + " Subject: Hello...\n"
	    + "Contact: <sip:user@129.145.167.130:5090>\n"
	    + "Accept-Contact: *;type=\"application/x-chess\"\n"
	    + "Content-Type: text/plain\n"
	    + "Content-Length: 15\n"
	    + "\n"
	    + "Hello SIP-World\n";
	filter = "*";
	assertTrue("Match(" + buf + "," + filter + ")",
		   checksender(buf, filter));

	filter = "foo";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "*.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	filter = "???.com";
	assertTrue("Match(" + buf + "," + filter + ")",
		   !checksender(buf, filter));

	byte []acceptcontact_type = new byte[512];

	getSipAcceptContactType(buf.getBytes(),acceptcontact_type, 512);

	String act = new String(acceptcontact_type, 0, 7);

	assertTrue("Media type " + act + "==x-chess",
		   act.equalsIgnoreCase("x-chess"));
	assertTrue("Media type " + act + "!=x-chessbad",
		   !act.equalsIgnoreCase("x-chessbad"));
	assertTrue("Media type " + act + "!=bad",
		   !act.equalsIgnoreCase("bad"));
    }

    /**
     * Run the tests
     */
    public void runTests() {
        declare("Pattern tests");
        test1();
    }

    boolean checksender(String buf, String filter)  {
	byte[] sender = new byte[512];

	getSipFromHeaderURI(buf.getBytes(), sender, 512);
	return checksipfilter(filter.getBytes(), sender);
    }

    native int getSipFromHeaderURI(byte[] buf, byte[] sender, int len);

    native int getSipAcceptContactType(byte[] buf, byte[] sender, int len);

    native boolean checksipfilter(byte[] filter, byte[] sender);
}

