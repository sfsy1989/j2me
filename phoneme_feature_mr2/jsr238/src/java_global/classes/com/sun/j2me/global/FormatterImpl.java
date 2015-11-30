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
import java.util.Hashtable;
import javax.microedition.global.Formatter;
import javax.microedition.global.ResourceManager;
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
 */
public class FormatterImpl implements CommonFormatter {

    /**
     * Current FormatterImpl locale code.
     */
    private String locale;

    /**
     * Constructs a formatter implementation for the specified locale.
     *
     * @param  locale  desired FormatterImpl locale.
     * @throws UnsupportedLocaleException  The exception
     *      is thrown when locale isn't supported.
     */
    public FormatterImpl(String locale) throws UnsupportedLocaleException {
        int locale_index = LocaleHelpers.indexInSupportedLocales(locale,
                Formatter.getSupportedLocales());
        if (locale_index == -1) {
            throw new UnsupportedLocaleException("Locale \""
                                                 + locale +
                                                 "\" unsupported.");
        }

        this.locale = locale;
    }


    /**
     * Creates and returns a set of symbols for number formatting.
     *
     * @return  <code>NumberFormatSymbols</code> instance for current locale
     */
    private NumberFormatSymbols getNumberFormatSymbols() {
        try {
            ResourceManager rm = ResourceManager.getManager(
                    ResourceManager.DEVICE, locale);
            NumberFormatSymbols symbols = (NumberFormatSymbols)rm.getResource(
                    Constants.NUMBER_FORMAT_SYMBOL_RESOURCE_ID);
            return symbols;
        } catch (NullPointerException npe_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.ERROR) {
                Logging.report(Logging.ERROR, LogChannels.LC_JSR238,
                    "Base name or locale is null");
            }
        } catch (IllegalArgumentException iae_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.ERROR) {
                Logging.report(Logging.ERROR, LogChannels.LC_JSR238,
                    "Locale identifier is not valid" + locale);
            }
        } catch (ResourceException re_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.ERROR) {
                Logging.report(Logging.ERROR, LogChannels.LC_JSR238,
                    "No resources are found or the resource file is invalid");
            }
        } catch (UnsupportedLocaleException ule_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.ERROR) {
                Logging.report(Logging.ERROR, LogChannels.LC_JSR238,
                    "Locale is not listed in the meta-information file"
                    + locale);
            }
        }
        if (Logging.REPORT_LEVEL <= Logging.WARNING) {
            Logging.report(Logging.WARNING, LogChannels.LC_JSR238,
                "Using neutral locale symbols for locale: " 
                + locale);
        }
        return new NumberFormatSymbols();
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
        try {
            ResourceManager rm = ResourceManager.getManager(
                    ResourceManager.DEVICE, locale);
            DateFormatSymbols symbols = (DateFormatSymbols)rm.getResource(
                    Constants.DATETIME_FORMAT_SYMBOL_RESOURCE_ID);
            DateTimeFormat dtf = new DateTimeFormat(style, symbols);
            NumberFormat nf = new NumberFormat(NumberFormat.INTEGERSTYLE,
                                               getNumberFormatSymbols());
            return dtf.format(dateTime, nf);
        } catch (NullPointerException npe_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.WARNING) {
                Logging.report(Logging.WARNING, LogChannels.LC_JSR238,
                    "Base name or locale is null");
            }
        } catch (IllegalArgumentException iae_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.WARNING) {
                Logging.report(Logging.WARNING, LogChannels.LC_JSR238,
                    "locale identifier is not valid" + locale);
            }
        } catch (ResourceException re_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.WARNING) {
                Logging.report(Logging.WARNING, LogChannels.LC_JSR238,
                    "No resources are found or the resource file is invalid");
            }
        } catch (UnsupportedLocaleException ule_ignore) {
            /* intentionally ignored */
            if (Logging.REPORT_LEVEL <= Logging.WARNING) {
                Logging.report(Logging.WARNING, LogChannels.LC_JSR238,
                    "Locale is not listed in the meta-information file"
                    + locale);
            }
        }
        return "";
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
        NumberFormat nf = new NumberFormat(NumberFormat.CURRENCYSTYLE,
                                           getNumberFormatSymbols());
        if (Double.isInfinite(number) ||
            Double.isNaN(number)) {
            String oldSymbol = nf.setCurrencySymbol("");
            String formatted = nf.format(number);
            nf.setCurrencySymbol(oldSymbol);
            return formatted;
        } else {
            return nf.format(number);
        }
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
        NumberFormat nf = new NumberFormat(NumberFormat.CURRENCYSTYLE,
                                           getNumberFormatSymbols());
        String symbol = "";
        
        if (!(Double.isInfinite(number) ||
                Double.isNaN(number))) {
	        symbol = nf.getCurrencySymbolForCode(currencyCode);
	        if (symbol == null) {
	            symbol = currencyCode;
	        }
        }
        
        String oldSymbol = nf.setCurrencySymbol(symbol);
        String formatted = nf.format(number);
        nf.setCurrencySymbol(oldSymbol);

        return formatted;
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
        NumberFormat nf = new NumberFormat(NumberFormat.NUMBERSTYLE,
                                           getNumberFormatSymbols());
        return nf.format(number);
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
        NumberFormat nf = new NumberFormat(NumberFormat.NUMBERSTYLE,
                                           getNumberFormatSymbols());
        nf.setMinimumFractionDigits(decimals);
        nf.setMaximumFractionDigits(decimals);
        return nf.format(number);
    }


    /**
     * Formats an integer using locale-specific rules. The result may include
     * grouping separators.
     *
     * @param  number  the number to format
     * @return         formatted integer number
     */
    public String formatNumber(long number) {
        NumberFormat nf = new NumberFormat(NumberFormat.INTEGERSTYLE,
                                           getNumberFormatSymbols());
        return nf.format(number);
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
        NumberFormat nf = new NumberFormat(NumberFormat.PERCENTSTYLE,
                                           getNumberFormatSymbols());
        nf.setMultiplier(1);
        return nf.format(number);
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
        NumberFormat nf = new NumberFormat(NumberFormat.PERCENTSTYLE,
                                           getNumberFormatSymbols());
        nf.setMinimumFractionDigits(1);
        nf.setMaximumFractionDigits(decimals);
        return nf.format(number);
    }
}
