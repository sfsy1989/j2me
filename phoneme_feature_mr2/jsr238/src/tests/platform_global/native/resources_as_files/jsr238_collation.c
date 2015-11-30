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

/**
 * @file
 * Implementation of low-level porting API for JSR 238 (MI18N) under Linux.
 */

#include <java_types.h>
#include <jsr238_collation.h>
#include <midp_logging.h>


/**
 * Gets the number of supported locales for string collation.
 *
 * @return the number of supported locales or 0 if something is wrong
 */
jint jsr238_get_collation_locales_count() {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_collation_locales_count()\n");

    return 0;
}

/**
 * Gets a locale name for string collation for the index.
 *
 * @param loc    buffer for the locale
 * @param index  index of the locale
 * @return 0 on success, -1 otherwise
 */
JSR238_STATUSCODE jsr238_get_collation_locale(jbyte* loc, jint index) {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_collation_locale()\n");

    /* Suppress unused parameter warnings */
    (void)loc;
    (void)index;

    return -1;
}

/**
 * Compare two strings.
 *
 * @param s1            first string to compare
 * @param len1          length of the the first string
 * @param s2            second string to compare
 * @param len2          length of the second string
 * @param locale_index  index of the locale
 * @param level         level of collation
 * @return negative if s1 belongs before s2, 0 if the strings are equal, positive if s1 belongs after s2
 */
jint jsr238_compare_strings(jchar* s1, jsize len1, jchar* s2, jsize len2, jint locale_index, jint level) {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_compare_strings()\n");

    /* Suppress unused parameter warnings */
    (void)s1;
    (void)len1;
    (void)s2;
    (void)len2;
    (void)locale_index;
    (void)level;

    return 0;
}
