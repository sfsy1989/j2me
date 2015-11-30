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
 * Credential.java
 *
 * Created on Feb 2, 2004
 *
 */
package gov.nist.siplite.stack.authentication;

/**
 * Authentication credentials.
 *
 * <a href="{@docRoot}/uncopyright.html">This code is in the public domain.</a>
 */
public class Credentials {
    /** User name. */
    private String userName = null;
    /** Password. */
    private String password = null;
    /** Realm. */
    private String realm = null;
    
    /**
     * Creates a new Credential composed of a username password and realm
     * for later user by the API when a 401 or 407 Message will be received
     * @param userName - the user name
     * @param password - the password
     * @param realm - the realm
     */
    public Credentials(
            String userName,
            String password,
            String realm) {
        this.userName = userName;
        this.password = password;
        this.realm = realm;
    }
    
    /**
     * Gets the user name for this credential
     * @return the user name for this credential
     */
    public String getUserName() {
        return userName;
    }
    
    /**
     * Sets the user name for this credential
     * @param userName - the user name for this credential
     */
    public void setUserName(String userName) {
        this.userName = userName;
    }
    
    /**
     * Gets the password for this credential
     * @return the password for this credential
     */
    public String getPassword() {
        return password;
    }
    
    /**
     * Sets the password for this credential
     * @param password - the password for this credential
     */
    public void setPassword(String password) {
        this.password = password;
    }
    
    /**
     * Gets the realm for this credential
     * @return the realm for this credential
     */
    public String getRealm() {
        return realm;
    }
    
    /**
     * Sets the realm for this credential
     * @param realm - the realm for this credential
     */
    public void setRealm(String realm) {
        this.realm = realm;
    }
}
