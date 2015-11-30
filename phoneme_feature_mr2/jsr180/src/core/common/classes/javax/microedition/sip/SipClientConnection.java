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

import java.io.*;

/**
 * SipClientConnection represents SIP client transaction.
 * Application can create a new SipClientConnection with Connector 
 * or SipDialog object. 
 * @see JSR180 spec, v 1.0.1, p 21-33
 *
 */
public interface SipClientConnection extends SipConnection {
    
    /**
     * Initializes SipClientConnection to a specific SIP 
     * request method (REGISTER, INVITE, MESSAGE, ...).  
     * @see JSR180 spec, v 1.0.1, p 25
     *
     * @param method - Name of the method
     * @param scn - SipConnectionNotifier to which the request will be
     * associated. If SipConnectionNotifier is null the request will not be
     * associated to a user defined listening point.
     * @throws IllegalArgumentException - if the method is invalid
     * @throws SipException - INVALID_STATE if the request can not be set,
     * because of wrong state in SipClientConnection. Furthermore, ACK and
     * CANCEL methods can not be initialized in Created state.
     */
    public void
            initRequest(java.lang.String method,
            javax.microedition.sip.SipConnectionNotifier scn)
            throws IllegalArgumentException, SipException;
    
    /**
     * Sets Request-URI explicitly. Request-URI can be set only in
     * Initialized state.
     * @see JSR180 spec, v 1.0.1, p 26
     *
     * @param URI - Request-URI
     * @throws IllegalArgumentException - MAY be thrown if the URI is invalid
     * @throws SipException - INVALID_STATE if the Request-URI can not be set,
     * because of wrong state.
     * INVALID_OPERATION if the Request-URI is not allowed to be set.
     */
    public void setRequestURI(java.lang.String URI)
    throws IllegalArgumentException, SipException;
    
    /**
     * Convenience method to initialize SipClientConnection with SIP request
     * method ACK. ACK can be applied only to INVITE request.
     * @see JSR180 spec, v 1.0.1, p 27
     *
     */
    public void initAck()
    throws SipException;
    
    /**
     * Convenience method to initialize SipClientConnection with SIP request
     * method CANCEL.
     * @see JSR180 spec, v 1.0.1, p 27-28
     *
     */
    public javax.microedition.sip.SipClientConnection initCancel()
    throws SipException;
    
    /**
     * Receives SIP response message. The receive method will update the
     * SipClientConnection with the last new received response.
     * If no message is received the method will block until something is
     * received or specified amount of time has elapsed.
     * @see JSR180 spec, v 1.0.1, p 28
     *
     * @param timeout - the maximum time to wait in milliseconds.
     * 0 = do not wait, just poll
     * @return Returns true if response was received. Returns false if
     * the given timeout elapsed and no response was received.
     * @throws SipException - INVALID_STATE if the receive can not be
     * called because of wrong state.
     * @throws IOException - if the message could not be received or
     * because of network failure
     */
    public boolean receive(long timeout)
    throws SipException, IOException;
    
    /**
     * Sets the listener for incoming responses. If a listener is
     * already set it
     * will be overwritten. Setting listener to null will remove the current
     * listener.
     * @see JSR180 spec, v 1.0.1, p 28
     *
     * @param sccl - reference to the listener object. Value null will remove
     *  the existing listener.
     * @throws IOException - if the connection is closed.
     */
    public void
            setListener(javax.microedition.sip.SipClientConnectionListener
            sccl)
            throws IOException;
    
    /**
     * Enables the refresh on for the request to be sent. The method return a
     * refresh ID, which can be used to update or stop the refresh.
     * @see JSR180 spec, v 1.0.1, p 29
     *
     * @param srl - callback interface for refresh events, if this is null the
     * method returns 0 and refresh is not enabled.
     * @return refresh ID. If the request is not refreshable returns 0.
     * @throws SipException - INVALID_STATE if the refresh can not be enabled
     * in this state.
     */
    public int enableRefresh(javax.microedition.sip.SipRefreshListener srl)
    throws SipException;
    
    /**
     * Sets credentials for possible digest authentication.
     * @see JSR180 spec, v 1.0.1, p 29
     *
     * @param username - username (for this protection domain)
     * @param password - user password (for this protection domain)
     * @param realm - defines the protection domain
     * @throws SipException - INVALID_STATE if the credentials can not
     * be set in this state.
     * @throws NullPointerException - if the username, password or realm is null
     */
    public void setCredentials(java.lang.String username,
            java.lang.String password,
            java.lang.String realm)
            throws SipException;
}

