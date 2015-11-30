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

import java.util.Vector;

import gov.nist.core.ParseException;
import gov.nist.microedition.sip.StackConnector;
import gov.nist.siplite.address.Address;
import gov.nist.siplite.address.SipURI;
import gov.nist.siplite.address.TelURL;
import gov.nist.siplite.address.URI;
import gov.nist.siplite.parser.Lexer;

/**
 * SipAddress provides a generic SIP address parser.
 * @see JSR180 spec, v 1.0.1, p 52-57
 *
 */
public class SipAddress {
    /**
     * the nist-siplite corresponding address
     */
    private Address address;
    /**
     * Construct a new SipAddress from string.
     * @see JSR180 spec, v 1.0.1, p 60
     *
     */
    public SipAddress(java.lang.String address)
    throws IllegalArgumentException {
        try {
            this.address =
                    StackConnector.addressFactory.createAddress(address);
        } catch (ParseException pe) {
            throw new IllegalArgumentException(pe.getMessage());
        }

        // NullPointerException may be thrown in case if address is null.
        // We don't catch it because TCK expects this exception.
    }

    /**
     * Construct a new SipAddress from display name and URI.
     * @param displayName - user display name
     * @param URI - SIP URI
     * @throws IllegalArgumentException - if there was an error
     * in parsing the arguments.
     */
    public SipAddress(java.lang.String displayName, java.lang.String URI)
    throws IllegalArgumentException {
        try {
            URI uri = StackConnector.addressFactory.createURI(URI);
            this.address =
                StackConnector.addressFactory.createAddress(displayName, uri);
            if (false == address.isSIPAddress()) {
                throw new IllegalArgumentException("Wrong requested scheme");
            }
        } catch (ParseException pe) {
            throw new IllegalArgumentException(pe.getMessage());
        } catch (java.lang.NullPointerException pe) {
            throw new IllegalArgumentException(pe.getMessage());
        }
    }

    /**
     * Returns the display name of SIP address.
     * @return display name or null if not available
     */
    public java.lang.String getDisplayName() {
        return address.getDisplayName();
    }

    /**
     * Sets the display name. Empty string "" removes the display name.
     * @param name - display name
     * @throws IllegalArgumentException - if the display name is invalid
     */
    public void setDisplayName(java.lang.String name)
    throws IllegalArgumentException {
        address.setDisplayName(name);
    }

    /**
     * Returns the scheme of SIP address.
     * @return the scheme of this SIP address e.g. sip or sips
     */
    public java.lang.String getScheme() {
        return address.getScheme();
    }

    /**
     * Sets the scheme of SIP address. Valid scheme format is defined
     * in RFC 3261 [1] p.224
     * @param scheme the scheme of SIP address
     * @throws IllegalArgumentException if the scheme is invalid
     */
    public void setScheme(java.lang.String scheme)
            throws IllegalArgumentException {
        address.getURI().setScheme(scheme);
    }

    /**
     * Returns the user part of SIP address.
     * @return user part of SIP address. Returns null if the
     * user part is missing.
     */
    public java.lang.String getUser() {
        return address.getUser();
    }

    /**
     * Sets the user part of SIP address.
     * @param user - the user part
     * @throws IllegalArgumentException - if the user part is invalid
     */
    public void setUser(java.lang.String user)
            throws IllegalArgumentException {
        address.setUser(user);
    }

    /**
     * Returns the URI part of the address (without parameters)
     * i.e. scheme:user@host:port.
     * @return the URI part of the address
     */
    public java.lang.String getURI() {
        return address.getPlainURI();
    }

    /**
     * Sets the URI part of the SIP address (without parameters)
     * i.e. scheme:user@host:port. Possible URI parameters are ignored.
     * @param URI - URI part
     * @throws IllegalArgumentException - if the URI is invalid
     */
    public void setURI(java.lang.String URI)
            throws IllegalArgumentException {
        address.setURI(URI);
    }

    /**
     * Returns the host part of the SIP address.
     * @return host part of this address.
     */
    public java.lang.String getHost() {
        return address.getHost();
    }

    /**
     * Sets the host part of the SIP address.
     * @param host - host part
     * @throws IllegalArgumentException - if the post part is invalid
     */
    public void setHost(java.lang.String host)
            throws IllegalArgumentException {
        address.setHost(host);
    }

    /**
     * Returns the port number of the SIP address. If port number is
     * not set, return 5060.
     * If the address is wildcard "*" return 0.
     * @return the port number
     */
    public int getPort() {
        return address.getPort();
    }

    /**
     * Sets the port number of the SIP address. Valid range is
     * 0-65535, where 0 means that the port number is removed
     * from the address URI.
     * @param port port number, valid range 0-65535, 0 means
     * that port number is removed from the address URI.
     * @throws IllegalArgumentException if the port number is invalid.
     */
    public void setPort(int port)
            throws IllegalArgumentException {
        if (port < 0 || port > 65535) {
            throw new IllegalArgumentException("Invalid port: " + port);
        }

        URI uri = address.getURI();
        if (uri.isSipURI()) {
            if (port == 0) {
                ((SipURI)uri).removePort();
            } else {
                ((SipURI)uri).setPort(port);
            }
        }
        // IMPL_NOTE : do something for the tel URL
    }

    /**
     * Returns the value associated with the named URI parameter.
     * @param name - the name of the parameter
     * @return the value of the named parameter,
     *  or empty string for parameters
     * without value and null if the parameter is not defined
     */
    public java.lang.String getParameter(java.lang.String name) {
        return address.getParameter(name);
    }

    /**
     * Sets the named URI parameter to the specified value. If the
     * value is null
     * the parameter is interpreted as a parameter without value.
     * Existing parameter will be overwritten, otherwise the parameter
     * is added.
     * @param name - the named URI parameter
     * @param value - the value
     * @throws IllegalArgumentException - if the parameter is
     *  invalid RFC 3261,
     * chapter 19.1.1 SIP and SIPS URI Components "URI parameters" p.149
     */
    public void setParameter(java.lang.String name, java.lang.String value)
            throws IllegalArgumentException {
        address.setParameter(name, value);
    }

    /**
     * Removes the named URI parameter.
     * @param name - name of the parameter to be removed
     */
    public void removeParameter(java.lang.String name) {
        URI uri = address.getURI();
        if (uri.isSipURI())
            ((SipURI)uri).removeParameter(name);
        // IMPL_NOTE : do something for the tel URL
    }

    /**
     * Returns a String array of all parameter names.
     * @return String array of parameter names. Returns null if
     * the address does not have any parameters.
     */
    public java.lang.String[] getParameterNames() {
        return address.getParameterNames();
    }

    /**
     * Returns a fully qualified SIP address,  with display name,  URI and URI
     * parameters. If display name is not specified only a SIP URI is returned.
     * If the port is not explicitly set (to 5060 or other value) it
     *  will be omitted
     * from the address URI in returned String.
     * @return a fully qualified SIP name address,  SIP or SIPS URI
     */
    public java.lang.String toString() {
        return address.toString();
    }
}
