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
package gov.nist.siplite.address;

import java.util.*;

import gov.nist.core.*;
import gov.nist.siplite.SIPConstants;
import gov.nist.siplite.parser.Lexer;


/**
 * The SipURI structure.
 *
 *
 * <a href="{@docRoot}/uncopyright.html">This code is in the public domain.</a>
 *
 */
public class SipURI extends URI {

    /**
     * Authority for the uri.
     */
    protected Authority authority = null;

    /**
     * uriParms list
     */
    protected NameValueList uriParms = null;

    /**
     * qheaders list
     */
    protected NameValueList qheaders = null;

    /**
     * telephoneSubscriber field
     */
    protected TelephoneNumber telephoneSubscriber = null;
    
    /** Flag if the instance is server URI. */
    private boolean isServer = false;
    
    /** Flag if the instance is server shared URI. */
    private boolean isShared = false;

    /** Default constructor. */
    public SipURI() {
        scheme = SIPConstants.SCHEME_SIP;
        uriParms = new NameValueList("uriparms");
        qheaders = new NameValueList("qheaders");
        qheaders.setSeparator("&");
    }

    /**
     * Sets URI as server.
     */
    public void setServer() {
        isServer = true;
    }

    /**
     * Sets URI as shared.
     */
    public void setShared() {
        isShared = true;
    }

    /**
     * Checks if URI is shared.
     * @return true when URI is server else false
     */
    public boolean isServer() {
        return isServer;
    }

    /**
     * Checks if URI is shared.
     * @return true when URI is shared else false
     */
    public boolean isShared() {
        return isShared;
    }

    /**
     * Constructor given the scheme.
     * The scheme must be either Sip or Sips.
     * @param scheme URI protocol scheme
     * @exception IllegalArgumentException if scheme is no sip or sips
     */
    public void setScheme(String scheme) throws IllegalArgumentException {
        if (compareToIgnoreCase(scheme, SIPConstants.SCHEME_SIP) != 0 &&
                compareToIgnoreCase(scheme, SIPConstants.SCHEME_SIPS) != 0)
            throw new IllegalArgumentException("bad scheme " + scheme);
        this.scheme = scheme.toLowerCase();
    }

    /**
     * Clears all URI Parameters.
     * @since v1.0
     */
    public void clearUriParms() {
        uriParms = new NameValueList("uriparms");
    }

    /**
     * Clears the password from the user part if it exists.
     */
    public void clearPassword() {
        if (this.authority != null) {
            UserInfo userInfo = authority.getUserInfo();
            if (userInfo != null) userInfo.clearPassword();
        }
    }

    /**
     * Gets the authority.
     * @return the authority information
     */
    public Authority getAuthority() {
        return this.authority;
    }

    /**
     * Clears all Qheaders.
     */
    public void clearQheaders() {
        qheaders = new NameValueList("qheaders");
    }

    /**
     * Compares two URIs and return true if they are equal.
     * @param that the object to compare to.
     * @return true if the object is equal to this object.
     */
    public boolean equals(Object that) {

        if (that == null) {
            return false;
        }

        if (!this.getClass().equals(that.getClass())) {
            return false;
        }

        SipURI other = (SipURI) that;

        // Compare the authority portion.
        if (!this.authority.equals(other.authority))
            return false;

        // compare the parameter lists.
        // Assuming default for TRANSPORT so cannot use the method
        // of NameValueList to check for equality
        NameValueList hisParms = other.uriParms;

        // transport comparation
        String currTransport = uriParms.getValueDefault
            (SIPConstants.GENERAL_TRANSPORT, SIPConstants.TRANSPORT_UDP);
        String hisTransport = hisParms.getValueDefault
            (SIPConstants.GENERAL_TRANSPORT, SIPConstants.TRANSPORT_UDP);
        if (!currTransport.equals(hisTransport))
            return false;

        // other parameters
        Enumeration enumNames = uriParms.getKeys();
        String currKey;
        while (enumNames.hasMoreElements()) {
            currKey = (String)enumNames.nextElement();
            if (currKey.equals(SIPConstants.GENERAL_TRANSPORT)) continue;
            // check that other URI contains this parameter
            if (!hisParms.hasNameValue(currKey))
                return false;
            // check that other URI contains the same value of this parameter
            if (!uriParms.getValue(currKey).equals(
                hisParms.getValue(currKey)))
                    return false;
        }

        // leave headers alone - they are just a screwy way of constructing
        // an entire sip message header as part of a URL.
        return true;
    }

