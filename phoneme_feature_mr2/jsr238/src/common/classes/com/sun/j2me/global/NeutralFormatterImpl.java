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

import java.util.Calendar;

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
 * This realization of <code>Formatter</code> is for locale-neutral
 * formatting only. Therefore, it is common for all implementations and
 * platforms.
 */
public class NeutralFormatterImpl implements CommonFormatter {

    /**
     * Default constructor of locale-neutral Formatter realization class.
     */
    NeutralFormatterImpl() { }

    /**
     * Formats a date/time instance using ISO 8601 (RFC 3339) rules.
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
        DateFormatSymbols dfs = new DateFormatSymbols();
        dfs.patterns[0] = "yyyy-MM-dd"; // DATE_SHORT
        dfs.patterns[1] = "yyyy-MM-dd"; // DATE_LONG
        dfs.patterns[2] = "HH:mm:ssz"; // TIME_SHORT
        dfs.patterns[3] = "HH:mm:ssz"; // TIME_LONG
        dfs.patterns[4] = "{0}T{1}"; // DATETIME_SHORT
        dfs.patterns[5] = "{0}T{1}"; // DATETIME_LONG
        DateTimeFormat dtf = new DateTimeFormat(style, dfs);
        NumberFormat nf = new NumberFormat(NumberFormat.INTEGERSTYLE,
                new NumberFormatSymbols());
        return dtf.format(dateTime, nf);
    }


    /**
     * Formats a currency amount using locale-specific rules. This method
     * assumes that the amount is in the locale's currency. The result uses the
     * locale-specific decimal separator and may include grouping separators.
     * The number of decimals is also locale-specific. <p>
     *
     * This method does not perform any currency conversions based on exchange
     * rates.
     *
     * @param  number  the currency amount to format
     * @return         the formatted currency amount
     */
    public String formatCurrency(double number) {
        return formatCurrency(number, "");
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
        if ((Double.isNaN(number))||
           (Double.isInfinite(number))) {
           return Double.toString(number);
        } else {
            return formatNumber(number) + currencyCode;
        }
    }


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
        return Double.toString(number);
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

	// rounding to decimals fraction digits
	double rounder = 0.5;
	for (int power = 0; power < decimals; ++power) {
	    rounder /= 10;
	}
	if (number >= 0 )
	  	number += rounder;
	else
		number -= rounder;

        String num = (new Double(number)).toString();
        int ePos = num.indexOf('E');
        int dotPos = num.indexOf('.');

        if (ePos != -1) {
            StringBuffer mantissa = new StringBuffer(num.substring(0, dotPos) +
                    num.substring(dotPos + 1, ePos));
            int eVal = Integer.parseInt(num.substring(ePos + 1));
            if (eVal > 0) {
                while (eVal >= ePos - dotPos) {
                    mantissa.append('0');
                    ePos++;
                }
                dotPos += eVal;
            } else {
                int insPos = num.indexOf('-') + 1;
                if (insPos > 1) {
                    insPos = 0;
                }
                while (eVal++ < 0) {
                    mantissa.insert(insPos, '0');
                }
                dotPos = insPos + 1;
            }
            num = mantissa.toString();
            num = num.substring(0, dotPos) + '.' + num.substring(dotPos);
        }

        dotPos++;
        num += "0000000000000000";
        num = num.substring(0, dotPos + decimals);
        return num;
    }


    /**
     * Formats an integer using locale-specific rules. The result may include
     * grouping separators.
     *
     * @param  number  the number to format
     * @return         formatted integer number
     */
    public String formatNumber(long number) {
        return Long.toString(number);
    }


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
        return Long.toString(number) +
                NumberFormat.NONLOCALIZED_PERCENT_SIGN;
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
        return formatNumber((double)number * 100, decimals) +
                NumberFormat.NONLOCALIZED_PERCENT_SIGN;
    }
}
