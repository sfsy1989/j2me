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
#include <jsr238_format.h>
#include <jsr238_resources.h>
#include <midp_logging.h>

static int empty_locale_index = 0;

/**
 * Gets the number of supported locales for data formatting.
 *
 * @return the number of supported locales or 0 if something is wrong.
 */
jint jsr238_get_format_locales_count() {
    int rloc_count;

    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_format_locales_count()\n");

    rloc_count = jsr238_get_resource_locales_count();

    empty_locale_index = 0;
    while (empty_locale_index < rloc_count && jsr238_get_resource_locale((char*)0, empty_locale_index) != 0) {
        empty_locale_index++;
    }

    if (empty_locale_index < rloc_count) {
        rloc_count--;
    }
    return rloc_count;
}

/**
 * Gets the number of supported locales for data formatting.
 *
 * @return the number of supported locales or 0 if something is wrong.
 */
JSR238_STATUSCODE jsr238_get_format_locale(jbyte* loc, jint index) {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_format_locale()\n");

    return jsr238_get_resource_locale(loc, (index < empty_locale_index ? index : index + 1));
}

/**
 * Gets size of date and time formatting symbols.
 *
 * @param locale_index  index of the locale.
 * @return size of formatting symbols (in bytes), -1 on any error.
 */
JSR238_STATUSCODE jsr238_get_date_format_symbols_length(jsize* length /* OUT */, jint locale_index) {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_date_format_symbols_length()\n");
    return jsr238_get_resource_length(length, DATETIME_FORMAT_SYMBOL_RESOURCE_ID,
                                      (locale_index < empty_locale_index ? locale_index : locale_index + 1));
}

/**
 * Gets date and time formatting symbols as an array of bytes.
 *
 * @param symbols       buffer to store the formatting symbols.
 * @param sym_len       buffer size.
 * @param locale_index  index of the locale.
 * @return 0 on success, -1 otherwise.
 */
JSR238_STATUSCODE jsr238_get_date_format_symbols(jbyte* symbols, jint sym_len, jint locale_index) {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_date_format_symbols()\n");

    return jsr238_get_resource(symbols, sym_len, DATETIME_FORMAT_SYMBOL_RESOURCE_ID,
                               (locale_index < empty_locale_index ? locale_index : locale_index + 1));
}

/**
 * Gets size of number formatting symbols.
 *
 * @param locale_index  index of the locale.
 * @return size of formatting symbols (in bytes), -1 on any error.
 */
JSR238_STATUSCODE jsr238_get_number_format_symbols_length(jsize* length /* OUT */, jint locale_index) {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_number_format_symbols_length()\n");

    return jsr238_get_resource_length(length, NUMBER_FORMAT_SYMBOL_RESOURCE_ID,
                                      (locale_index < empty_locale_index ? locale_index : locale_index + 1));
}

/**
 * Gets number formatting symbols as an array of bytes.
 *
 * @param symbols       buffer to store the formatting symbols.
 * @param sym_len       buffer size.
 * @param locale_index  index of the locale.
 * @return 0 on success, -1 otherwise.
 */
JSR238_STATUSCODE jsr238_get_number_format_symbols(jbyte* symbols, jint sym_len, jint locale_index) {
    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_number_format_symbols()\n");

    return jsr238_get_resource(symbols, sym_len, NUMBER_FORMAT_SYMBOL_RESOURCE_ID,
                               (locale_index < empty_locale_index ? locale_index : locale_index + 1));
}