    /**
     * Constructs a URL from the parsed structure.
     * @return String
     */
    public String encode() {
        StringBuffer retval =
                new StringBuffer(scheme).append(Separators.COLON);
        if (authority != null) {
            retval.append(authority.encode());
        }

        if (!uriParms.isEmpty()) {
            retval.append(Separators.SEMICOLON).append(uriParms.encode());
        }

        if (!qheaders.isEmpty()) {
            retval.append(Separators.QUESTION).append(qheaders.encode());
        }

        return retval.toString();
    }

    /**
     * Returns a string representation.
     *
     * @return the String representation of this URI.
     */
    public String toString() { return this.encode(); }

    /**
     * Gets user at host information.
     * @return user@host portion of the uri (null if none exists).
     */
    public String getUserAtHost() {
        if (authority == null) {
            return null;
        }

        String user = authority.getUserInfo().getUser();
        String host = authority.getHost().encode();

        return new StringBuffer(user).
            append(Separators.AT).append(host).toString();
    }

    /**
     * Returns the URI part of the address (without parameters)
     * i.e. scheme:user@host:port.
     * @return URI part of the address
     */
    public String getPlainURI() {
        StringBuffer retval =
                new StringBuffer(scheme).append(Separators.COLON);
        retval.append(getUserAtHostPort());
        return retval.toString();
    }

    /**
     * Gets user at host and port infromation.
     * instead of the current function body.
     * @return user@host portion of the uri (null if none exists).
     */
    public String getUserAtHostPort() {
        return authority.encode();
    }

    /**
     * Gets the parameter (do a name lookup) and return null if none exists.
     * @param parmname Name of the parameter to get.
     * @return Parameter of the given name (null if none exists).
     */
    public Object getParm(String parmname) {
        Object obj = uriParms.getValue(parmname);
        return obj;
    }

    /**
     * Gets the method parameter.
     * @return Method parameter.
     */
    public String getMethod() {
        return (String) getParm(SIPConstants.GENERAL_METHOD);
    }

    /**
     * Accessor for URI parameters
     * @return A name-value list containing the parameters.
     */
    public NameValueList getUriParms() {
        return uriParms;
    }

    /**
     * Removes the URI parameters.
     *
     */
    public void removeUriParms() {
        uriParms = new NameValueList();
    }

    /**
     * Accessor forSIPObjects
     * @return Get the query headers (that appear after the ? in
     * the URL)
     */
    public NameValueList getQheaders() {
        return qheaders;
    }

    /**
     * Gets the user parameter.
     * @return User parameter (user= phone or user=ip).
     */
    public String getUserType() {
        return (String) uriParms.getValue(SIPConstants.GENERAL_USER);
    }

    /**
     * Gets the password of the user.
     * @return User password when it embedded as part of the uri
     * ( a very bad idea).
     */
    public String getUserPassword() {
        if (authority == null)
            return null;
        return authority.getPassword();
    }

    /**
     * Sets the user password.
     * @param password - password to set.
     * @throws IllegalArgumentException if password contains invalid
     * characters
     */
    public void setUserPassword(String password)
            throws IllegalArgumentException  {
        if (authority == null) {
            authority = new Authority();
        }
        authority.setPassword(password);
    }

    /**
     * Returns the stucture corresponding to the telephone number
     * provided that the user is a telephone subscriber.
     * @return TelephoneNumber part of the url (only makes sense
     * when user = phone is specified)
     */
    public TelephoneNumber getTelephoneSubscriber() {
        if (telephoneSubscriber == null) {
            telephoneSubscriber = new TelephoneNumber();
        }
        return telephoneSubscriber;
    }

