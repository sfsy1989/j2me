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
 * SdpEncoderImpl.java
 *
 * Created on January 14, 2002, 11:40 AM
 */
package gov.nist.javax.sdp;

import java.io.*;

/**
 * Implementation of SDP encoder.
 *
 * @version  JSR141-PUBLIC-REVIEW (Subject to change).
 *
 *
 * <a href="{@docRoot}/uncopyright.html">This code is in the public domain.</a>
 *
 */
public class SdpEncoderImpl {

    /** Creates new SdpEncoderImpl */
    public SdpEncoderImpl() {
    }

    /**
     * Specifies the character set to be used to display the session name and
     * information data. By default, the ISO-10646 character set in UTF-8 
     * encoding is used. If a more compact representation is required, other 
     * character sets may be used such as ISO-8859-1 for Northern European 
     * languages.
     * 
     * @param enc enc - name of character encoding to use for session name
     *  and information data
     * @throws UnsupportedEncodingException if the named encoding 
     * is not supported
     */
    public void setEncoding(String enc) throws UnsupportedEncodingException {
        throw new UnsupportedEncodingException("Method not supported"); 
    }
    
    /**
     * Specifies whether to try to write "typed-time" fields instead
     * of raw integer values. This makes the session description more
     * readable but may have an adverse effect on serialization time.
     *
     * Ordinarily time values are given in numbers of seconds, but for
     * readability they may be specified using logical units. From RFC 2327:
     *
     *  To make announcements more compact, times may also be given in units of
     *  days, hours or minutes. The syntax for these is a number immediately
     *  followed by a single case-sensitive character. Fractional units are not
     *  allowed - a smaller unit should be used instead. The following unit
     *  specification characters are allowed:
     *
     *     d - days (86400 seconds)
     *     h - minutes (3600 seconds)
     *     m - minutes (60 seconds)
     *     s - seconds (allowed for completeness but not recommended)
     *
     * @param flag  if true this Outputter should emit "typed" time
     *  specs in preference to untyped times.
     */    
    public void setTypedTime(boolean flag) {
        
    }

    /**
     * Specifies whether to generate "a=rtpmap:" attributes for static
     * RTP/AVP format strings.
     * This is recommended but makes messages bigger. The default is
     * not to write such  attributes.
     * @param flag  if true "a=rtpmap:" attributes are generated for 
     * all "RTP/AVP" formats
     */    
    public void setRtpmapAttribute(boolean flag) {
       
    }

    
    
}
