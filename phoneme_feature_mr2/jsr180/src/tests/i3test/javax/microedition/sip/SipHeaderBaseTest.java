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

import gov.nist.core.Separators;
import javax.microedition.io.Connector;
import java.io.IOException;

/**
 * This is a base class for all Test&lt;HeaderName&gt;Header classes.
 */
public abstract class SipHeaderBaseTest extends TestCase {

    /**
     * Creates SipHeader with the specified name and parameter.
     *
     * @param  headerName  header's name.
     * @param  headerParam header's parameter.
     * @return SipHeader instance if successfully created, null otherwise.
     */
    protected SipHeader createSipHeader(String headerName, String headerParam) {
        SipHeader sh;

        try {
           sh = new SipHeader(headerName, headerParam);
        } catch (java.lang.IllegalArgumentException e) {
            sh = null;
            fail("IllegalArgumentException: " + e);
        } catch (Exception e) {
            sh = null;
            fail(e + " was caught");
        }

        return sh;
    }
    
    /**
     * Checks if an exception is thrown when attempting to create
     * a header with invalid value.
     *
     * @param  headerName   header's name.
     * @param  invalidParam value that is invalid for this type of header.
     * @return true if the test was passed, false otherwise.
     */
    protected boolean testConstructorNegative(String headerName,
                                              String invalidParam) {
        SipHeader sh;
        boolean result;

        try {
           sh = new SipHeader(headerName, invalidParam);
           result = false;
           fail("Constructor: IAE was not thrown!");
        } catch (java.lang.IllegalArgumentException e) {
            result = true;
        } catch (Exception e) {
            result = false;
            fail(e + " was thrown instead of IAE.");
        }

        return result;
    }

    /**
     * Test for toString() method of Header's subclasses.
     *
     * @param headerName name of the header.
     * @param headerParam value and parameters of the header.
     */
    protected void testToString(String headerName, String headerParam) {
        SipHeader sh = createSipHeader(headerName, headerParam);

        if (sh == null) {
            return;
        }
        
        String strHeader = sh.toString();
        
        // Skip CRLF
        strHeader = removeNewLine(strHeader);

        assertTrue("Invalid string representation: '" + strHeader + "'",
            equalsIgnoreSpaces(strHeader, headerName + ": " + headerParam));
    }

    /**
     * Test for setValue() method of Header's subclasses.
     *
     * @param headerName name of the header.
     * @param headerParam value and parameters of the header.
     */
    protected void testSetValue(String headerName, String headerParam) {
        SipHeader sh = createSipHeader(headerName, headerParam);

        if (sh == null) {
            return;
        }
        
        // Here the one common case is tested:
        // if setValue() argument does include header's parameters,
        // IllegalArgumentException must be generated.
        try {
            sh.setValue("test;param1=val1");
            fail("setValue(): IAE was not thrown!");
        } catch (IllegalArgumentException iae) {
            // ok: IAE was thrown
        } catch (Exception e) {
            fail("setValue(): " + e + " was thrown instead of IAE!");
        }
        
        assertTrue(true);
    }
        
    /**
     * Cuts ending CRLF's if any.
     *
     * @param str string to process.
     * @return the resulting string.
     */
    protected String removeNewLine(String str) {
        int len = Separators.NEWLINE.length();
        
        // Remove CRLF
        while (str.endsWith(Separators.NEWLINE)) {
            str = str.substring(0, str.length() - len);
        }
        
        return str;
    }
    
    /**
     * Remove spaces/tabs surrounding delimeters in the string.
     *
     * @param str string to process.
     * @return the resulting string.
     */
    protected String removeSpaces(String str) {
        String strResult = "";
        boolean delimeterOn = false;
        char ch;
        
        for (int i = 0; i < str.length(); i++) {
            ch = str.charAt(i);
            
            if (delimeterOn) {
                if ((ch == ' ') || (ch == '\t')) {
                    continue; // skip the space/tab
                }
                
                delimeterOn = false;
            }
                
            if (ch == ';' || ch == '/') {
                strResult = strResult.trim();
                delimeterOn = true;
            }
            
            strResult += ch;
        } // end for
        
        return strResult;
    }
    
    /**
     * Compares two strings ignoring the spaces and tabs around delimeters.
     * Comparision is case-insensitive.
     *
     * @param str1 first string to compare.
     * @param str2 second string to compare.
     * @return true if the string are equal, false otherwise.
     */
    protected boolean equalsIgnoreSpaces(String str1, String str2) {
        String s1 = removeSpaces(str1);
        String s2 = removeSpaces(str2);

        // DEBUG: System.out.println(">>> s1 = '" + s1 + "'");
        // DEBUG: System.out.println(">>> s2 = '" + s2 + "'");
        
        return s1.equalsIgnoreCase(s2);
    }
}

