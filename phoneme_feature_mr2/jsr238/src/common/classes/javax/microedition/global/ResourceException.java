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
package javax.microedition.global;

// JAVADOC COMMENT ELIDED 
public final class ResourceException extends RuntimeException {

    // JAVADOC COMMENT ELIDED 
    public final static int DATA_ERROR = 5;

    // JAVADOC COMMENT ELIDED 
    public final static int METAFILE_NOT_FOUND = 7;

    // JAVADOC COMMENT ELIDED 
    public final static int NO_RESOURCES_FOR_BASE_NAME = 3;

    // JAVADOC COMMENT ELIDED 
    public final static int NO_SYSTEM_DEFAULT_LOCALE = 4;

    // JAVADOC COMMENT ELIDED 
    public final static int RESOURCE_NOT_FOUND = 1;

    // JAVADOC COMMENT ELIDED 
    public final static int UNKNOWN_ERROR = 0;

    // JAVADOC COMMENT ELIDED 
    public final static int UNKNOWN_RESOURCE_TYPE = 6;

    // JAVADOC COMMENT ELIDED 
    public final static int WRONG_RESOURCE_TYPE = 2;

    /**
     *  Error code.
     */
    private int err;

    // JAVADOC COMMENT ELIDED 
    public ResourceException(int err, java.lang.String message) {
        super(message);
        if ((err < 0) || (err > 7)) {
            throw new IllegalArgumentException();
        }
        this.err = err;
    }

    // JAVADOC COMMENT ELIDED 
    public int getErrorCode() {
        return err;
    }
}

