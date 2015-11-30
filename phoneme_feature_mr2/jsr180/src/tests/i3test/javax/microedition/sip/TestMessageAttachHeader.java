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

import java.util.Enumeration;
import gov.nist.siplite.message.*;
import com.sun.midp.i3test.TestCase;
import gov.nist.siplite.header.*;

/**
 * Test for Message.attachHeader() method.
 */
public class TestMessageAttachHeader extends TestCase {

    /**
     * Body of the test 1: test for attachHeader().
     */
    void Test1() {
        Request request = null;
        String[] strContacts = {
            "Contact: <sip:UserB@192.168.200.201>;expires=3600\n",
            "Contact: <mailto:UserB@biloxi.com>;expires=4294967295\n"
        };

        try {
            request = new Request();

            request.addHeader(strContacts[0]);
            request.addHeader(strContacts[1]);
        } catch (Exception ex) {
            fail("Exception was thrown: " + ex);
        }

        ContactList contacts = request.getContactHeaders();
        assertNotNull("contacts is null!", contacts);
        assertEquals("Wrong number of contacts",
            strContacts.length, contacts.size());

        Enumeration en = contacts.getElements();
        int i = 0;

        while (en.hasMoreElements()) {
            String hdr = ((Header)en.nextElement()).toString();

            // Remove trailing '\n' and/or '\r'.
            int idx1 = hdr.indexOf('\n');
            int idx2 = hdr.indexOf('\r');

            if (idx1 >= 0  && idx1 < idx2) {
                hdr = hdr.substring(0, idx1);
            } else {
                if (idx2 >= 0) {
                    hdr = hdr.substring(0, idx2);
                }
            }

            assertEquals("Invalid Contact header value!",
                strContacts[i], hdr + "\n");
            i++;
        }

        /** TODO: add more test cases. */
    }

    /**
     * Run the tests.
     */
    public void runTests() {
        declare("attachHeader() test");
        Test1();
    }
}