    /**
     * Gets the host and port of the server.
     * @return get the host:port part of the url parsed into a
     * structure.
     */
    public HostPort getHostPort() {
        if (authority == null)
            return null;
        else {
            return authority.getHostPort();
        }
    }

    /**
     * Gets the port from the authority field.
     *
     * @return the port from the authority field.
     */
    public int getPort() {
        HostPort hp = this.getHostPort();

        if (hp == null) {
            return -1;
        }

        return hp.getPort();
    }

    /**
     * Returns default port number according to current scheme.
     * 
     * @return Current scheme default port.
     */
    public int getDefaultPort() {
        return (scheme.equalsIgnoreCase(SIPConstants.SCHEME_SIP)) ?
            SIPConstants.DEFAULT_NONTLS_PORT : SIPConstants.DEFAULT_TLS_PORT;
    }

    /**
     * Gets the host protion of the URI.
     * @return the host portion of the url.
     */
    public String getHost() {
        Host h = authority.getHost();
        return (h == null) ? "" : h.encode();
    }

    /**
     * Returns true if the user is a telephone subscriber.
     * If the host is an Internet telephony
     * gateway, a telephone-subscriber field MAY be used instead
     * of a user field. The telephone-subscriber field uses the
     * notation of RFC 2806 [19]. Any characters of the un-escaped
     * "telephone-subscriber" that are not either in the set
     * "unreserved" or "user-unreserved" MUST be escaped. The set
     * of characters not reserved in the RFC 2806 description of
     * telephone-subscriber contains a number of characters in
     * various syntax elements that need to be escaped when used
     * in SIP URLs, for example quotation marks (%22), hash (%23),
     * colon (%3a), at-sign (%40) and the "unwise" characters,
     * i.e., punctuation of %5b and above.
     * <p>
     * The telephone number is a special case of a user name and
     * cannot be distinguished by a BNF. Thus, a URL parameter,
     * user, is added to distinguish telephone numbers from user
     * names.
     * <p>
     * The user parameter value "phone" indicates that the user
     * part contains a telephone number. Even without this
     * parameter, recipients of SIP URLs MAY interpret the pre-@
     * part as a telephone number if local restrictions on the
     * @return true if the user is a telephone subscriber.
     */
    public boolean isUserTelephoneSubscriber() {
        String usrtype = (String) uriParms.getValue(USER);
        if (usrtype == null) {
            return false;
        }
        return usrtype.equals("phone");
    }

    /**
     * Removes the ttl value from the parameter list if it exists.
     */
    public void removeTTL() {
        if (uriParms != null) uriParms.delete(TTL);
    }

    /**
     * Removes the maddr param if it exists.
     */
    public void removeMAddr() {
        if (uriParms != null) uriParms.delete(MADDR);
    }

    /**
     * Deletes the transport string.
     */
    public void removeTransport() {
        if (uriParms != null) uriParms.delete(TRANSPORT);
    }

    /**
     * Removes a header given its name (provided it exists).
     * @param name name of the header to remove.
     */
    public void removeHeader(String name) {
        if (qheaders != null) qheaders.delete(name);
    }

    /**
     * Removes all headers.
     */
    public void removeHeaders() {
        qheaders = new NameValueList("qheaders");
    }

    /**
     * Sets the user type.
     */
    public void removeUserType() {
        if (uriParms != null) uriParms.delete(USER);
    }

    /**
     * Removes the port setting.
     */
    public void removePort() {
        authority.removePort();
    }

    /**
     * Removes the Method.
     */
    public void removeMethod() {
        if (uriParms != null) uriParms.delete(METHOD);
    }

    /**
     * Sets the user of SipURI. The identifier of a particular resource at
     * the host being addressed. The user and the user password including the
     * "at" sign make up the user-info.
     *
     * @param uname the new String value of the user.
     * @throws IllegalArgumentException if the user name is invalid
     */
    public void setUser(String uname) throws IllegalArgumentException {
        if (authority == null) {
            authority = new Authority();
        }

        authority.setUser(uname);
    }

