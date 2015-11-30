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
package gov.nist.javax.sdp;

import gov.nist.javax.sdp.fields.*;
import java.util.*;
import gov.nist.core.*;

/** 
 * Field implementation of Media Description interface.
 * @version JSR141-PUBLIC-REVIEW (subject to change).
 *
 *
 * <a href="{@docRoot}/uncopyright.html">This code is in the public domain.</a>
 */
public class MediaDescriptionImpl 
{
    /** Media type field. */
    protected MediaField mediaField;
    /** Extra information field. */
    protected InformationField informationField;
    /** Current connection. */
    protected ConnectionField connectionField;
    /** Vector of bandwidths. */
    protected Vector bandwidthFields;
    /** Key field. */
    protected KeyField keyField;
    /** Connection attributes. */
    protected Vector attributeFields;

    /**
     * Encodes to a canonical form.
     * @return encoded string of object contents
     * @since v1.0
     */
    public String encode() {
	StringBuffer retval = new StringBuffer();

	if (mediaField != null) 
	    retval.append(mediaField.encode());

	if (informationField != null) 
	    retval.append(informationField.encode());

	if (connectionField != null) 
	    retval.append(connectionField.encode());

	if (bandwidthFields != null) {
	    for (int i = 0; i < bandwidthFields.size(); i++) {
		BandwidthField bandwidthField = 
		    (BandwidthField) bandwidthFields.elementAt(i);
		retval.append(bandwidthField.encode());
	    }
	}

	if (keyField != null) 
	    retval.append(keyField.encode());

	if (attributeFields != null) {
	    for (int i = 0; i < attributeFields.size(); i++) 
		retval.append(((SDPField)attributeFields.elementAt(i)).
			      encode());
	}

	return retval.toString();
    }
    /** 
     * Returns encoded text.
     * @return encoded string of object contents
     */
    public String toString() {
	return this.encode();
    }
 
    /** Default constructor. */
    public MediaDescriptionImpl() { 
	this.bandwidthFields = new Vector(); 
	this.attributeFields = new Vector();
    }

    /** 
     * Gets the media field
     * @return the media field
     */
    public MediaField getMediaField() {
	return mediaField;
    } 

    /**
     * Gets the information field.
     * @return the information field
     */
    public InformationField getInformationField() {
	return informationField;
    } 

    /** 
     * Gets the connection filed
     * @return the connection firld
     */
    public ConnectionField getConnectionField() {
	return connectionField;
    } 

    /**
     * Gets the key field.
     * @return the key field
     */
    public KeyField getKeyField() {
	return keyField;
    } 

    /** 
     * Gets the attributes filed.
     * @return the vector of attributes
     */
    public Vector getAttributeFields() {
	return attributeFields;
    } 

    /**
     * Sets the media field member.
     * @param m the new media field
     */
    public void setMediaField(MediaField m) {
	mediaField = m;
    } 

    /**
     * Sets the information field member.
     * @param i the new information filed
     */
    public void setInformationField(InformationField i) {
	informationField = i;
    } 

    /**
     * Sets the connection field member.
     * @param c the new connection field 
     */
    public void setConnectionField(ConnectionField c) {
	connectionField = c;
    } 

    /**
     * Sets the bandwidth field member.
     * @param b the new bandwidth field 
     */
    public void addBandwidthField(BandwidthField b) {
	bandwidthFields.addElement(b);
    } 

    /**
     * Sets the key field member.
     * @param k the new key field 
     */
    public void setKeyField(KeyField k) {
	keyField = k;
    } 

    /**
     * Sets the attribute fields member.
     * @param a the new vector of attributes 
     */
    public void setAttributeFields(Vector a) {
	attributeFields = a;
    } 
 
 
 
 
    /**
     * Returns the Media field of the description.
     * @return the Media field of the description.
     */
    public MediaField getMedia() {
	return mediaField;
 
    }
 
    /** 
     * Adds a new attirbute to the list.
     * @param af the attribute field to be processed
     */
    protected void addAttribute(AttributeField af) { 
	this.attributeFields.addElement(af);
    } 
 
