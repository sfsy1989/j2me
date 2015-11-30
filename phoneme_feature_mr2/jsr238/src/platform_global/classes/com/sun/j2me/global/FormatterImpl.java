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

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.Hashtable;
import javax.microedition.global.Formatter;
import javax.microedition.global.ResourceException;
import javax.microedition.global.UnsupportedLocaleException;
import com.sun.midp.log.Logging;
import com.sun.midp.log.LogChannels;

/**
 * This class actually realizes most of the methods of
 * {@link javax.microedition.global.Formatter}. Specifically, these are:
 * <ul>
 *   <li> {@link #formatDateTime(Calendar, int)}
 *   <li> {@link #formatCurrency(double)}
 *   <li> {@link #formatCurrency(double, String)}
 *   <li> {@link #formatNumber(double)}
 *   <li> {@link #formatNumber(double, int)}
 *   <li> {@link #formatNumber(long)}
 *   <li> {@link #formatPercentage(long number)}
 *   <li> {@link #formatPercentage(float, int)}
 * </ul>
 * <p>
 * This realization of <code>Formatter</code> is intended to be based on
 * some native functions for formatting support. Regarding this,
 * <code>FormatterImpl</code> uses locale index that points to its locale
 * instead of working with explicit locale code.
 */
public class FormatterImpl implements CommonFormatter {

    /**
     *  Current FormatterImpl locale index in the list of supported locales.
     */
    private int locale_index;

    /**
     * Constructs a <code>Formatter</code> implementation for the specified
     * locale.
     *
     * @param  locale  desired <code>FormatterImpl</code> locale
     * @throws UnsupportedLocaleException  The exception
     *      is thrown when locale isn't supported.
     */
    public FormatterImpl(String locale) throws UnsupportedLocaleException {
		locale_index = FormatAbstractionLayerImpl.getFormatLocaleIndex(locale);
		if (locale_index < 0) {
			throw new UnsupportedLocaleException("Locale \""
												 + locale +
												 "\" unsupported.");
		}
    }


    /**
     * Formats a date/time instance using locale-specific rules.
     * The <code>style</code> parameter determines the extent and style
     * of the result. <p>
     * If <code>dateTime</code> has an associated
     * <code>java.util.TimeZone</code>
     * object, <code>dateTime</code> MUST be interpreted as "wall time", not
     * universal time. The offset of that time zone MUST be reflected in the
     * formatted time.
     *
     * @param  dateTime         the date and time to format
     * @param  style            the formatting style to use
     * @return                  the date and/or time formatted as a string
     */
    public String formatDateTime(Calendar dateTime, int style) {
            return formatDateTime0(locale_index,
                                   dateTime.get(Calendar.YEAR), 
                                   dateTime.get(Calendar.MONTH) + 1, 
                                   dateTime.get(Calendar.DAY_OF_WEEK),
                                   dateTime.get(Calendar.DAY_OF_MONTH),
                                   dateTime.get(Calendar.HOUR_OF_DAY),
                                   dateTime.get(Calendar.MINUTE),
                                   dateTime.get(Calendar.SECOND),
                                   style);
    }

    private native String formatDateTime0(int locale,
                                          int year, int month,
                                          int dayOfWeek, int day,
                                          int hour, int minute, int secs,
                                          int style);


    /**
     * Formats a currency amount using locale-specific rules. This method
     * assumes that the amount is in the locale's currency. The result uses the
     * locale-specific decimal separator and may include grouping separators.
     * The number of decimals is also locale-specific. <p>
     *
     * This method does not perform any currency conversions based on exchange
     * rates.
     *x
     * @param  number  the currency amount to format
     * @return         the formatted currency amount
     */
    public String formatCurrency(double number) {
        return formatCurrency(number, null);
    }

    /**
     * Formats a currency amount using the locale-specific rules but using the
     * symbol of the specified currency. The currency is specified using its
     * ISO 4217 three-letter code, such as "USD", "EUR" or "GBP". If there is a
     * currency symbol attached to the ISO 4217 code in the implementation,
     * that symbol MUST be used instead of the locale's currency symbol. If the
     * implementation does not provide a currency symbol for a given ISO 4217
     * code, the code MUST be used as such. <p>
     *
     * The result uses the locale-specific decimal separator and may include
     * grouping separators. The number of decimals is also locale-specific. <p>
     *
     * This method does not perform any currency conversions based on exchange
     * rates.
     *
     * @param  number        the currency amount to format
     * @param  currencyCode  the ISO 4217 currency code to use
     * @return               formatted currency amount
     */
    public String formatCurrency(double number, String currencyCode) {
        if (Double.isNaN(number) || Double.isInfinite(number)) {
            return new Double(number).toString();
        }
        return formatCurrency0(locale_index, number, currencyCode);
    }