    /**
     * Removes the user.
     */
    public void removeUser() {
        authority.removeUserInfo();
    }

    /**
     * Sets the default parameters for this URI.
     * Do nothing if the parameter is already set to some value.
     * Otherwise set it to the given value.
     * @param name Name of the parameter to set.
     * @param value value of the parameter to set.
     */
    public void setDefaultParm(String name, Object value) {
        if (uriParms.getValue(name) == null) {
            NameValue nv = new NameValue(name, value);
            uriParms.add(nv);
        }
    }


    /**
     * Sets the authority member
     * @param newAuthority Authority to set.
     */
    public void setAuthority(Authority newAuthority) {
        authority = newAuthority;
    }

    /**
     * Sets the host for this URI.
     * @param h host to set.
     */
    public void setHost(Host h) {
        // authority = new Authority();
        if (authority == null) {
            authority = new Authority();
        }
        authority.setHost(h);
    }

    /**
     * Sets the qheaders member.
     * @param parms query headers to set.
     */
    public void setQheaders(NameValueList parms) {
        qheaders = parms;
    }

    /**
     * Sets the MADDR parameter.
     * @param mAddr Host Name to set
     */
    public void setMAddr(String mAddr) {
        NameValue nameValue = uriParms.getNameValue(MADDR);
        Host host = new Host();
        host.setAddress(mAddr);

        if (nameValue != null) {
            nameValue.setValue(host);
        } else {
            nameValue = new NameValue(MADDR, host);
            uriParms.add(nameValue);
        }
    }

    /**
     * Sets the value of the user parameter. The user URI parameter exists to
     * distinguish telephone numbers from user names that happen to look like
     * telephone numbers. This is equivalent to setParameter("user", user).
     *
     * @param usertype new value String value of the method parameter
     */
    public void setUserParam(String usertype) {
        uriParms.delete(USER);
        uriParms.add(USER, usertype);
    }

    /**
     * Sets the Method.
     * @param method method parameter
     */
    public void setMethod(String method) {
        uriParms.add(METHOD, method);
    }

    /**
     * Sets ISDN subaddress of SipURL.
     * @param isdnSubAddress ISDN subaddress
     */
    public void setIsdnSubAddress(String isdnSubAddress) {
        if (telephoneSubscriber == null)
            telephoneSubscriber = new TelephoneNumber();
        telephoneSubscriber.setIsdnSubaddress(isdnSubAddress);
    }

    /**
     * Sets the telephone subscriber field.
     * @param tel Telephone subscriber field to set.
     */
    public void setTelephoneSubscriber(TelephoneNumber tel) {
        telephoneSubscriber = tel;
    }



    /**
     * Sets the port to a given value.
     * @param p Port to set.
     */
    public void setPort(int p) {
        if (authority == null) authority = new Authority();
        authority.setPort(p);
    }

    /**
     * Boolean to check if a parameter of a given name exists.
     * @param name Name of the parameter to check on.
     * @return a boolean indicating whether the parameter exists.
     */
    public boolean hasParameter(String name) {
        return uriParms.getValue(name) != null;
    }


    /**
     * Sets the query header when provided as a name-value pair.
     * @param nameValue qeuery header provided as a name,value pair.
     */
    public void setQHeader(NameValue nameValue) {
        this.qheaders.set(nameValue);
    }

    /**
     * Sets the parameter as given.
     * @param nameValue - parameter to set.
     */
    public void setUriParameter(NameValue nameValue) {
        uriParms.set(nameValue);
    }

    /**
     * Returns true if the transport parameter is defined.
     * @return true if transport appears as a parameter and false otherwise.
     */
    public boolean hasTransport() {
        return hasParameter(TRANSPORT);
    }

    /**
     * Removes a parameter given its name
     * @param name -- name of the parameter to remove.
     */
    public void removeParameter(String name) {
        uriParms.delete(name);
    }

