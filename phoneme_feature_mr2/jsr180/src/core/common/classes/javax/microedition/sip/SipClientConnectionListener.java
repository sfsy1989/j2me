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

package javax.microedition.sip;

/**
 * Listener interface for incoming SIP responses.
 * @see JSR180 spec, v 1.0.1, p 40
 *
 */
public interface SipClientConnectionListener {
    
    /**
     * This method gives the SipClientConnection instance, which has received
     * a new SIP response. The application implementing this listener interface
     * has to call SipClientConnection.receive() to initialize the
     * SipClientConnection object with the new response.
     * @param scc - SipClientConnection carrying the response
     */
    public void
            notifyResponse(javax.microedition.sip.SipClientConnection scc);
}