    private native String formatCurrency0(int locale, double number, String currency);


    /**
     * Formats a decimal number using locale-specific rules. The result
     * includes a locale-specific decimal separator and may include grouping
     * separators. <p>
     *
     * The symbols used for negative and positive infinity and NaN are
     * implementation-specific. Implementations MAY use the appropriate Unicode
     * character (U+221F INFINITY) if applicable.
     *
     * @param  number  the number to format
     * @return         formatted decimal number
     */
    public String formatNumber(double number) {
        if (Double.isNaN(number) || Double.isInfinite(number)) {
            return new Double(number).toString();
        }
        return formatNumber0(locale_index, number, -1);
    }


    /**
     * Formats a decimal number using locale-specific rules, with the specified
     * number of decimals. The result includes a locale-specific decimal
     * separator and may include grouping separators. <p>
     *
     * The number of decimals MUST be between 1 and 15. The formatted result
     * MUST have exactly the specified number of decimals, even if some of the
     * trailing digits are zeroes. <p>
     *
     * The symbols used for negative and positive infinity and NaN are
     * implementation-specific. Implementations MAY use the appropriate Unicode
     * character (U+221F INFINITY) if applicable.
     *
     * @param  number    the number to format
     * @param  decimals  number of decimals
     * @return           formatted decimal number
     */
    public String formatNumber(double number, int decimals) {
		if (decimals<=0 || decimals>15){
			throw new IllegalArgumentException("Wrong decimals parameter");
		}
        if (Double.isNaN(number) || Double.isInfinite(number)) {
            return new Double(number).toString();
        }
        return formatNumber0(locale_index, number, decimals);
    }


    /**
     * Formats an integer using locale-specific rules. The result may include
     * grouping separators.
     *
     * @param  number  the number to format
     * @return         formatted integer number
     */
    public String formatNumber(long number) {
        return formatNumber1(locale_index, number);
    }

    private native String formatNumber0(int locale, double number, int decimals);

	private native String formatNumber1(int locale, long number);

    /**
     * Formats an integral percentage value using locale-specific rules. This
     * method places the locale-specific percent sign at the correct position
     * in relation to the number, with the appropriate number of space
     * (possibly none) between the sign and the number. <p>
     *
     * A percentage is expressed as an integer value. Negative percentages are
     * allowed.
     *
     * @param  number  the number to format
     * @return         formatted percentage number
     */
    public String formatPercentage(long number) {
        return formatPercentage1(locale_index, number);
    }

    /**
     * Formats a percentage with the specified number of decimals using
     * locale-specific rules. This method places the locale-specific percent
     * sign at the correct position in relation to the number, with the
     * appropriate amount of space (possibly none) between the sign and the
     * number. <p>
     *
     * A percentage is expressed as a decimal number, with the value 0.0
     * interpreted as 0% and the value 1.0 as 100%. Percentages larger than
     * 100% are expressed as values greater than 1.0. Negative percentages are
     * allowed, and expressed as values smaller than 0.0. The percentage is
     * rounded to the specified number of decimals. <p>
     *
     * The number of decimals MUST be between 1 and 15. The formatted result
     * MUST have exactly the specified number of decimals, even if some of the
     * trailing digits are zeroes.
     *
     * @param  number    the percentage to format, expressed as a positive or
     *      negative number
     * @param  decimals  the number of decimals to use (1 &lt;= decimals &lt;=
     *      15)
     * @return           formatted percentage string
     */
    public String formatPercentage(float number, int decimals) {
		if (decimals<=0 || decimals>15){
			throw new IllegalArgumentException("Wrong decimals parameter");
		}
        if (Float.isNaN(number) || Float.isInfinite(number)) {
            return new Float(number).toString();
        }
        return formatPercentage0(locale_index, number, decimals);
    }

    private native String formatPercentage0(int locale_index, float number, int decimals);

	private native String formatPercentage1(int locale_index, long number);
}