    /**
     * Checks if named attribute exists.
     * @param name the attribute to check
     * @return true is attribute is found 
     */
    protected boolean hasAttribute(String name) {
	for (int i = 0; i < this.attributeFields.size(); i++) {
	    AttributeField af = 
		(AttributeField) this.attributeFields.elementAt(i);
	    if (af.getAttribute().getName().equals(name))
		return true;
	}
	return false;
    }
 
    /**
     * Sets the Media field of the description.
     * @param media to set
     * @throws SdpException if the media field is null
     */
    public void setMedia(MediaField media)
	throws SdpException {
	if (media == null)
	    throw new SdpException("The media is null");
	mediaField = media;
    }
 
    /**
     * Returns value of the info field (i=) of this object.
     * @return value of the info field (i=) of this object.
     */
    public InformationField getInfo() {
	return informationField;
    }
 
    /**
     * Sets the i= field of this object.
     * @param i to set
     * @throws SdpException if the info is null
     */
    public void setInfo(InformationField i)
	throws SdpException {
	if (i == null)
	    throw new SdpException("The info is null");
	this.informationField = i;
    }
 
    /**
     * Returns the connection information associated with this object. This may
     * be null for SessionDescriptions if all Media
     * objects have a connection object and may be null for Media
     * objects if the corresponding session connection is non-null.
     * @return connection
     */
    public ConnectionField getConnection() {
 
	return connectionField;
 
    }
 
    /**
     * Sets the connection data for this entity.
     * @param conn to set
     * @throws SdpException if the connexion is null
     */
    public void setConnection(ConnectionField conn)
	throws SdpException {
	if (conn == null)
	    throw new SdpException("The conn is null");
	connectionField = conn;
 
    }
 
    /**
     * Returns the Bandwidth of the specified type.
     * @param create type of the Bandwidth to return
     * @return the Bandwidth or null if undefined
     */
 
    public Vector getBandwidths(boolean create) { 
	return bandwidthFields;
    }
 
 
    /**
     * Sets the value of the Bandwidth with the specified type.
     * @param bandwidths type of the Bandwidth object whose value is requested
     * @throws SdpException if vector is null
     */
    public void setBandwidths(Vector bandwidths)
	throws SdpException {
	if (bandwidths == null) 
	    throw new SdpException("The vector bandwidths is null");
	this.bandwidthFields = bandwidths;
    }
 
    /**
     * Returns the integer value of the specified bandwidth name.
     * @param name the name of the bandwidth type.
     * @throws SdpParseException if a parsing error occurs
     * @return the value of the named bandwidth
     */
    public int getBandwidth(String name)
	throws SdpParseException {
 
	if (name == null)
	    throw new NullPointerException("null parameter");
	if (bandwidthFields == null)
	    return -1;
	else {
	    for (int i = 0; i < bandwidthFields.size(); i++) {
		BandwidthField bandwidthField = (BandwidthField) 
		    bandwidthFields.elementAt(i);
		String type = bandwidthField.getBwtype();
		if (type != null && 
		    type.equals(name)) 
		    return bandwidthField.getBandwidth();
	    }
	    return -1;
	}
    }
 
    /**
     * Sets the value of the specified bandwidth type.
     * @param name the name of the bandwidth type.
     * @param value the value of the named bandwidth type.
     * @throws SdpException if the name is null
     */
    public void setBandwidth(String name,
			     int value)
	throws SdpException {
	if (name == null)
	    throw new SdpException("The name is null");
	else {
	    for (int i = 0; i < bandwidthFields.size(); i++) {
		BandwidthField bandwidthField = (BandwidthField) 
		    bandwidthFields.elementAt(i);
		String type = bandwidthField.getBwtype();
		if (type != null && 
		    type.equals(name)) 
		    bandwidthField.setBandwidth(value);
	    }
 
	}
    }
 
