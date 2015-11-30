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

package com.sun.midp.io.j2me.sip;

import com.sun.midp.security.*;
import com.sun.midp.security.SecurityInitializer;
import com.sun.cldc.io.ConnectionBaseInterface;
import com.sun.midp.midlet.*;
import gov.nist.core.ParseException;
import gov.nist.microedition.sip.StackConnector;
import gov.nist.siplite.address.SipURI;
import gov.nist.siplite.parser.URLParser;
import gov.nist.siplite.SIPConstants;
import java.io.*;
import javax.microedition.io.*;
import javax.microedition.sip.SipException;

/**
 * This class implements the base necessary functionality
 * for a SIP connection. Classes sip.Protocol and sips.Protocol
 * are subclasses of ProtocolBase class
 * This class is a thin wrapper around the NIST
 * JSR180 implementation of the sip URI
 * protocol handler.
 * This class handles the security token
 * intialization and invokes the NISt handler.
 */

public abstract class ProtocolBase 
    implements ConnectionBaseInterface {

    /**
     * Inner class to request security token from SecurityInitializer.
     * SecurityInitializer should be able to check this inner class name.
     */
    static private class SecurityTrusted
        implements ImplicitlyTrustedClass {};

    /** Security token for SIP/SIPS protocol class */
    private static SecurityToken classSecurityToken =
        SecurityInitializer.requestToken(new SecurityTrusted());

    /**
     * Checks if transport protocol is supported.
     *
     * @param protocol protocol name
     *
     * @return true when protocol is supported
     */
    public boolean isProtocolSupported(String protocol) {
        if (protocol == null) {
          return false;
        } else {
            return protocol.equalsIgnoreCase(SIPConstants.TRANSPORT_UDP) ||
                protocol.equalsIgnoreCase(SIPConstants.TRANSPORT_TCP);
        }
    }

    /**
     * Sets up the state of the connection, but
     * does not actually connect to the server until there's something
     * to do.
     * <p>
     * @param name the URL for the connection, without the
     *  without the protocol part.
     * @param mode the access mode, ignored
     * @param timeouts flag to indicate that the caller wants
     * timeout exceptions, ignored
     * @return reference to this connection
     * @exception IllegalArgumentException if a parameter is invalid
     * @exception ConnectionNotFoundException if the connection cannot be
     * found
     * @exception IOException if some other kind of I/O error occurs
     */
    public abstract Connection openPrim(String name, int mode, boolean timeouts)
    throws IOException, IllegalArgumentException,
            ConnectionNotFoundException;

    /**
     * The Connector convenience methods to gain access to a specific
     * input or output stream directly are not
     * supported by the SIP API. The implementations MUST
     * throw IOException if these methods are called with SIP URIs.
     */

    /**
     * Throw IOException on call openInputStream() method.
     *
     * @return  nothing
     * @exception  IOException always throws
     *
     */
    public InputStream openInputStream() throws IOException {
        throw new IOException("SIP connection doesn't support input stream");
    }


    /**
     * Throw IOException on call openInputStream() method.
     *
     * @return  nothing
     * @exception  IOException always throws
     *
     */
    public OutputStream openOutputStream() throws IOException {
        throw new IOException("SIP connection doesn't support output stream");
    }


    /**
     * Throw IOException on call openInputStream() method.
     *
     * @return  nothing
     * @exception  IOException always throws
     *
     */
    public DataInputStream openDataInputStream() throws IOException {
        throw new IOException("SIP connection doesn't support input stream");
    }


    /**
     * Throw IOException on call openInputStream() method.
     *
     * @return  nothing
     * @exception  IOException always throws
     *
     */
    public DataOutputStream openDataOutputStream() throws IOException {
        throw new IOException("SIP connection doesn't support output stream");
    }

    /**
     * Checks for the required permission.
     * @param name  resource to insert into the permission question
     * @param protocol  protocol name
     * @param permission  value of the permission constant
     * @exception IOInterruptedException if another thread interrupts the
     *   calling thread while this method is waiting to preempt the
     *   display.
     */
    protected void checkForPermission(String name, String protocol,
            int permission) throws InterruptedIOException {
        Scheduler scheduler;
        MIDletSuite midletSuite;

        scheduler = Scheduler.getScheduler();
        midletSuite = scheduler.getMIDletSuite();

        // there is no suite running when installing from the command line
        if (midletSuite == null) {
            return;
        }
        name = protocol + ":" + name;
        try {
            midletSuite.checkForPermission(permission, name);
        } catch (InterruptedException ie) {
            throw new InterruptedIOException(
                    "Interrupted while trying to ask the user permission");
        }
    }

    /**
     * Open a client or server socket connection.
     * <p>
     * The name string for this protocol should be:
     * {scheme}:[{target}][{params}]
     * where:
     * scheme is SIP(S) scheme supported by the system sip
     * target is user network address in form
     * of {user_name}@{target_host}[:{port}]
     * or {telephone_number}, see examples below.
     * params stands for additional SIP URI parameters like ;transport=udp
     * <B>Opening new client connection (SipClientConnection):</B>
     * If the target host is included a, SipClientConnection will be returned.
     * Examples:
     * sip:bob@biloxi.com?sips:alice@10.128.0.8:5060
     * sip:alice@company.com:5060;transport=tcp
     * sip:+358-555-1234567;postd=pp22@foo.com;user=phone
     * <B>Opening new server connection (SipConnectionNotifier):</B>
     * Three forms of SIP URIs are allowed where the target host is omitted:
     * sips:nnnn, returns SipConnectionNotifier listening to incoming SIP
     * requests on port number nnnn.
     * sip:, returns SipConnectionNotifier listening to incoming SIP requests
     * on a port number allocated by the system.
     * sip:[nnnn];type=application/x-type,
     * returns SipConnectionNotifier listening to incoming SIP requests that
     * match to the MIME type application/x-type
     * (specified in the URI parameter type). Port number is optional.
     * Examples of opening a SIP connection:
     * SipClientConnection scn = (SipClientConnection)
     *    Connector.open(sips:bob@biloxi.com);
     * SipClientConnection scn= (SipClientConnection)
     *    Connector.open(sip:alice@company.com:5060;transport=tcp);
     *  SipConnectionNotifier scn = (SipConnectionNotifier)
     *    Connector.open(sip:5060);
     *  SipConnectionNotifier scn = (SipConnectionNotifier)
     *    Connector.open(sip:5080);
     *  SipConnectionNotifier scn = (SipConnectionNotifier)
     *    Connector.open(sip:);
     *
     * @param name       the [{target}][{params}] for the connection
     * @param scheme     the scheme name (sip or sips)
     *
     * @return client or server SIPS connection
     *
     * @exception  IOException  if an I/O error occurs.
     * @exception  ConnectionNotFoundException  if the host cannot be
     * connected to
     * @exception  IllegalArgumentException  if the name is malformed
     */
    protected Connection openConn(String name, String scheme)
    throws IOException, IllegalArgumentException, SipException {
        
        Connection retval = null;
        URLParser parser = new URLParser(scheme + ":" + name);
        SipURI inputURI = null;
        
        try {
            inputURI = (SipURI)parser.parseWholeString();
        } catch (ParseException exc) {
            throw new IllegalArgumentException(exc.getMessage());
        }
        
        String transport = inputURI.getTransportParam();
        if (transport == null) { // get default transport name
            transport =  SIPConstants.TRANSPORT_UDP;
        } else if (!transport.equalsIgnoreCase(SIPConstants.TRANSPORT_UDP) &&
            !transport.equalsIgnoreCase(SIPConstants.TRANSPORT_TCP)) {
            throw new SipException(SipException.TRANSPORT_NOT_SUPPORTED);
        }
        
        
        if (transport == null) { // get default transport name
            transport =  SIPConstants.TRANSPORT_UDP;
        }
        int portNum = inputURI.getPort();
        
        boolean isSecure = inputURI.isSecure();
        
        StackConnector stackConnector =
            StackConnector.getInstance(classSecurityToken);
        
        if (inputURI.isServer()) { // server URI
            String mimeType = inputURI.getTypeParam();
            if (inputURI.isShared()) { // sip:*;type="application/..."
                if (mimeType == null) { // no type parameter
                    throw new IllegalArgumentException("No type parameter "
                        + "in shared URI");
                }
                retval = (Connection)stackConnector.
                    createSharedSipConnectionNotifier(
                        isSecure,
                        transport,
                        mimeType);
            } else { // dedicated sip:5060;...
                retval = (Connection)stackConnector.
                    createSipConnectionNotifier(
                        portNum,
                        isSecure,
                        transport,
                        mimeType);
            }
        } else { // client URI

            if (portNum == -1) { // set default port
                inputURI.setPort(SIPConstants.DEFAULT_NONTLS_PORT);
            }

            retval = (Connection) stackConnector.
                createSipClientConnection(inputURI);
        }
        return retval;
    }

}