    /**
     * Sets the hostPort field of the embedded authority field.
     * @param hostPort is the hostPort to set.
     */
    public void setHostPort(HostPort hostPort) {
        if (this.authority == null) {
            this.authority = new Authority();
        }
        authority.setHostPort(hostPort);
    }

    /**
     * Copies the current instance.
     * @return copy of current instance
     */
    public Object clone() {
        SipURI retval = new SipURI();
        retval.scheme = new String(this.scheme);
        retval.authority = (Authority) this.authority.clone();
        retval.uriParms = (NameValueList) this.uriParms.clone();
        if (this.qheaders != null)
            retval.qheaders =
                    (NameValueList) this.qheaders.clone();

        if (this.telephoneSubscriber != null) {
            retval.telephoneSubscriber =
                    (TelephoneNumber)
                    this.telephoneSubscriber.clone();
        }
        return retval;
    }

    /**
     * Returns the value of the named header, or null if it is not set.
     * SIP/SIPS URIs may specify headers. As an example, the URI
     * sip:joe@jcp.org?priority=urgent has a header "priority" whose
     * value is "urgent".
     *
     * @param name name of header to retrieve
     * @return the value of specified header
     */
    public String getHeader(String name) {
        return this.qheaders.getValue(name) != null ?
            this.qheaders.getValue(name).toString() : null;

    }

    /**
     * Returns an Iterator over the names (Strings) of all headers present
     * in this SipURI.
     *
     * @return an Iterator over all the header names
     */
    public Vector getHeaderNames() {
        return this.qheaders.getNames();
    }

    /**
     * Returns the value of the <code>lr</code> parameter, or null if this
     * is not set. This is equivalent to getParameter("lr").
     *
     * @return the value of the <code>lr</code> parameter
     */
    public String getLrParam() {
        boolean haslr = this.hasParameter(LR);
        return haslr? "true":null;
    }

    /**
     * Returns the value of the <code>maddr</code> parameter, or null if this
     * is not set. This is equivalent to getParameter("maddr").
     *
     * @return the value of the <code>maddr</code> parameter
     */
    public String getMAddrParam() {
        NameValue maddr = uriParms.getNameValue(MADDR);
        if (maddr == null)
            return null;
        String host = (String) maddr.getValue();
        return host;
    }

    /**
     * Returns the value of the <code>method</code> parameter, or null if this
     * is not set. This is equivalent to getParameter("method").
     *
     * @return the value of the <code>method</code> parameter
     */
    public String getMethodParam() {
        return this.getParameter(METHOD);
    }

    /**
     * Returns the value of the named parameter, or null if it is not set. A
     *
     * zero-length String indicates flag parameter.
     *
     *
     *
     * @param name name of parameter to retrieve
     *
     * @return the value of specified parameter
     *
     */
    public String getParameter(String name) {
        Object val = uriParms.getValue(name.toLowerCase());
        if (val == null)
            return null;
        if (val instanceof GenericObject)
            return ((GenericObject)val).encode();
        else return val.toString();
    }

    /**
     * Returns an Iterator over the names (Strings) of all parameters present
     * in this ParametersHeader.
     * @return an Iterator over all the parameter names
     *
     */
    public Vector getParameterNames() {
        return uriParms.getNames();
    }

    /**
     * Returns the value of the "ttl" parameter, or -1 if this is not set.
     * This method is equivalent to getParameter("ttl").
     *
     * @return the value of the <code>ttl</code> parameter
     */
    public int getTTLParam() {
        Integer ttl = (Integer) uriParms.getValue(SIPConstants.GENERAL_TTL);
        if (ttl != null)
            return ttl.intValue();
        else
            return -1;
    }

    /**
     * Returns the value of the "transport" parameter, or null if this is not
     * set. This is equivalent to getParameter("transport").
     *
     * @return the transport paramter of the SipURI
     */
    public String getTransportParam() {
        if (uriParms != null) {
            return (String) uriParms.getValue(SIPConstants.GENERAL_TRANSPORT);
        } else return null;
    }

