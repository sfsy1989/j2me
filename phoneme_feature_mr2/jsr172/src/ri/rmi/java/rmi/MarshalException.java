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

package java.rmi;

/**
 * A <code>MarshalException</code> is thrown if a
 * <code>java.io.IOException</code> occurs while marshalling the remote call
 * header, arguments or return value for a remote method call.  A
 * <code>MarshalException</code> is also thrown if the receiver does not
 * support the protocol version of the sender.
 *
 * <p>If a <code>MarshalException</code> occurs during a remote method call,
 * the call may or may not have reached the server.  If the call did reach the
 * server, parameters may have been deserialized.  A call may not be
 * retransmitted after a <code>MarshalException</code> and reliably preserve
 * "at most once" call semantics.
 * 
 * @version 1.10, 12/03/01
 * @since   JDK1.1
 */
public class MarshalException extends RemoteException {

    /* indicate compatibility with JDK 1.1.x version of class */
    private static final long serialVersionUID = 6223554758134037936L;

    /**
     * Constructs a <code>MarshalException</code> with the specified
     * detail message.
     *
     * @param s the detail message
     * @since JDK1.1
     */
    public MarshalException(String s) {
	super(s);
    }

    /**
     * Constructs a <code>MarshalException</code> with the specified
     * detail message and nested exception.
     *
     * @param s the detail message
     * @param ex the nested exception
     * @since JDK1.1
     */
    public MarshalException(String s, Exception ex) {
	super(s, ex);
    }
}

