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

import javax.microedition.io.Connection;
import java.io.IOException;
import javax.microedition.io.ConnectionNotFoundException;
import java.io.InterruptedIOException;
import com.sun.midp.security.Permissions;

/**
 * This class implements the necessary functionality
 * for a SIP connection.
 */

public class Protocol extends ProtocolBase {

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
    public Connection openPrim(String name, int mode, boolean timeouts)
        throws IOException, IllegalArgumentException,
        ConnectionNotFoundException {
	
        /* Check the suite is permitted to use a SIP connection. */
        checkForPermission(name, "sip", Permissions.SIP);

        /*
         * Construct the SIP handler with the initialized
         * security token and invoke it's openPrim checks.
         */
        return openConn(name, "sip");
    }
        
}