    /**
     * Returns the value of the "type" parameter, or null if this is not
     * set. This is equivalent to getParameter("type"). The result is
     * unquoted
     *
     * @return the transport paramter of the SipURI
     */
    public String getTypeParam() {
        if (uriParms != null) {
            NameValue nv = uriParms.getNameValue(SIPConstants.GENERAL_TYPE);
            if (nv == null) {
                return null;
            }
            return (String)nv.getUnquotedValue();
        } else return null;
    }

    /**
     * Returns the value of the <code>userParam</code>,
     * or null if this is not set.
     * <p>
     * This is equivalent to getParameter("user").
     *
     * @return the value of the <code>userParam</code> of the SipURI
     */
    public String getUser() {
        return authority.getUser();
    }

    /**
     * Returns true if this SipURI is secure i.e. if this SipURI represents a
     * sips URI. A sip URI returns false.
     *
     * @return <code>true</code> if this SipURI represents a sips URI, and
     * <code>false</code> if it represents a sip URI.
     */
    public boolean isSecure() {
        return equalsIgnoreCase(this.getScheme(), SIPConstants.SCHEME_SIPS);
    }

    /**
     * This method determines if this is a URI with a scheme of "sip"
     * or "sips".
     *
     * @return true if the scheme is "sip" or "sips", false otherwise.
     */
    public boolean isSipURI() {
        return true;
    }

    /**
     * This method determines if this is a URI with a scheme of
     * "tel"
     *
     * @return true if the scheme is "tel", false otherwise.
     */
    public boolean isTelURL() {
        return false;
    }

    /**
     * Sets the value of the specified header fields to be included in a
     * request constructed from the URI. If the header already had a value it
     * will be overwritten.
     *
     * @param name - a String specifying the header name
     * @param value - a String specifying the header value
     */
    public void setHeader(String name, String value) {
        if (qheaders.getValue(name) == null) {
            NameValue nv = new NameValue(name, value);
            qheaders.add(nv);
        } else {
            NameValue nv = qheaders.getNameValue(name);
            nv.setValue(value);
        }
    }

    /**
     * Returns the host part of this SipURI.
     * @param host the host part of the URI
     * @throws IllegalArgumentException if the host part is formated wrong way
     */
    public void setHost(String host) throws IllegalArgumentException {
        Lexer lexer = new Lexer("sip_urlLexer", host);
        HostNameParser hnp = new HostNameParser(lexer);
        try {
            Host h = new Host(hnp.hostName());
            if (GenericObject.compareToIgnoreCase(h.getHostname(), host) == 0) {
                this.setHost(h);
                return;
            }
        } catch (ParseException e) {
            throw new IllegalArgumentException(e.getMessage());
        }
        throw new IllegalArgumentException("Illegal host name");
    }

    /**
     * Sets the value of the <code>lr</code> parameter of this SipURI. The lr
     * parameter, when present, indicates that the element responsible for
     * this resource implements the routing mechanisms specified in RFC 3261.
     * This parameter will be used in the URIs proxies place in the
     * Record-Route header field values, and may appear in the URIs in a
     * pre-existing route set.
     */
    public void setLrParam() {
        if (uriParms.getValue(SIPConstants.GENERAL_LR) != null)
            return;
        NameValue nv = new NameValue(SIPConstants.GENERAL_LR, null);
        uriParms.add(nv);
    }

    /**
     * Sets the value of the <code>maddr</code> parameter of this SipURI. The
     * maddr parameter indicates the server address to be contacted for this
     * user, overriding any address derived from the host field. This is
     * equivalent to setParameter("maddr", maddr).
     *
     * @param maddr new value of the <code>maddr</code> parameter
     */
    public void setMAddrParam(String maddr)
    throws ParseException {
        if (maddr == null)
            throw new NullPointerException("bad maddr");
        setParameter(SIPConstants.GENERAL_MADDR, maddr);
    }

