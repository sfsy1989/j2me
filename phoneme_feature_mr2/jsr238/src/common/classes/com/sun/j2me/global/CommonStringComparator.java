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
package com.sun.j2me.global;

/**
 * The <code>CommonStringComparator</code> interface defines methods
 * for string comparison that {@link javax.microedition.global.StringComparator}
 * realization classes must implement.
 */
public interface CommonStringComparator {

    /**
     * Compares the two strings using the rules specific to the associated 
     * locale of this instance. Use the <code>equals</code> method to more 
     * conveniently test for equality.
     * 
     * @param s1 first string to compare
     * @param s2 second string to compare
     * @return negative if s1 belongs before s2, zero if the strings are equal, 
     *      positive if s1 belongs after s2
     * @throws NullPointerException if either s1 or s2 is <code>null</code>
     * @see #equals(String, String)
     */
    public int compare(String s1, String s2);

    /**
     * Tests if the two strings are equal according to the rules specific to 
     * the associated locale of this instance.
     *
     * @param s1 first string to compare
     * @param s2 second string to compare
     * @return <code>true</code> if the strings are equal, <code>false</code> 
     *      if not
     * @throws NullPointerException if either s1 or s2 is <code>null</code>
     */
    public boolean equals(String s1, String s2);
}