    /**
     * Removes the specified bandwidth type.
     * @param name the name of the bandwidth type.
     */
    public void removeBandwidth(String name) {
	if (name == null) {
	    throw new NullPointerException("null bandwidth type");
	} else {
	    int i = 0;
	    for (i = 0; i < bandwidthFields.size(); i++) {
		BandwidthField bandwidthField = (BandwidthField) 
		    bandwidthFields.elementAt(i);
		String type = bandwidthField.getBwtype();
		if (type != null && 
		    type.equals(name))
		    break;
 
	    }
	    if (i < bandwidthFields.size()) 
		bandwidthFields.removeElementAt(i);
	}
    }
 
    /**
     * Returns the key data.
     * @return the key data.
     */
    public KeyField getKey() {
	return keyField;
    }
 
    /** 
     * Sets encryption key information. This consists of a method and an 
     * encryption key included inline.
     * @param key the encryption key data; depending on method may be null
     * @throws SdpException if the key is null
     */
    public void setKey(KeyField key)
	throws SdpException {
	if (key == null)
	    throw new SdpException("The key is null");
	setKeyField(key);
    }
 
    /**
     * Returns the set of attributes for this Description as a 
     * Vector of Attribute objects in the order they were parsed.
     * @param create specifies whether to return null or a 
     * new empty Vector in case
     * no attributes exists for this Description
     * @return attributes for this Description
     */
    public Vector getAttributes(boolean create) {
	return attributeFields;
    }
 
    /**
     * Adds the specified Attribute to this Description object.
     * @param attributes the attribute to add
     * @throws SdpException if the attributes is null
     */
    public void setAttributes(Vector attributes)
	throws SdpException {
	this.attributeFields = attributes;
    }
 
    /**
     * Returns the value of the specified attribute.
     * @param name the name of the attribute.
     * @throws SdpParseException
     * @return the value of the named attribute
     */
    public String getAttribute(String name)
	throws SdpParseException {
	if (name != null) {
	    for (int i = 0; i < this.attributeFields.size(); i++) {
		AttributeField af = (AttributeField)
		    this.attributeFields.elementAt(i);
		if (name.equals(af.getAttribute().getName())) 
		    return (String) af.getAttribute().getValue();
	    }
	    return null; 
	} else throw new NullPointerException("null arg!");
    }
 
    /**
     * Sets the value of the specified attribute
     * @param name the name of the attribute.
     * @param value the value of the named attribute.
     * @throws SdpException if the parameters are null
     */
    public void setAttribute(String name,
			     String value)
	throws SdpException {
	if (name == null)
	    throw new SdpException("The parameters are null");
	else {
 
	    int i = 0;
	    for (i = 0; i < this.attributeFields.size(); i++) {
		AttributeField af = (AttributeField) 
		    this.attributeFields.elementAt(i);
		if (af.getAttribute().getName().equals(name)) {
		    NameValue nv = af.getAttribute();
		    nv.setValue(value);
		    break;
		}
 
	    }
 
	    if (i == this.attributeFields.size()) {
		AttributeField af = new AttributeField();
		NameValue nv = new NameValue(name, value);
		af.setAttribute(nv);
		this.attributeFields.addElement(af);
	    }
 
	}
    }
 
    /**
     * Removes the attribute specified by the value parameter.
     * @param name the name of the attribute.
     */
    public void removeAttribute(String name) {
	if (name == null)
	    throw new NullPointerException("null arg!");
	if (name != null) {
	    int i = 0;
	    for (i = 0; i < this.attributeFields.size(); i++) {
		AttributeField af = (AttributeField) 
		    this.attributeFields.elementAt(i);
		if (af.getAttribute().getName().equals(name)) break;
	    }
	    if (i < attributeFields.size())
		attributeFields.removeElementAt(i);
	}
    }
 
