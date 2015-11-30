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

package javax.microedition.sip;

import java.util.Vector;

import gov.nist.core.NameValueList;
import gov.nist.core.ParseException;
import gov.nist.microedition.sip.StackConnector;
import gov.nist.siplite.header.Header;

import com.sun.midp.i3test.TestCase;

/**
 * SipHeaderChecker is a class to check SipHeader functionality
 *
 */
class SipHeaderChecker {
    /** Short form of header. */
    private static String shortForm =
	"i call-id:m contact:e content-encoding:l content-length:" +
	"c content-type:f from:s subject:k supported:t to:v via:";

    /**
     * Output from short format string.
     * @param name label for format string
     * @return formatted string
     */
    public static String toShortForm(String name) {

        name = name.toLowerCase();
        int i = shortForm.indexOf(" " + name + ":");
        return i == -1 ? name : shortForm.substring(i - 1, i);
    }

    /**
     * Output to long format string.
     * @param name label to process
     * @return formatted string
     */
    public static String toLongForm(String name) {

        name = name.toLowerCase();
        int i = shortForm.indexOf(name + " ");
        return i == -1 ? name :
	    shortForm.substring(i + 2, shortForm.indexOf(':', i));
    }
    /** Array of testcases. */
    static SipHeaderChecker[] testCases = {

	// # 1
	new SipHeaderChecker("Accept:" +
			     " application/sdp;level=1",
			     "application/sdp",
			     new String[][] {{"level", "1"}}),

	// # 2
	new SipHeaderChecker("aCCEPT:" +
			     " application/sdp;level=1",
			     "application/sdp",
			     new String[][] {{"level", "1"}}),

	// # 3
	new SipHeaderChecker("Accept:" +
			     " application/sdp;gparam1=1;gparam2",
			     "application/sdp",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 4
	new SipHeaderChecker("Accept:" +
			     " application/sdp;gparam2;gparam1=1",
			     "application/sdp",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 5
	new SipHeaderChecker("Accept:" +
			     " application/sdp;gparam2",
			     "application/sdp",
			     new String[][] {{"gparam2", ""}}),

	// # 6
	new SipHeaderChecker("Accept-Encoding:" +
			     " gzip",
			     "gzip",
			     new String[][] {}),

	// # 7
	new SipHeaderChecker("Accept-EncoDING:" +
			     " gzip",
			     "gzip",
			     new String[][] {}),

	// # 8
	new SipHeaderChecker("Accept-Language:" +
			     " da",
			     "da",
			     new String[][] {}),

	// # 9
	new SipHeaderChecker("Accept-Language:" +
			     " en-gb;q=0.8",
			     "en-gb",
			     new String[][] {{"q", "0.8"}}),

	// # 10
	new SipHeaderChecker("Accept-LanguaGE:" +
			     " en-gb;q=0.8",
			     "en-gb",
			     new String[][] {{"q", "0.8"}}),

	// # 11
	new SipHeaderChecker("Accept-Language:" +
			     " en-gb;Q=0.8",
			     "en-gb",
			     new String[][] {{"q", "0.8"}}),

	// # 12
	new SipHeaderChecker("Accept-Language:" +
			     " en-gb;gparam2;gparam1=1",
			     "en-gb",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 13
	new SipHeaderChecker("Alert-Info:" +
			     " <http://www.example.com/sounds/moo.wav>",
			     "<http://www.example.com/sounds/moo.wav>",
			     new String[][] {}),

	// # 14
	new SipHeaderChecker("Alert-info:" +
			     " <http://www.example.com/sounds/moo.wav>",
			     "<http://www.example.com/sounds/moo.wav>",
			     new String[][] {}),

	// # 15
	new SipHeaderChecker("Alert-Info:" +
			     " <http://www.example.com/sounds/moo.wav>"
			     + ";gparam2;gparam1=1",
			     "<http://www.example.com/sounds/moo.wav>",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 16
	new SipHeaderChecker("Allow:" +
			     " INVITE",
			     "INVITE",
			     new String[][] {}),

	// # 17
	new SipHeaderChecker("Allow:" +
			     " ACK",
			     "ACK",
			     new String[][] {}),

	// # 18
	new SipHeaderChecker("Allow:" +
			     " OPTIONS",
			     "OPTIONS",
			     new String[][] {}),

	// # 19
	new SipHeaderChecker("Allow:" +
			     " CANCEL",
			     "CANCEL",
			     new String[][] {}),

	// # 20
	new SipHeaderChecker("Allow:" +
			     " BYE",
			     "BYE",
			     new String[][] {}),

	// # 21
	new SipHeaderChecker("alloW:" +
			     " INVITE",
			     "INVITE",
			     new String[][] {}),

	// # 22
	new SipHeaderChecker("Authentication-Info:" +
			     " nextnonce=\"47364c23432d2e131a5fb210812c\"",
			     "nextnonce=\"47364c23432d2e131a5fb210812c\"",
			     new String[][] {}),

	// # 23
	new SipHeaderChecker("Authentication-INFO:" +
			     " nextnonce=\"47364c23432d2e131a5fb210812c\"",
			     "nextnonce=\"47364c23432d2e131a5fb210812c\"",
			     new String[][] {}),

	// # 24
	new SipHeaderChecker("Authorization:" +
			     " Digest username=\"Alice\","
			     + " realm=\"atlanta.com\","
			     +" nonce=\"84a4cc6f3082121f32b42a2187831a9e\","
			     + " response="
			     + "\"7587245234b3434cc3412213e5f113a5432\"",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"response",
			       "\"7587245234b3434cc3412213e5f113a5432\""},
			      {"nonce",
			       "\"84a4cc6f3082121f32b42a2187831a9e\""},
			      {"username", "\"Alice\""}}),

	// # 25
	new SipHeaderChecker("AuthorizatioN:" +
			     " Digest username=\"Alice\","
			     + " realm=\"atlanta.com\","
			     + " nonce=\"84a4cc6f3082121f32b42a2187831a9e\","
			     + " response="
			     + "\"7587245234b3434cc3412213e5f113a5432\"",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"response",
			       "\"7587245234b3434cc3412213e5f113a5432\""},
			      {"nonce",
			       "\"84a4cc6f3082121f32b42a2187831a9e\""},
			      {"username", "\"Alice\""}}),

	// # 26
	new SipHeaderChecker("Authorization:" +
			     " Digest username=\"Alice\","
			     + " realm=\"atlanta.com\","
			     + " nonce=\"84a4cc6f3082121f32b42a2187831a9e\","
			     + " response="
			     + "\"7587245234b3434cc3412213e5f113a5432\","
			     + "auth-param1=1,auth-param2=\"param2\"",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"response",
			       "\"7587245234b3434cc3412213e5f113a5432\""},
			      {"nonce",
			       "\"84a4cc6f3082121f32b42a2187831a9e\""},
			      {"auth-param2", "\"param2\""},
			      {"auth-param1", "1"},
			      {"username", "\"Alice\""}}),

	// # 27
	new SipHeaderChecker("Call-ID:" +
			     " f81d4fae-7dec-11d0-a765-00a0c91e6bf6@biloxi.com",
			     "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@biloxi.com",
			     new String[][] {}),

	// # 28
	new SipHeaderChecker("Call-ID:" +
			     " f81d4fae-7dec-11d0-a765-00a0c91e6bf6",
			     "f81d4fae-7dec-11d0-a765-00a0c91e6bf6",
			     new String[][] {}),

	// # 29
	new SipHeaderChecker("Call-Id:" +
			     " f81d4fae-7dec-11d0-a765-00a0c91e6bf6@biloxi.com",
			     "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@biloxi.com",
			     new String[][] {}),

	// # 30
	new SipHeaderChecker("i:" +
			     "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@192.0.2.4",
			     "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@192.0.2.4",
			     new String[][] {}),

	// # 31
	new SipHeaderChecker("Call-Info:" +
			     " <http://wwww.example.com/alice/photo.jpg>"
			     +" ;purpose=icon",
			     "<http://wwww.example.com/alice/photo.jpg>",
			     new String[][] {{"purpose", "icon"}}),

	// # 32
	new SipHeaderChecker("Call-INFO:" +
			     " <http://wwww.example.com/alice/photo.jpg>"
			     + " ;purpose=icon",
			     "<http://wwww.example.com/alice/photo.jpg>",
			     new String[][] {{"purpose", "icon"}}),

	// # 33
	new SipHeaderChecker("Call-Info:" +
			     " <http://wwww.example.com/alice/photo.jpg>"
			     + " ;purpose=icon;gparam2;gparam1=1",
			     "<http://wwww.example.com/alice/photo.jpg>",
			     new String[][] {{"purpose", "icon"},
					     {"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 34
	new SipHeaderChecker("Contact:" +
			     " \"Mr. Watson\""
			     + " <sip:watson@worcester.bell-telephone.com>;"
			     + "q=0.7; expires=3600",
			     "\"Mr. Watson\""
			     + " <sip:watson@worcester.bell-telephone.com>",
			     new String[][] {{"expires", "3600"},
					     {"q", "0.7"}}),

	// # 35
	new SipHeaderChecker("ContacT:" +
			     " \"Mr. Watson\""
			     + " <sip:watson@worcester.bell-telephone.com>;"
			     + "q=0.7; expires=3600",
			     "\"Mr. Watson\""
			     + " <sip:watson@worcester.bell-telephone.com>",
			     new String[][] {{"expires", "3600"},
					     {"q", "0.7"}}),

	// # 36
	new SipHeaderChecker("Contact:" +
			     " \"Mr. Watson\""
			     + " <sip:watson@worcester.bell-telephone.com>;"
			     + "q=0.7; expires=3600;gparam2;gparam1=1",
			     "\"Mr. Watson\""
			     + " <sip:watson@worcester.bell-telephone.com>",
			     new String[][] {{"expires", "3600"},
					     {"gparam2", ""},
					     {"gparam1", "1"},
					     {"q", "0.7"}}),

	// # 37
	new SipHeaderChecker("m:" +
			     " <sips:bob@192.0.2.4>;expires=60",
			     "<sips:bob@192.0.2.4>",
			     new String[][] {{"expires", "60"}}),

	// # 38
	new SipHeaderChecker("M:" +
			     " <sips:bob@192.0.2.4>;expires=60",
			     "<sips:bob@192.0.2.4>",
			     new String[][] {{"expires", "60"}}),

	// # 39
	new SipHeaderChecker("Content-Disposition:" +
			     " session",
			     "session",
			     new String[][] {}),

	// # 40
	new SipHeaderChecker("Content-DispositioN:" +
			     " session",
			     "session",
			     new String[][] {}),

	// # 41
	new SipHeaderChecker("Content-Disposition:" +
			     " session;gparam2;gparam1=1",
			     "session",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 42
	new SipHeaderChecker("Content-Encoding:" +
			     " gzip",
			     "gzip",
			     new String[][] {}),

	// # 43
	new SipHeaderChecker("Content-EncodinG:" +
			     " gzip",
			     "gzip",
			     new String[][] {}),

	// # 44
	new SipHeaderChecker("e:" +
			     " tar",
			     "tar",
			     new String[][] {}),

	// # 45
	new SipHeaderChecker("E:" +
			     " tar",
			     "tar",
			     new String[][] {}),

	// # 46
	new SipHeaderChecker("Content-Language:" +
			     " fr",
			     "fr",
			     new String[][] {}),

	// # 47
	new SipHeaderChecker("Content-LanguaGE:" +
			     " fr",
			     "fr",
			     new String[][] {}),

	// # 48
	new SipHeaderChecker("Content-Length:" +
			     " 349",
			     "349",
			     new String[][] {}),

	// # 49
	new SipHeaderChecker("Content-length:" +
			     " 349",
			     "349",
			     new String[][] {}),

	// # 50
	new SipHeaderChecker("l:" +
			     " 173",
			     "173",
			     new String[][] {}),

	// # 51
	new SipHeaderChecker("L:" +
			     " 173",
			     "173",
			     new String[][] {}),

	// # 52
	new SipHeaderChecker("Content-Type:" +
			     " application/sdp",
			     "application/sdp",
			     new String[][] {}),

	// # 53
	new SipHeaderChecker("Content-type:" +
			     " application/sdp",
			     "application/sdp",
			     new String[][] {}),

	// # 54
	new SipHeaderChecker("Content-Type:" +
			     " application/sdp;gparam1=1",
			     "application/sdp",
			     new String[][] {{"gparam1", "1"}}),

	// # 55
	new SipHeaderChecker("c:" +
			     " text/html; charset=ISO-8859-4",
			     "text/html",
			     new String[][] {{"charset", "ISO-8859-4"}}),

	// # 56
	new SipHeaderChecker("CSeq:" +
			     " 4711 INVITE",
			     "4711 INVITE",
			     new String[][] {}),

	// # 57
	new SipHeaderChecker("cSeq:" +
			     " 4711 INVITE",
			     "4711 INVITE",
			     new String[][] {}),

	// # 58
	new SipHeaderChecker("Date:" +
			     " Mon, 10 Jan 2005 23:29:00 GMT",
			     "Mon, 10 Jan 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 59
	new SipHeaderChecker("Date:" +
			     " Tue, 08 Feb 2005 23:29:00 GMT",
			     "Tue, 08 Feb 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 60
	new SipHeaderChecker("Date:" +
			     " Wed, 09 Mar 2005 23:29:00 GMT",
			     "Wed, 09 Mar 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 61
	new SipHeaderChecker("Date:" +
			     " Thu, 07 Apr 2005 23:29:00 GMT",
			     "Thu, 07 Apr 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 62
	new SipHeaderChecker("Date:" +
			     " Fri, 13 May 2005 23:29:00 GMT",
			     "Fri, 13 May 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 63
	new SipHeaderChecker("Date:" +
			     " Sat, 11 Jun 2005 23:29:00 GMT",
			     "Sat, 11 Jun 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 64
	new SipHeaderChecker("Date:" +
			     " Sun, 10 Jul 2005 23:29:00 GMT",
			     "Sun, 10 Jul 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 65
	new SipHeaderChecker("Date:" +
			     " Sat, 13 Aug 2005 23:29:00 GMT",
			     "Sat, 13 Aug 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 66
	new SipHeaderChecker("Date:" +
			     " Sat, 10 Sep 2005 23:29:00 GMT",
			     "Sat, 10 Sep 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 67
	new SipHeaderChecker("Date:" +
			     " Sat, 15 Oct 2005 23:29:00 GMT",
			     "Sat, 15 Oct 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 68
	new SipHeaderChecker("Date:" +
			     " Sat, 12 Nov 2005 23:29:00 GMT",
			     "Sat, 12 Nov 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 69
	new SipHeaderChecker("Date:" +
			     " Sat, 10 Dec 2005 23:29:00 GMT",
			     "Sat, 10 Dec 2005 23:29:00 GMT",
			     new String[][] {}),

	// # 70
	new SipHeaderChecker("DatE:" +
			     " Sat, 13 Nov 2010 23:29:00 GMT",
			     "Sat, 13 Nov 2010 23:29:00 GMT",
			     new String[][] {}),

	// # 71
	new SipHeaderChecker("Error-Info:" +
			     " <sip:not-in-service-recording@atlanta.com>",
			     "<sip:not-in-service-recording@atlanta.com>",
			     new String[][] {}),

	// # 72
	new SipHeaderChecker("Error-InfO:" +
			     " <sip:not-in-service-recording@atlanta.com>",
			     "<sip:not-in-service-recording@atlanta.com>",
			     new String[][] {}),

	// # 73
	new SipHeaderChecker("Error-Info:" +
			     " <sip:not-in-service-recording@atlanta.com>;"
			     + "gparam2;gparam1=1",
			     "<sip:not-in-service-recording@atlanta.com>",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 74
	new SipHeaderChecker("Expires:" +
			     " 5",
			     "5",
			     new String[][] {}),

	// # 75
	new SipHeaderChecker("EXPIRES:" +
			     " 5",
			     "5",
			     new String[][] {}),

	// # 76
	new SipHeaderChecker("From:" +
			     " \"A. G. Bell\" <sip:agb@bell-telephone.com> ;"
			     + "tag=a48s",
			     "\"A. G. Bell\" <sip:agb@bell-telephone.com>",
			     new String[][] {{"tag", "a48s"}}),

	// # 77
	new SipHeaderChecker("FROM:" +
			     " \"A. G. Bell\" <sip:agb@bell-telephone.com> ;"
			     + "tag=a48s",
			     "\"A. G. Bell\" <sip:agb@bell-telephone.com>",
			     new String[][] {{"tag", "a48s"}}),

	// # 78
	new SipHeaderChecker("From:" +
			     " \"A. G. Bell\" <sip:agb@bell-telephone.com> ;"
			     + "tag=a48s;gparam2;gparam1=1",
			     "\"A. G. Bell\" <sip:agb@bell-telephone.com>",
			     new String[][] {{"tag", "a48s"},
					     {"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 79
	new SipHeaderChecker("f:" +
			     " Anonymous <sip:c8oqz84zk7z@privacy.org>;"
			     + "tag=hyh8",
			     "Anonymous <sip:c8oqz84zk7z@privacy.org>",
			     new String[][] {{"tag", "hyh8"}}),

	// # 80
	new SipHeaderChecker("F:" +
			     " Anonymous <sip:c8oqz84zk7z@privacy.org>;"
			     + "tag=hyh8",
			     "Anonymous <sip:c8oqz84zk7z@privacy.org>",
			     new String[][] {{"tag", "hyh8"}}),

	// # 81
	new SipHeaderChecker("In-Reply-To:" +
			     " 70710@saturn.bell-tel.com",
			     "70710@saturn.bell-tel.com",
			     new String[][] {}),

	// # 82
	new SipHeaderChecker("In-Reply-TO:" +
			     " 70710@saturn.bell-tel.com",
			     "70710@saturn.bell-tel.com",
			     new String[][] {}),

	// # 83
	new SipHeaderChecker("Max-Forwards:" +
			     " 6",
			     "6",
			     new String[][] {}),

	// # 84
	new SipHeaderChecker("Max-forwards:" +
			     " 6",
			     "6",
			     new String[][] {}),

	// # 85
	new SipHeaderChecker("Min-Expires:" +
			     " 60",
			     "60",
			     new String[][] {}),

	// # 86
	new SipHeaderChecker("Min-expires:" +
			     " 60",
			     "60",
			     new String[][] {}),

	// # 87
	new SipHeaderChecker("MIME-Version:" +
			     " 1.0",
			     "1.0",
			     new String[][] {}),

	// # 88
	new SipHeaderChecker("MIME-version:" +
			     " 1.0",
			     "1.0",
			     new String[][] {}),

	// # 89
	new SipHeaderChecker("Organization:" +
			     " Boxes by Bob",
			     "Boxes by Bob",
			     new String[][] {}),

	// # 90
	new SipHeaderChecker("organization:" +
			     " Boxes by Bob",
			     "Boxes by Bob",
			     new String[][] {}),

	// # 91
	new SipHeaderChecker("Priority:" +
			     " emergency",
			     "emergency",
			     new String[][] {}),

	// # 92
	new SipHeaderChecker("prioritY:" +
			     " emergency",
			     "emergency",
			     new String[][] {}),

	// # 93
	new SipHeaderChecker("Priority:" +
			     " token",
			     "token",
			     new String[][] {}),

	// # 94
	new SipHeaderChecker("Proxy-Authenticate:" +
			     " Digest realm=\"atlanta.com\","
			     + "domain=\"sip:ss1.carrier.com\","
			     + " qop=\"auth\","
			     + "nonce=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaque=\"\", stale=FALSE, algorithm=MD5",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"algorithm", "MD5"},
			      {"stale", "FALSE"},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"opaque", "\"\""},
			      {"domain", "\"sip:ss1.carrier.com\""},
			      {"qop", "\"auth\""}}),

	// # 95
	new SipHeaderChecker("Proxy-Authenticate:" +
			     " Digest realm=\"atlanta.com\","
			     + "domain=\"sip:ss1.carrier.com\", "
			     + "qop=\"auth\","
			     + "nonce=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaque=\"\", stale=TRUE, algorithm=MD5",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"algorithm", "MD5"},
			      {"stale", "TRUE"},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"opaque", "\"\""},
			      {"domain", "\"sip:ss1.carrier.com\""},
			      {"qop", "\"auth\""}}),

	// # 96
	new SipHeaderChecker("PRoxy-Authenticate:" +
			     " Digest realM=\"atlanta.com\","
			     + "domaiN=\"sip:ss1.carrier.com\", "
			     + "qoP=\"auth\","
			     + "noncE=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaquE=\"\", stale=FALSe, algorithm=mD5",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"algorithm", "mD5"},
			      {"stale", "FALSe"},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"opaque", "\"\""},
			      {"domain", "\"sip:ss1.carrier.com\""},
			      {"qop", "\"auth\""}}),

	// # 97
	new SipHeaderChecker("Proxy-Authenticate:" +
			     " Digest realm=\"atlanta.com\","
			     + "domain=\"sip:ss1.carrier.com\", "
			     + "qop=\"auth\","
			     + "nonce=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaque=\"\", stale=FALSE, algorithm=MD5,"
			     + "auth-param1=1,auth-param2=\"param2\"",
			     "Digest",
			     new String[][]
			     {{"qop", "\"auth\""},
			      {"domain", "\"sip:ss1.carrier.com\""},
			      {"opaque", "\"\""},
			      {"realm", "\"atlanta.com\""},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"algorithm", "MD5"},
			      {"stale", "FALSE"},
			      {"auth-param2", "\"param2\""},
			      {"auth-param1", "1"}}),

	// # 98
	new SipHeaderChecker("Proxy-Authorization:" +
			     " Digest username=\"Alice\", "
			     + "realm=\"atlanta.com\", "
			     + "nonce=\"c60f3082ee1212b402a21831ae\", "
			     + "response=\"245f23415f11432b3434341c022\"",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"response", "\"245f23415f11432b3434341c022\""},
			      {"nonce", "\"c60f3082ee1212b402a21831ae\""},
			      {"username", "\"Alice\""}}),

	// # 99
	new SipHeaderChecker("PRoxy-Authorization:" +
			     " Digest username=\"Alice\", "
			     + "realm=\"atlanta.com\", "
			     + "nonce=\"c60f3082ee1212b402a21831ae\", "
			     + "response=\"245f23415f11432b3434341c022\"",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"response", "\"245f23415f11432b3434341c022\""},
			      {"nonce", "\"c60f3082ee1212b402a21831ae\""},
			      {"username", "\"Alice\""}}),

	// # 100
	new SipHeaderChecker("Proxy-Authorization:" +
			     " Digest username=\"Alice\", "
			     + "realm=\"atlanta.com\", "
			     + "nonce=\"c60f3082ee1212b402a21831ae\", "
			     + "response=\"245f23415f11432b3434341c022\","
			     + "auth-param1=1,auth-param2=\"param2\"",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"response", "\"245f23415f11432b3434341c022\""},
			      {"nonce", "\"c60f3082ee1212b402a21831ae\""},
			      {"auth-param2", "\"param2\""},
			      {"auth-param1", "1"},
			      {"username", "\"Alice\""}}),

	// # 101
	new SipHeaderChecker("Proxy-Require:" +
			     " foo",
			     "foo",
			     new String[][] {}),

	// # 102
	new SipHeaderChecker("Proxy-RequirE:" +
			     " foo",
			     "foo",
			     new String[][] {}),

	// # 103
	new SipHeaderChecker("Record-Route:" +
			     " <sip:server10.biloxi.com;lr>",
			     "<sip:server10.biloxi.com;lr>",
			     new String[][] {}),

	// # 104
	new SipHeaderChecker("Record-route:" +
			     " <sip:server10.biloxi.com;lr>",
			     "<sip:server10.biloxi.com;lr>",
			     new String[][] {}),

	// # 105
	new SipHeaderChecker("Record-Route:" +
			     " <sip:server10.biloxi.com;lr>;gparam2;gparam1=1",
			     "<sip:server10.biloxi.com;lr>",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 106
	new SipHeaderChecker("Record-Route:" +
			     " <sip:server10.biloxi.com>;gparam2;gparam1=1",
			     "<sip:server10.biloxi.com>",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 107
	new SipHeaderChecker("Reply-To:" +
			     " Bob <sip:bob@biloxi.com>",
			     "Bob <sip:bob@biloxi.com>",
			     new String[][] {}),

	// # 108
	new SipHeaderChecker("Reply-to:" +
			     " Bob <sip:bob@biloxi.com>",
			     "Bob <sip:bob@biloxi.com>",
			     new String[][] {}),

	// # 109
	new SipHeaderChecker("Reply-To:" +
			     " Bob <sip:bob@biloxi.com>;gparam2;gparam1=1",
			     "Bob <sip:bob@biloxi.com>",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 110
	new SipHeaderChecker("Require:" +
			     " 100rel",
			     "100rel",
			     new String[][] {}),

	// # 111
	new SipHeaderChecker("require:" +
			     " 100rel",
			     "100rel",
			     new String[][] {}),

	// # 112
	new SipHeaderChecker("Retry-After:" +
			     " 18000;duration=3600",
			     "18000",
			     new String[][] {{"duration", "3600"}}),

	// # 113
	new SipHeaderChecker("Retry-After:" +
			     " 120 (I'm in a meeting)",
			     "120 (I'm in a meeting)",
			     new String[][] {}),

	// # 114
	new SipHeaderChecker("Retry-after:" +
			     " 18000;duration=3600",
			     "18000",
			     new String[][] {{"duration", "3600"}}),

	// # 115
	new SipHeaderChecker("Retry-After:" +
			     " 18000;duration=3600;gparam2;gparam1=1",
			     "18000",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"},
					     {"duration", "3600"}}),

	// # 116
	new SipHeaderChecker("Route:" +
			     " <sip:bigbox3.site3.atlanta.com;lr>",
			     "<sip:bigbox3.site3.atlanta.com;lr>",
			     new String[][] {}),

	// # 117
	new SipHeaderChecker("RoutE:" +
			     " <sip:bigbox3.site3.atlanta.com;lr>",
			     "<sip:bigbox3.site3.atlanta.com;lr>",
			     new String[][] {}),

	// # 118
	new SipHeaderChecker("Route:" +
			     " <sip:bigbox3.site3.atlanta.com;lr>;"
			     + "gparam2;gparam1=1",
			     "<sip:bigbox3.site3.atlanta.com;lr>",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 119
	new SipHeaderChecker("Server:" +
			     " HomeServer v2",
			     "HomeServer v2",
			     new String[][] {}),

	// # 120
	new SipHeaderChecker("server:" +
			     " HomeServer v2",
			     "HomeServer v2",
			     new String[][] {}),

	// # 121
	new SipHeaderChecker("Subject:" +
			     " Need more boxes",
			     "Need more boxes",
			     new String[][] {}),

	// # 122
	new SipHeaderChecker("SUbject:" +
			     " Need more boxes",
			     "Need more boxes",
			     new String[][] {}),

	// # 123
	new SipHeaderChecker("s:" +
			     " Tech Support",
			     "Tech Support",
			     new String[][] {}),

	// # 124
	new SipHeaderChecker("S:" +
			     " Tech Support",
			     "Tech Support",
			     new String[][] {}),

	// # 125
	new SipHeaderChecker("Supported:" +
			     " 100rel",
			     "100rel",
			     new String[][] {}),

	// # 126
	new SipHeaderChecker("SupporteD:" +
			     " 100rel",
			     "100rel",
			     new String[][] {}),

	// # 127
	new SipHeaderChecker("Timestamp:" +
			     " 54",
			     "54",
			     new String[][] {}),

	// # 128
	new SipHeaderChecker("TimestamP:" +
			     " 54",
			     "54",
			     new String[][] {}),

	// # 129
	new SipHeaderChecker("To:" +
			     " The Operator <sip:operator@cs.columbia.edu>;"
			     + "tag=287447",
			     "The Operator <sip:operator@cs.columbia.edu>",
			     new String[][] {{"tag", "287447"}}),

	// # 130
	new SipHeaderChecker("TO:" +
			     " The Operator <sip:operator@cs.columbia.edu>;"
			     + "tag=287447",
			     "The Operator <sip:operator@cs.columbia.edu>",
			     new String[][] {{"tag", "287447"}}),

	// # 131
	new SipHeaderChecker("To:" +
			     " The Operator <sip:operator@cs.columbia.edu>;"
			     + "gparam2;gparam1=1",
			     "The Operator <sip:operator@cs.columbia.edu>",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 132
	new SipHeaderChecker("t:" +
			     " sip:+12125551212@server.phone2net.com",
			     "sip:+12125551212@server.phone2net.com",
			     new String[][] {}),

	// # 133
	new SipHeaderChecker("T:" +
			     " sip:+12125551212@server.phone2net.com",
			     "sip:+12125551212@server.phone2net.com",
			     new String[][] {}),

	// # 134
	new SipHeaderChecker("t:" +
			     " sip:+12125551212@server.phone2net.com;"
			     + "gparam2;gparam1=1",
			     "sip:+12125551212@server.phone2net.com",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 135
	new SipHeaderChecker("Unsupported:" +
			     " foo",
			     "foo",
			     new String[][] {}),

	// # 136
	new SipHeaderChecker("UnsupporteD:" +
			     " foo",
			     "foo",
			     new String[][] {}),

	// # 137
	new SipHeaderChecker("User-Agent:" +
			     " Softphone Beta1.5",
			     "Softphone Beta1.5",
			     new String[][] {}),

	// # 138
	new SipHeaderChecker("User-agent:" +
			     " Softphone Beta1.5",
			     "Softphone Beta1.5",
			     new String[][] {}),

	// # 139
	new SipHeaderChecker("Via:" +
			     " SIP/2.0/UDP erlang.bell-telephone.com:5060;"
			     + "branch=z9hG4bK87asdks7",
			     "SIP/2.0/UDP erlang.bell-telephone.com:5060",
			     new String[][] {{"branch", "z9hG4bK87asdks7"}}),

	// # 140
	new SipHeaderChecker("VIA:" +
			     " SIP/2.0/UDP erlang.bell-telephone.com:5060;"
			     + "branch=z9hG4bK87asdks7",
			     "SIP/2.0/UDP erlang.bell-telephone.com:5060",
			     new String[][] {{"branch", "z9hG4bK87asdks7"}}),

	// # 141
	new SipHeaderChecker("v:" +
			     " SIP/2.0/UDP 192.0.2.1:5060 ;"
			     + "received=192.0.2.207;branch=z9hG4bK77asjd",
			     "SIP/2.0/UDP 192.0.2.1:5060",
			     new String[][] {{"branch", "z9hG4bK77asjd"},
					     {"received", "192.0.2.207"}}),

	// # 142
	new SipHeaderChecker("V:" +
			     " SIP/2.0/UDP 192.0.2.1:5060 ;"
			     + "received=192.0.2.207;branch=z9hG4bK77asjd",
			     "SIP/2.0/UDP 192.0.2.1:5060",
			     new String[][] {{"branch", "z9hG4bK77asjd"},
					     {"received", "192.0.2.207"}}),

	// # 143
	new SipHeaderChecker("Via:" +
			     " SIP / 2.0 / UDP first.example.com: 4000;ttl=16;"
			     + "maddr=224.2.0.1 ;branch=z9hG4bKa7c6a8dlze.1",
			     "SIP/2.0/UDP first.example.com:4000",
			     new String[][]
			     {{"ttl", "16"},
			      {"maddr", "224.2.0.1"},
			      {"branch", "z9hG4bKa7c6a8dlze.1"}}),

	// # 144
	new SipHeaderChecker("Via:" +
			     " SIP/2.0/UDP erlang.bell-telephone.com:5060;"
			     + "gparam2;gparam1=1",
			     "SIP/2.0/UDP erlang.bell-telephone.com:5060",
			     new String[][] {{"gparam2", ""},
					     {"gparam1", "1"}}),

	// # 145
	new SipHeaderChecker("Warning:" +
			     " 307 isi.edu \"Session parameter 'foo' "
			     + "not understood\"",
			     "307 isi.edu \"Session parameter 'foo' "
			     + "not understood\"",
			     new String[][] {}),

	// # 146
	new SipHeaderChecker("WarninG:" +
			     " 307 isi.edu \"Session parameter 'foo' "
			     + "not understood\"",
			     "307 isi.edu \"Session parameter 'foo' "
			     + "not understood\"",
			     new String[][] {}),

	// # 147
	new SipHeaderChecker("WWW-Authenticate:" +
			     " Digest realm=\"atlanta.com\","
			     + "domain=\"sip:boxesbybob.com\", "
			     + "qop=\"auth\","
			     + "nonce=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaque=\"\", stale=FALSE, algorithm=MD5",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"algorithm", "MD5"},
			      {"stale", "FALSE"},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"opaque", "\"\""},
			      {"domain", "\"sip:boxesbybob.com\""},
			      {"qop", "\"auth\""}}),

	// # 148
	new SipHeaderChecker("WWW-Authenticate:" +
			     "Digest realm=\"atlanta.com\","
			     + "domain=\"sip:boxesbybob.com\", "
			     + "qop=\"auth\","
			     + "nonce=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaque=\"\", stale=FALSE, algorithm=MD5",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"algorithm", "MD5"},
			      {"stale", "FALSE"},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"opaque", "\"\""},
			      {"domain", "\"sip:boxesbybob.com\""},
			      {"qop", "\"auth\""}}),

	// # 149
	new SipHeaderChecker("WWW-Authenticate:" +
			     "   Digest   realm=\"atlanta.com\"  ,   "
			     + "domain=\"sip:boxesbybob.com\"  ,   "
			     + "qop=\"auth\" ,    "
			     + "nonce=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaque=\"\",    stale=FALSE,  algorithm=MD5",
			     "Digest",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"algorithm", "MD5"},
			      {"stale", "FALSE"},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"opaque", "\"\""},
			      {"domain", "\"sip:boxesbybob.com\""},
			      {"qop", "\"auth\""}}),

	// # 150
	new SipHeaderChecker("WWW-AuthenticatE:" +
			     " DIGEST REALM=\"atlanta.com\""
			     + ",DOMAIn=\"sip:boxesbybob.com\", "
			     + "QoP=\"auth\","
			     + "NONCE=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "OPAQUE=\"\", stale=true, algorithm=md5",
			     "DIGEST",
			     new String[][]
			     {{"realm", "\"atlanta.com\""},
			      {"algorithm", "md5"},
			      {"stale", "true"},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"opaque", "\"\""},
			      {"domain", "\"sip:boxesbybob.com\""},
			      {"qop", "\"auth\""}}),

	// # 151
	new SipHeaderChecker("WWW-Authenticate:" +
			     " Digest realm=\"atlanta.com\","
			     + "domain=\"sip:boxesbybob.com\", "
			     + "qop=\"auth\","
			     + "nonce=\"f84f1cec41e6cbe5aea9c8e88d359\","
			     + "opaque=\"\", stale=FALSE, "
			     + "algorithm=MD5,auth-param1=1,"
			     + "auth-param2=\"param2\"",
			     "Digest",
			     new String[][]
			     {{"qop", "\"auth\""},
			      {"domain", "\"sip:boxesbybob.com\""},
			      {"opaque", "\"\""},
			      {"realm", "\"atlanta.com\""},
			      {"nonce", "\"f84f1cec41e6cbe5aea9c8e88d359\""},
			      {"algorithm", "MD5"},
			      {"stale", "FALSE"},
			      {"auth-param2", "\"param2\""},
			      {"auth-param1", "1"}}),

	// # 152
	new SipHeaderChecker("xxx:" +
			     " unknown field type; "
			     + "some_string_parameter=\"atlanta.com\"; "
			     + "some_other_parameter=other_parameter_value; "
			     + "empty_parameter",
			     "unknown field type",
			     new String[][]
			     {{"some_other_parameter", "other_parameter_value"},
			      {"some_string_parameter", "\"atlanta.com\""},
			      {"empty_parameter", ""}}),

	// # 153
	new SipHeaderChecker("xxx:" +
			     "\tunknown\tfield\t\ttype;\t"
			     + "some_string_parameter=\"atlanta.com\";\t"
			     + "some_other_parameter=other_parameter_value;\t"
			     + "  \tempty_parameter",
			     "unknown field type",
			     new String[][]
			     {{"some_other_parameter", "other_parameter_value"},
			      {"some_string_parameter", "\"atlanta.com\""},
			      {"empty_parameter", ""}}),

	// # 154
	new SipHeaderChecker("xxx:" +
			     " unknown field type;"
			     + "some_string_parameter=\"atlanta.com\";"
			     + "some_other_parameter=other_parameter_value",
			     "unknown field type",
			     new String[][]
			     {{"some_other_parameter", "other_parameter_value"},
			      {"some_string_parameter", "\"atlanta.com\""}}),

	// # 155
	new SipHeaderChecker("xxx:" +
			     " unknown field type   ;"
			     + "some_string_parameter=\"atlanta.com\";"
			     + "some_other_parameter=other_parameter_value",
			     "unknown field type",
			     new String[][]
			     {{"some_other_parameter", "other_parameter_value"},
			      {"some_string_parameter", "\"atlanta.com\""}}),

	// # 156
	new SipHeaderChecker("xxx:" +
			     " unknown field type   ;    "
			     + "some_string_parameter=    \"atlanta.com\";    "
			     + "some_other_parameter =    "
			     + "other_parameter_value",
			     "unknown field type",
			     new String[][]
			     {{"some_other_parameter", "other_parameter_value"},
			      {"some_string_parameter", "\"atlanta.com\""}}),

	// # 157
	new SipHeaderChecker("xxx:" +
			     " unknown field type;"
			     + "some_string_parameter   =\"atlanta.com\"   ;"
			     + "some_other_parameter  =  other_parameter_value",
			     "unknown field type",
			     new String[][]
			     {{"some_other_parameter", "other_parameter_value"},
			      {"some_string_parameter", "\"atlanta.com\""}}),

	// # 158
	new SipHeaderChecker("xxx:" +
			     " unknown field type",
			     "unknown field type",
			     new String[][] {}),

	// # 159
	new SipHeaderChecker("xxx:" +
			     " unknown field type; "
			     + "some_string_parameter=\"atlanta.com\"",
			     "unknown field type",
			     new String[][]
			     {{"some_string_parameter",
			       "\"atlanta.com\""}}),

	// # 160
	new SipHeaderChecker("xxx:" +
			     " unknown field type; empty_parameter    ",
			     "unknown field type",
			     new String[][] {{"empty_parameter", ""}}),

	// # 161
	new SipHeaderChecker("xxx:" +
			     "unknown field type;empty_parameter",
			     "unknown field type",
			     new String[][] {{"empty_parameter", ""}}),

	// # 162
	new SipHeaderChecker("1-.!%*_+`'~:" +
			     "unknown field type;empty_parameter",
			     "unknown field type",
			     new String[][] {{"empty_parameter", ""}}),

    // # 163
    new SipHeaderChecker("Accept-Contact:" +
                 " *;type=\"application/vnd.company.x-game\"",
                 "*",
                 new String[][]
                 {{"type", "\"application/vnd.company.x-game\""}}),

    // # 164
    new SipHeaderChecker("a:" +
                 " *;type=\"application/vnd.company.x-game\"",
                 "*",
                 new String[][]
                 {{"type", "\"application/vnd.company.x-game\""}})
    };

    /** Current name being processed. */
    String name;
    /** Current value being processed. */
    String value;
    /** Parsed header values. */
    String parsedHeaderValue;
    /** Extracted parameters. */
    String[][] parameters;
    /** Parsed name. */
    String parsedName;

    /**
     * Constructor for basic test case.
     * @param header the test input string
     * @param parsedHeaderValue expected value string
     * @param parameters expected paramaters from parsed header
     */
    public SipHeaderChecker(String header, String parsedHeaderValue,
			    String[][] parameters) {
        int i = header.indexOf(':');
        name = header.substring(0, i);
        value = header.substring(i + 1);
        this.parsedHeaderValue = parsedHeaderValue;
        this.parameters = parameters;
        parsedName = toLongForm(name);
    }
    /**
     * Checks if test passed.
     * @return true if test passes
     */
    boolean check() {

	// DEBUG: System.out.println("Header: " + name + ":" + value);

        SipHeader h;
        try {
            h = new SipHeader(name, value);
        } catch (IllegalArgumentException e) {
            System.out.println("Error parsing the header:" + e
			       + " name=" + name
			       + " value=" + value);
            e.printStackTrace();
            return false;
        }

        String strValue = h.toString();

        boolean ok = true;

        ok &= checkName(h);
        ok &= checkValue(h);
        ok &= checkParameters(h);

	// DEBUG: System.out.println("Checking getHeaderValue result");

        try {
            h = new SipHeader(h.getName(), h.getHeaderValue());
        } catch (IllegalArgumentException e) {
            System.out.println("Error parsing values returned by parser " + e
			       + " name=" + h.getName()
			       + " value=" + h.getHeaderValue());
            e.printStackTrace();
            return false;
        }

        ok &= checkName(h);
        ok &= checkValue(h);
        ok &= checkParameters(h);

        // DEBUG: System.out.println("Checking toString result");

        int i = strValue.indexOf(':');

        try {
            h = new SipHeader(strValue.substring(0, i).trim(),
                              strValue.substring(i + 1).trim());
        } catch (IllegalArgumentException e) {
            System.out.println("Error parsing toString result " + e
			       + " name=" + strValue.substring(0, i).trim()
			       + " value=" + strValue.substring(i + 1).trim());
            e.printStackTrace();
            return false;
        }

        ok &= checkName(h);
        ok &= checkValue(h);
        ok &= checkParameters(h);

        return ok;
    }
    /**
     * Checks for valid name in hedaer.
     * @param h the header to be validated
     * @return true if name is valid
     */
    boolean checkName(SipHeader h) {

        String pname = h.getName();

        if (! (parsedName.equalsIgnoreCase(pname) ||
               toShortForm(parsedName).equalsIgnoreCase(pname))) {
            System.out.println("Invalid name " + h.getName());
            return false;
        }
        return true;
    }
    /**
     * Checks for valid value in header.
     * @param h the header to be validated
     * @return true if value is valid
     */
    boolean checkValue(SipHeader h) {

        String pValue = h.getValue();

        if ((pValue == null || parsedHeaderValue == null) &&
	    pValue != parsedHeaderValue) {
            System.out.println("getValue(): Invalid value '" + h.getValue() +
			       "', must be '" + parsedHeaderValue + "'");
            return false;
        }

        pValue = fixSpaces(pValue);
        String name = toShortForm(h.getName());

        // In case of "t:"/"T:" header, parsedHeaderValue is
        // a SIP URI, so add "<"/">" around the value.
        if (name.equalsIgnoreCase("t")) {
            if (!pValue.startsWith("<")) {
                pValue = "<" + pValue + ">";
            }

            if (!parsedHeaderValue.startsWith("<")) {
                parsedHeaderValue = "<" + parsedHeaderValue + ">";
            }
        }

        boolean isSensitive = name.equals("i") ||
	    name.equals("cseq") ||
	    name.equals("date");

        if (!(isSensitive ?
	    pValue.equals(parsedHeaderValue) :
	    pValue.equalsIgnoreCase(parsedHeaderValue))) {
            System.out.println("Invalid value '" + h.getValue() +
			       "', must be '" + parsedHeaderValue + "'");
            return false;
        }
        return true;
    }
    /**
     * Checks for valid parameters in header.
     * @param h the header to be validated
     * @return true if parameters are valid
     */
    boolean checkParameters(SipHeader h) {

        String[] paramNames = h.getParameterNames();

        if (parameters.length == 0) {
            if (paramNames != null) {
                System.out.println("getParameterNames must return null");
                return false;
            }
            return true;
        }

        if (paramNames == null) {
            System.out.println("getParameterNames returns null");
            return false;
        }
	if (parameters.length != paramNames.length) {
	    System.out.println("getParameterNames returns invalid "+
			       "number of parameters: " + paramNames.length +
			       ", must be " + parameters.length);
	    return false;
	}

        for (int i = 0; i < parameters.length; i++) {

            String paramName = parameters[i][0];
            String paramValue = parameters[i][1];

            boolean found = false;
            for (int j = 0; j < paramNames.length; j++) {
                if (paramName.equalsIgnoreCase(paramNames[j])) {
                    found = true;
                    break;
                }
            }

            if (! found) {
                System.out.println(paramName +
				   "not found in results of getParameterNames");
                return false;
            }

            String v = h.getParameter(paramName);

            if (paramValue == null) {
                if (v != null) {
                    System.out.println("null expected: " + paramName);
                    return false;
                }
            } else {
                if (v == null) {
                    System.out.println("parameter " + paramName +
				       " not found");
                    return false;
                }

                v = fixSpaces(v);

                boolean isSensitive = v.startsWith("\"");

                if (! (isSensitive ? v.equals(paramValue) :
		       v.equalsIgnoreCase(paramValue))) {
                    System.out.println("Invalid parameter value: " +
				       v + ".");
                    return false;
                }
            }
        }
        return true;
    }
    /**
     * Inserts white space into string.
     * @param src input string to be processed
     * @return expanded whitespace string
     */
    public static String fixSpaces(String src) {

        if (src == null) {
            return null;
        }

        StringBuffer dst = new StringBuffer();

        boolean quoted = false;
        boolean prevBlank = false;
        for (int i = 0; i < src.length(); i++) {
            char c = src.charAt(i);

            if (c == '\"') {
                quoted = ! quoted;
            }

            if (c == '\t') {
                c = ' ';
            }
            boolean isBlank = c == ' ';
            if (quoted || ! (prevBlank && isBlank)) {
                dst.append(c);
            }
            prevBlank = isBlank;
        }
        return dst.toString();
    }
    /**
     * Extracts quoted strings.
     * @param src input string to be processed
     * @return quoted string
     */
    static String fixString(String src) {
        if (src == null) {
            return null;
        }
        if (src.indexOf('\"') == -1) {
            return src;
        }
        StringBuffer s = new StringBuffer();
        for (int i = 0; i < src.length(); i++) {
            char c = src.charAt(i);
            if (c == '\"') {
                s.append('\\');
            }
            if (c == '\t') {
                s.append('\\');
                c = 't';
            }
            s.append(c);
        }
        return s.toString();
    }

    /**
     * Runs the set of tests.
     * @return number of failed tests
     */
    public static int runTests() {
        int failed = 0, passed = 0;

        for (int i = 0; i < testCases.length; i++) {
            if (testCases[i].check()) {
                passed++;
            } else {
		System.out.println("\n*** " + i + "  "
				   + testCases[i].name + ":"
				   + testCases[i].value);
                failed++;
            }
        }
	if (failed > 0) {
	    System.out.println("\n*** RESULT: "
			       + passed + " passed, "
			       + failed + " failed; "
			       + "total = " + (passed + failed) + "\n");
	}
        return failed;
    }
}

/**
 * TestAllSipHeader class is a wrapper for SipHeaderChecker
 */
public class TestAllSipHeader extends TestCase {
    /**
     * Body of the test 1.
     *
     * Calls SipHeaderChecker.runTests().
     */
    void Test1() {
        int failed = SipHeaderChecker.runTests();

        assertTrue("Number of failed tests: " + failed, failed == 0);
    }

    /**
     * Run the test
     */
    public void runTests() {
        declare("SipHeader test");
        Test1();
    }
}