    /**
     * Sets the value of the <code>method</code> parameter. This specifies
     * which SIP method to use in requests directed at this URI. This is
     * equivalent to setParameter("method", method).
     *
     * @param method - new value String value of the method parameter
     */
    public void setMethodParam(String method)
    throws ParseException {
        setParameter(SIPConstants.GENERAL_METHOD, method);
    }

    /**
     * Sets the value of the specified parameter. If the parameter already had
     * a value it will be overwritten. A zero-length String indicates flag
     * parameter.
     *
     * @param name - a String specifying the parameter name
     * @param value - a String specifying the parameter value
     * @throws ParseException which signals that an error has been reached
     * unexpectedly while parsing the parameter name or value.
     */
    public void setParameter(String name, String value)
            throws ParseException {
        if (name.equalsIgnoreCase(SIPConstants.GENERAL_TTL)) {
            try {
                int ttl = Integer.parseInt(value);
            } catch (NumberFormatException ex) {
                throw new ParseException("bad parameter " + value, 0);
            }
        }
        name = name.toLowerCase();
        // If the value is null the parameter is interpreted
        // as a parameter without value.
        NameValue nv = new NameValue(name,
                                     (value == null) ? "" : value);
        uriParms.delete(name);
        uriParms.add(nv);
    }

    /**
     * Sets the scheme of this URI to sip or sips depending on whether the
     * argument is true or false. The default value is false.
     *
     * @param secure - the boolean value indicating if the SipURI is secure.
     */
    public void setSecure(boolean secure) {
        if (secure)
            scheme = SIPConstants.SCHEME_SIPS;
        else
            scheme = SIPConstants.SCHEME_SIP;
    }

    /**
     * Sets the value of the <code>ttl</code> parameter. The ttl parameter
     * specifies the time-to-live value when packets are sent using UDP
     * multicast. This is equivalent to setParameter("ttl", ttl).
     *
     * @param ttl - new value of the <code>ttl</code> parameter
     */
    public void setTTLParam(int ttl) throws IllegalArgumentException {
        if (ttl <= 0)
            throw new IllegalArgumentException("Bad ttl value");
        if (uriParms != null) {
            uriParms.delete(SIPConstants.GENERAL_TTL);
            NameValue nv = new NameValue(SIPConstants.GENERAL_TTL,
                new Integer(ttl));
            uriParms.add(nv);
        }
    }

    /**
     * Sets the value of the "transport" parameter. This parameter specifies
     * which transport protocol to use for sending requests and responses to
     * this entity. The following values are defined: "udp", "tcp", "sctp",
     * "tls", but other values may be used also. This method is equivalent to
     * setParameter("transport", transport). Transport parameter constants
     * are defined in the {@link gov.nist.siplite.ListeningPoint}.
     *
     * @param transport - new value for the "transport" parameter
     * @see gov.nist.siplite.ListeningPoint
     */
    public void setTransportParam(String transport) throws ParseException {
        if (transport == null)
            throw new NullPointerException("null arg");
        if (equalsIgnoreCase(transport, SIPConstants.TRANSPORT_UDP) ||
                equalsIgnoreCase(transport, SIPConstants.TRANSPORT_TCP)) {
            NameValue nv = new NameValue(SIPConstants.GENERAL_TRANSPORT,
                transport.toLowerCase());
            uriParms.delete(SIPConstants.GENERAL_TRANSPORT);
            uriParms.add(nv);
        } else
            throw new ParseException("bad transport " + transport, 0);
    }

    /**
     * Returns the user part of this SipURI, or null if it is not set.
     *
     * @return the user part of this SipURI
     */
    public String getUserParam() {
        return getParameter("user");
    }

    /**
     * Returns whether the the <code>lr</code> parameter is set. This is
     * equivalent to hasParameter("lr"). This interface has no getLrParam as
     * RFC3261 does not specify any values for the "lr" paramater.
     *
     * @return true if the "lr" parameter is set, false otherwise.
     */
    public boolean hasLrParam() {
        return uriParms.getNameValue(SIPConstants.GENERAL_LR) != null;
    }
}
