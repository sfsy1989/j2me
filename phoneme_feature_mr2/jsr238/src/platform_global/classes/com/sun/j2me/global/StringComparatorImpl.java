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

import javax.microedition.global.StringComparator;
import javax.microedition.global.ResourceManager;
import javax.microedition.global.ResourceException;
import javax.microedition.global.UnsupportedLocaleException;

/**
 * This class realizes string comparison methods of
 * {@link javax.microedition.global.StringComparator}. Specifically, these are:
 * <ul>
 *   <li> {@link #compare(String, String)}
 *   <li> {@link #equals(String, String)}
 * </ul>
 * <p>
 * This realization of <code>StringComparator</code> is intended to be based on
 * native string comparison functions. Regarding this,
 * <code>StringComparatorImpl</code> uses locale index that points to its locale
 * instead of working with explicit locale code.
 */
public class StringComparatorImpl implements CommonStringComparator {

    /**
     * Current <code>StringComparatorImpl</code> level of comparison.
     * Possible values are:
     * <ul>
     *  <li> javax.microedition.global.StringComparator#LEVEL1
     *  <li> javax.microedition.global.StringComparator#LEVEL2
     *  <li> javax.microedition.global.StringComparator#LEVEL3
     *  <li> javax.microedition.global.StringComparator#IDENTICAL
     * </ul>
     */
    private int level;

    /**
     * Current <code>StringComparatorImpl</code> locale index in the list of
     * supported locales.
     */
    private int locale_index;


    /**
     * Constructs an instance using the specified locale and collation level.
     * <p>
     * If the specified locale is <code>null</code> or the empty string, this 
     * class uses the generic collation algorithm.
     *
     * @param locale  the locale to use, or <code>null</code> to use the generic
     *      collation algorithm
     * @param level   the collation level to use
     * @throws UnsupportedLocaleException if the specified locale is not 
     *      supported by the StringComparator implementation
     * @throws IllegalArgumentException if the specified locale is 
     *      non-<code>null</code> but is not valid according to the MIDP 
     *      specification, or if <code>level</code> is not one of the constants 
     *      defined in this class
     * @see javax.microedition.global.StringComparator#LEVEL1
     * @see javax.microedition.global.StringComparator#LEVEL2
     * @see javax.microedition.global.StringComparator#LEVEL3
     * @see javax.microedition.global.StringComparator#IDENTICAL
     */
    public StringComparatorImpl(String locale, int level) {
        this.level = level;

		locale_index = CollationAbstractionLayerImpl.getCollationLocaleIndex(locale);
		if (locale_index == -1) {
			throw new UnsupportedLocaleException("Locale \""
												 + locale +
												 "\" unsupported.");
		}
    }
    
    /**
     * Compares the two strings using the rules specific to the associated 
     * locale of this instance. Use the <code>equals</code> method to more 
     * conveniently test for equality.
     * 
     * @param s1 first string to compare
     * @param s2 second string to compare
     * @return negative if <code>s1</code> belongs before <code>s2</code>,
     *      zero if the strings are equal, positive if <code>s1</code> belongs
     *      after <code>s2</code>
     * @throws NullPointerException if either <code>s1</code> or <code>s2</code>
     *      is <code>null</code>
     * @see #equals(String, String)
     */
    public int compare(String s1, String s2) {
        if (s1 == null || s2 == null) {
            throw new NullPointerException("Cannot compare null strings");
        }
        return compare0(locale_index, s1, s2, level);
    }

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
    public boolean equals(String s1, String s2) {
        return compare(s1, s2) == 0;
    }

    /**
     *  Compare two strings using locale- and level-specific rules.
     *
     * @param s1            first string to compare
     * @param s2            second string to compare
     * @param locale_index  the locale index in supported locales list
     * @param level         the collation level to use
     * @return negative if <code>s1</code> belongs before <code>s2</code>,
     *      zero if the strings are equal, positive if <code>s1</code> belongs
     *      after <code>s2</code>
     */
    private static native int compare0(int locale_index, String s1, String s2, 
            int level);
}
