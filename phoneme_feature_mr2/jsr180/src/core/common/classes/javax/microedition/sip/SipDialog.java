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
 * SipDialog represents one SIP Dialog.
 * @see JSR180 spec, v 1.0.1, p 42-47
 *
 */
public interface SipDialog {
    /**
     * no response or error response (3xx-6xx) received (or sent).
     * Also if the dialog is terminated with BYE or un-SUBSCRIBE.
     */
    public static final byte TERMINATED = 0;
    /**
     * provisional 101-199 response received (or sent)
     * Method getNewClientConnection() can not be called in this state.
     */
    public static final byte EARLY = 1;
    /**
     * final 2xx response received (or sent)
     * All methods available.
     */
    public static final byte CONFIRMED = 2;
    
    /**
     * Returns new SipClientConnection in this dialog.
     * @see JSR180 spec, v 1.0.1, p 45
     *
     * @param method - given method
     * @return SipClientConnection with preset headers.
     * @throws IllegalArgumentException - if the method is invalid
     * @throws SipException - INVALID_STATE if the new connection can not be
     * established in the current state of dialog.
     */
    public javax.microedition.sip.SipClientConnection
            getNewClientConnection(java.lang.String method)
            throws IllegalArgumentException, SipException;
    
    /**
     * Does the given SipConnection belong to this dialog.
     * @param sc - SipConnection to be checked, can be either
     * SipClientConnection or SipServerConnection
     * @return true if the SipConnection belongs to the this dialog.
     * Returns false
     * if the connection is not part of this dialog or the dialog is terminated.
     */
    public boolean isSameDialog(javax.microedition.sip.SipConnection sc);
    
    /**
     * Returns the state of the SIP Dialog.
     * @return dialog state byte number.
     */
    public byte getState();
    
    /**
     * Returns the ID of the SIP Dialog.
     * @return Dialog ID (Call-ID + remote tag + local tag).
     * Returns null if the dialog is terminated.
     */
    public java.lang.String getDialogID();
}
