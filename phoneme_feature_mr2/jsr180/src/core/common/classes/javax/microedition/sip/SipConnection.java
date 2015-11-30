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

import javax.microedition.io.Connection;
import java.io.IOException;
import java.io.InterruptedIOException;

/**
 * SipConnection is the base interface for Sip connections.
 * @see JSR180 spec, v 1.0.1, p 11-20
 *
 */
public interface SipConnection extends Connection {
    
    /**
     * Sends the SIP message. Send must also close the OutputStream
     * if it was opened
     * @throws IOException - if the message could not be sent or because
     * of network failure
     * @throws InterruptedIOException - if a timeout occurs while
     *  either trying
     * to send the message or if this Connection object is closed
     * during this send operation
     * @throws SipException - INVALID_STATE if the message cannot be sent
     * in this state. <br> INVALID_MESSAGE there was an error
     *  in message format
     */
    public void send()
    throws IOException, SipException;
    
    /**
     * Sets header value in SIP message.
     * @see JSR180 spec, v 1.0.1, p 17
     *
     * @param value - the header value
     * @throws SipException - INVALID_STATE if header can not be set in
     * this state. <br> INVALID_OPERATION if the system does not allow to set
     * this header.
     * @throws IllegalArgumentException - MAY be thrown if the header or
     * value is invalid
     */
    public void setHeader(String name, String value)
    throws SipException, IllegalArgumentException;
    
    /**
     * Adds a header to the SIP message.
     * @see JSR180 spec, v 1.0.1, p 17
     *
     * @param name - name of the header, either in full or compact form.
     * RFC 3261 p.32
     * @param value - the header value
     * @throws SipException - INVALID_STATE if header can not be added in
     * this state. <br> INVALID_OPERATION if the system does not allow to add
     * this header.
     * @throws IllegalArgumentException - MAY be thrown if the header or
     * value is invalid
     */
    public void addHeader(String name, String value)
    throws SipException, IllegalArgumentException;
    
    /**
     * Reomves header from the SIP message.
     * @see JSR180 spec, v 1.0.1, p 18
     *
     * @param name - name of the header to be removed, either int
     * full or compact form RFC 3261 p.32.
     * @throws SipException - INVALID_STATE if header can not be removed in
     * this state. <br> INVALID_OPERATION if the system does not allow to remove
     * this header.
     */
    public void removeHeader(String name)
    throws SipException;
    
    /**
     * Gets the header field value(s) of specified header type
     * @param name - name of the header, either in full or compact form.
     * RFC 3261 p.32
     * @return array of header field values (topmost first), or null if the
     * current message does not have such a header or the header is for other
     * reason not available (e.g. message not initialized).
     */
    public String[] getHeaders(String name);
    
    /**
     * Gets the header field value of specified header type.
     * @param name - name of the header type, either in full or compact form.
     * RFC 3261 p.32
     * @return topmost header field value, or null if the
     * current message does not have such a header or the header is for other
     * reason not available (e.g. message not initialized).
     */
    public String getHeader(String name);
    
    /**
     * Gets the SIP method. Applicable when a message has been
     * initialized or received.
     * @return SIP method name REGISTER, INVITE, NOTIFY, etc. Returns null if
     * the method is not available.
     */
    public java.lang.String getMethod();
    
    /**
     * Gets Request-URI.
     * @see JSR180 spec, v 1.0.1, p 19
     *
     * @return Request-URI of the message. Returns null if the Request-URI
     * is not available.
     */
    public java.lang.String getRequestURI();
    
    /**
     * Gets SIP response status code. Available when SipClientConnection is in
     * Proceeding or Completed state or when SipServerConnection is in
     * Initialized state.
     * @return status code 1xx, 2xx, 3xx, 4xx, ... Returns 0 if the status code
     * is not available.
     */
    public int getStatusCode();
    
    /**
     * Gets SIP response reason phrase. Available when SipClientConnection is in
     * Proceeding or Completed state or when SipServerConnection is in
     * Initialized state.
     * @return reason phrase. Returns null if the reason phrase is
     *  not available.
     */
    public java.lang.String getReasonPhrase();
    
    /**
     * Returns the current SIP dialog. This is available when the SipConnection
     * belongs to a created SipDialog and the system has received (or sent)
     * provisional (101-199) or final response (200).
     * @return SipDialog object if this connection belongs to a dialog,
     * otherwise returns null.
     */
    public javax.microedition.sip.SipDialog getDialog();
    
    /**
     * Returns InputStream to read SIP message body content.
     * @return InputStream to read body content
     * @throws java.io.IOException - if the InputStream can not be opened,
     * because of an I/O error occurred.
     * @throws SipException - INVALID_STATE the InputStream can not be opened
     * in this state (e.g. no message received).
     */
    public java.io.InputStream openContentInputStream()
    throws IOException, SipException;
    
    /**
     * Returns OutputStream to fill the SIP message body content.
     * @see JSR180 spec, v 1.0.1, p 20
     *
     * @return OutputStream to write body content
     * @throws IOException if the OutputStream can not be opened,
     * because of an I/O error occurred.
     * @throws SipException INVALID_STATE the OutputStream can not be opened
     * in this state (e.g. no message initialized).
     * UNKNOWN_LENGTH Content-Length header not set.
     * UNKNOWN_TYPE Content-Type header not set.
     */
    public java.io.OutputStream openContentOutputStream()
    throws IOException, SipException;
}
