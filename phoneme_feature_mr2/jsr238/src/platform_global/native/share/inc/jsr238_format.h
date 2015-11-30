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

#ifndef __JSR238_FORMAT_H_
#define __JSR238_FORMAT_H_

/**
 * @file
 * Header file for low-level formatting porting API of JSR 238 (MI18N).
 * @ingroup jsr238porting_layer
 * @brief #include <jsr238_format.h>
 * @{
 *
 * This file defines the porting interface for native implementation of
 * of data/number formatting functionality.
 */
#include <jsr238.h>

/**
 * Gets the number of supported locales for data formatting
 * @param pcount out count of collation locales
 *
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 */
extern JSR238_STATUSCODE jsr238_get_format_locales_count(/* OUT */jint* pcount);

/**
 * Gets locale name for data formatting with the given index.
 *
 * @param locale_index of the locale. If neutral locale supported its index MUST be 0
 * @param locale_name_out buffer for the locale.
 * @param plen	in - length of output buffer,
 *				out - length of returned string in jchars including terminating zero 
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 */
extern JSR238_STATUSCODE jsr238_get_format_locale_name(jint index, /* OUT */ jchar* locale_name_out, /* IN|OUT */ jint* plen);


/**
 * Gets locale index used for date/number formatting by the given locale name
 *
 * @param locale_name string containing requested locale name, null for neutral locale
 * @param pindex out index of requested locale
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 */
extern JSR238_STATUSCODE jsr238_get_format_locale_index(const jchar* locale_name, /* OUT */ jint* pindex);


/**
 * Formats a date as a date string for a specified locale.
 * 
 * @param locale_index index of locale to select.
 * @param year current year
 * @param month current month
 * @param dow current day of the week
 * @param dom current day of the month
 * @param hour current hour
 * @param min current minute
 * @param sec current second
 * @param style formatting style (0-5)
 * @param res_buffer the buffer to store formatted string
 * @param pbuffer_len	in the length of buffer to store string
 *						out the length of formatted string
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 * @note If <code>buffer_len</code> is zero, the function returns the number 
 *       of unicode characters required to hold the formatted currency string,
 *       and the buffer is not used
 */
extern JSR238_STATUSCODE jsr238_format_date_time(jint locale_index, jint year, jint month, 
                             jint dow, jint dom, 
                             jint hour, jint min, jint sec, 
                             jint style,
							 /* OUT */ jchar *res_buffer,  /* IN|OUT */ jint* pbuffer_len);

/**
 * Formats a double number for a specified locale.
 * 
 * @param locale_index index of locale to select.
 * @param d double number to format
 * @param decimals the number of fractional digits
 * @param res_buffer the buffer to store formatted string
 * @param pbuffer_len	in the length of buffer to store string
 *						out the length of formatted string
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 * @note If <code>res_len</code> is zero, the function returns the number 
 *       of unicode characters required to hold the formatted currency string,
 *       and the buffer is not used
 */
extern JSR238_STATUSCODE jsr238_format_double_number(jint locale_index, jdouble d, jint decimals,
													 /* OUT */ jchar *res_buffer,  /* IN|OUT */ jint* pbuffer_len);


/**
 * Formats a integer numberfor a specified locale.
 * 
 * @param locale_index index of locale to select.
 * @param l long intefer number to format
 * @param res_buffer the buffer to store formatted string
 * @param pbuffer_len	in the length of buffer to store string
 *						out the length of formatted string
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 * @note If <code>res_len</code> is zero, the function returns the number 
 *       of unicode characters required to hold the formatted currency string,
 *       and the buffer is not used
 */
extern JSR238_STATUSCODE jsr238_format_integer_number(jint locale_index, jlong l,
													  /* OUT */ jchar *res_buffer,  /* IN|OUT */ jint* pbuffer_len);

/**
 * Formats a number string as a currency string for a specified locale.
 * 
 * @param locale index of locale to select.
 * @param d double value to format
 * @param code  the ISO 4217 currency code to use, if null default currency for locale is used
 * @param res_buffer the buffer to store formatted string
 * @param pbuffer_len	in the length of buffer to store string
 *						out the length of formatted string
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 * @note If <code>res_len</code> is zero, the function returns the number 
 *       of unicode characters required to hold the formatted currency string,
 *       and the buffer is not used
 */
extern JSR238_STATUSCODE jsr238_format_currency(jint locale, double d, const jchar* code,
												/* OUT */ jchar *res_buffer,  /* IN|OUT */ jint* pbuffer_len);

/**
 * Formats a float number as a percentage string customized for a specified locale.
 * A percentage is expressed as a decimal number, with the value 0.0 interpreted as 0%
 * and the value 1.0 as 100%. Percentages larger than 100% are expressed as values greater than 1.0.
 * Negative percentages are allowed, and expressed as values smaller than 0.0.
 * The percentage is rounded to the specified number of decimals.
 * @param locale_index index of locale to select.
 * @param f float value to format
 * @param decimals the number of fractional digits in result, if decimals is set to -1 number of decimal is taken by default for given locale
 * @param res_buffer the buffer to store formatted string
 * @param pbuffer_len	in the length of buffer to store string
 *						out the length of formatted string
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 * @note If <code>res_len</code> is zero, the function returns the number 
 *       of javacall_utf16 characters required to hold the formatted number string,
 *       and the buffer is not used
 */
extern JSR238_STATUSCODE jsr238_format_float_percentage(jint locale_index, jfloat f, jint decimals,
														/* OUT */ jchar *res_buffer,  /* IN|OUT */ jint* pbuffer_len);

/**
 * Formats an integral percentage value using locale-specific rules. 
 * @param locale_index index of locale to select.
 * @param l long integer to format
 * @param res_buffer the buffer to store formatted string
 * @param pbuffer_len	in the length of buffer to store string
 *						out the length of formatted string
 * @return JSR238_STATUSCODE_OK if all done successfuly, 
 *         JSR238 error code otherwise 
 * @note If <code>res_len</code> is zero, the function returns the number 
 *       of javacall_utf16 characters required to hold the formatted number string,
 *       and the buffer is not used
 */
extern JSR238_STATUSCODE jsr238_format_integer_percentage(jint locale_index, jlong l,
														  /* OUT */ jchar *res_buffer,  /* IN|OUT */ jint* pbuffer_len);

/** @} */

#endif /* __JSR238_FORMAT_H_ */
