/*
 * Portions Copyright  2000-2007 Sun Microsystems, Inc. All Rights
 * Reserved.  Use is subject to license terms.
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
/*
 */
package gov.nist.core;
/**
 *  Handles Internal error failures and prints a stack trace (for debugging).
 *
 *@version  JAIN-SIP-1.1
 *
 *
 *<a href="{@docRoot}/uncopyright.html">This code is in the public domain.</a>
 *
 */
public class InternalErrorHandler {
    /**
     * Handles an unexpected exception.
     * @param ex exception to be processed
     */
    public static void handleException(Exception ex) {
        try {
            throw ex;
        } catch (Exception e) {
            System.err.println("Unexpected exception : " + e);
            System.err.println("Error message is " + ex.getMessage());
            System.err.println("*************Stack Trace ************");
            e.printStackTrace();
            System.exit(0);
        }
    }
    
    /**
     * Handles an unexpected condition (and print the error code).
     * @param emsg message text for the exception
     */
    public static void handleException(String emsg) {
        try {
            throw new Exception(emsg);
        } catch (Exception ex) {
            ex.printStackTrace();
            System.exit(0);
        }
    }
}