    /**
     * Returns a Vector containing a string indicating the MIME type
     * for each of the codecs in this description.
     * <p>
     * A MIME value is computed for each codec in the media description.
     * <p>
     * The MIME type is computed in the following fashion:
     * The type is the mediaType from the media field.
     * The subType is determined by the protocol.
     * <p>
     * The result is computed as the string of the form:
     * <pre>
     * type + '/' + subType
     * </pre>
     * The subType portion is computed in the following fashion.
     * RTP/AVP
     * the subType is returned as the codec name. This will either
     * be extracted from the rtpmap attribute or computed.
     * other
     * the protocol is returned as the subType.
     * <p>
     * If the protocol is RTP/AVP and the rtpmap attribute for a
     * codec is absent, then the codec name will be computed in the
     * following fashion.
     * String indexed in table SdpConstants.avpTypeNames
     * if the value is an int greater than or equal to 0 and less than 
     * AVP_DEFINED_STATIC_MAX, and has been assigned a
     * value.
     * SdpConstant.RESERVED
     * if the value is an int greater than or equal to 0 and less than 
     * AVP_DEFINED_STATIC_MAX, and has not been
     * assigned a value.
     * SdpConstant.UNASSIGNED
     * An int greater than or equal to AVP_DEFINED_STATIC_MAX
     * and less than
     * AVP_DYNAMIC_MIN - currently
     * unassigned.
     * SdpConstant.DYNAMIC
     * Any int less than 0 or greater than or equal to AVP_DYNAMIC_MIN
     * @throws SdpException if there is a problem extracting the parameters.
     * @return a Vector containing a string indicating the MIME type for 
     * each of the codecs in this description
     */
    public Vector getMimeTypes()
	throws SdpException {
	MediaField mediaField = (MediaField)getMedia();
	String type = mediaField.getMediaType();
	String protocol = mediaField.getProtocol();
	Vector formats = mediaField.getMediaFormats(false);
 
	Vector v = new Vector();
	for (int i = 0; i < formats.size(); i++) {
	    String result = null;
	    if (protocol.equals("RTP/AVP")) {
		if (getAttribute(SdpConstants.RTPMAP) != null)
		    result = type + "/"+protocol;
		else {
 
		}
	    } else
		result = type + "/" + protocol;
	    v.addElement(result);
	}
	return v;
    }
 
    /**
     * Returns a Vector containing a string of parameters for each of
     * the codecs in this description.
     * <p>
     * A parameter string is computed for each codec.
     * <p>
     * The parameter string is computed in the following fashion.
     * <p>
     * The rate is extracted from the rtpmap or static data.
     * <p>
     * The number of channels is extracted from the rtpmap or static data.
     * <p>
     * The ptime is extracted from the ptime attribute.
     * <p>
     * The maxptime is extracted from the maxptime attribute.
     * <p>
     * Any additional parameters are extracted from the ftmp attribute.
     * @throws SdpException if there is a problem extracting the parameters.
     * @return a Vector containing a string of parameters for each of the 
     * codecs in this description.
     */
    public Vector getMimeParameters()
	throws SdpException {
	String rate = getAttribute("rate");
	String ptime = getAttribute("ptime");
	String maxptime = getAttribute("maxptime");
	String ftmp = getAttribute("ftmp");
	Vector result = new Vector();
	result.addElement(rate);
	result.addElement(ptime);
	result.addElement(maxptime);
	result.addElement(ftmp);
	return result;
    }
 
    /** 
     * Adds dynamic media types to the description.
     * @param payloadNames a Vector of String - each one the name of 
     * a dynamic payload to be added (usually an integer larger
     * than SdpConstants.AVP_DYNAMIC_MIN).
     * @param payloadValues a Vector of String - each contains the 
     * value describing the correlated dynamic payloads to be added
     * @throws SdpException if either vector is null or empty.
     * if the vector sizes are unequal.
     */
    public void addDynamicPayloads(Vector payloadNames,
				   Vector payloadValues)
	throws SdpException {
	MediaField mediaField = (MediaField)getMedia();
	if (payloadNames == null || payloadValues == null)
	    throw new SdpException(" The vectors are null");
	else {
	    if (payloadNames.isEmpty() || payloadValues.isEmpty())
		throw new SdpException(" The vectors are empty");
	    else {
		if (payloadNames.size() != payloadValues.size())
		    throw new SdpException(" The vector sizes are unequal");
		else {
		    for (int i = 0; i < payloadNames.size(); i++) {
			String name = (String)payloadNames.elementAt(i);
			String value = (String)payloadValues.elementAt(i);
			setAttribute(name, value);
		    }
		}
	    }
	}
    }

}
