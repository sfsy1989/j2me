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
import gov.nist.siplite.header.*;

/**
 * Test for support of multiline header values.
 */
public class TestMultilineHeaders extends SipHeaderBaseTest {

    /**
     * Body of the test 1: support of multiline header values.
     */
    void Test1() {
        SipHeader sh;

        // Header's name / header's value to set / expected return value
        String[][] strHeaders = {
            {
                "Subject",
                    "I know you're there,\r\n" +
                    "\tpick up the phone and talk to me!",
                        "I know you're there, pick up the phone and talk to me!"
            },

            {
                "Subject",
                    "I know you're there,\r\n" +
                    " pick up the phone and talk to me!",
                        "I know you're there, pick up the phone and talk to me!"
            },

            {
                "Subject",
                    "I know you're there,\r\n" +
                    "     pick up the phone and talk to me!",
                        "I know you're there,     pick up " +
                            "the phone and talk to me!"
            },

            {
                "Subject",
                    "I know you're there,\n\r" +
                    "\tpick up the phone and talk to me!",
                        "I know you're there, pick up the phone and talk to me!"
            },

            {
                "Subject",
                    "I know you're there,\n" +
                    "\tpick up the phone and talk to me!",
                        "I know you're there, pick up the phone and talk to me!"
            },

            {
                "Subject",
                    "I know you're there,\r" +
                    "\tpick up the phone and talk to me!",
                        "I know you're there, pick up the phone and talk to me!"
            },

            {
                "Organization",
                    "Sun \n" + " Microsystems",
                        "Sun  Microsystems"
            },

            {
                "AnExtendedHeader",
                    "I know you're there,\r\n" +
                    " pick up the phone and talk to me!",
                        "I know you're there, pick up the phone and talk to me!"
            },
        };

        for (int i = 0; i < strHeaders.length; i++) {
            sh = createSipHeader(strHeaders[i][0], strHeaders[i][1]);

            if (sh == null) {
                return;
            }

            String val = sh.getValue();
            assertEquals("Invalid header's value!", strHeaders[i][2], val);
        } // end for
    }

    /**
     * Run the tests.
     */
    public void runTests() {
        declare("Test for support of multiline header values");
        Test1();
    }
}
