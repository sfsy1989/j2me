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

import com.sun.j2me.global.LocaleHelpers;
import com.sun.j2me.global.ResourceManagerFactory;
import com.sun.j2me.global.ResourceAbstractionLayer;
import javax.microedition.global.ResourceException;

/**
 * Resource manager functions implemetation. Provides a functionality of
 * {@link javax.microedition.global.ResourceManager}.
 * This class is required due to requirement do not have public/protected
 * constructor in javax.microedition.global.ResourceManager.
 * More detailed comments see for javax.microedition.global.ResourceManager.
 */
public class ResourceManager {

    /**
     * The instance of ResourceAbstractionLayer.
     */
    private static ResourceAbstractionLayer abstractionLayer =
            ResourceAbstractionLayer.getInstance();

    /**
     * Constant to indicate device resources.
     */
    public final static String DEVICE = "";

    /**
     * The base name.
     */
    private String baseName;
    /**
     * The locale identifier.
     */
    private String locale;

    /**
     * Gets a resource manager for the specified base name and locale. <p>
     *
     * If the base name is {@link #DEVICE} (the empty string), the resource
     * manager can only retrieve device resources. Other base names retrieve
     * application resources. The base name must not be <code>null</code>. <p>
     *
     * If resources for the specified locale are not found, this method
     * attempts to find resources by hierarchical matching. The matching
     * proceeds from <em>language-country-variant</em> to <em>language-country
     * </em>, then on to <em>language</em> , and finally to the empty string.
     * An empty string can also be used as the locale. It is a shortcut to
     * using common resources without going through the matching process. <p>
     *
     * If resources for the combination of the base name and the matching
     * locale are found, this method returns a manager for those resources. If
     * no resources are found for the combination, a
     * <code>ResourceException</code> is thrown.
     *
     * @param  baseName                 the base name, non-empty for
     *      application resources, <code>DEVICE</code> (the empty string) for
     *      device resources
     * @param  locale                   the locale to use either as is or as
     *      a starting point for hierarchical matching
     * @return                          the manager instance for the base
     *      name and the specified or matched locale
     * @throws  ResourceException           if no resources for the combination
     *      of base name and locale are found, or the resource file is invalid
     * @see                                 #DEVICE
     */
    public final static ResourceManager getManager(String baseName,
            String locale) throws ResourceException {
        // DevResourceManager
        if (baseName.equals(DEVICE)) {
            ResourceManagerFactory devMFactory =
                    abstractionLayer.getDevResourceManagerFactory();
            return devMFactory.getManager(DEVICE, locale);
        }
        // appResourceManager
        else {
            ResourceManagerFactory appMFactory =
                    abstractionLayer.getAppResourceManagerFactory();
            return appMFactory.getManager(baseName, locale);
        }
    }

    /**
     * Gets a resource manager for the specified base name and the first
     * matching locale in the supplied array. <p>
     *
     * If the base name is {@link #DEVICE} (the empty string), the resource
     * manager can only retrieve device resources. Other base names retrieve
     * application resources. The base name MUST NOT be <code>null</code>. <p>
     *
     * This method attempts to get a resource manager for a combination of the
     * base name and one of the locales in the array. The locales are tried
     * successively, stopping at the first match. This method MUST NOT perform
     * hierarchical matching. If none of the locales in the array result in a
     * match, a <code>ResourceException</code> MUST be thrown with the
     * appropriate error code. <p>
     *
     * <em>For example</em> , if the array of locales contains <code>{ "de-DE",
     * "fr", "en" }</code>, and resources are not found for <code>"de-DE"</code>
     * or <code>"fr"</code>, but are found for <code>"en"</code>, then this
     * method gets a manager for <code>"en"</code>. However, if resources are
     * not found for <code>"de-DE"</code>, but are found for <code>"fr"</code>,
     * then this method gets a manager for <code>"fr"</code> and disregards the
     * <code>"en"</code> locale. In this case if resources are not found for
     * <code>"de-DE"</code>, the locale <code>"de"</code> MUST NOT be tried;
     * instead the matching proceeds to the next locale in the array. <p>
     *
     * The <code>locales</code> array MUST NOT be <code>null</code>. Each of
     * the locale identifiers in the array MUST be a valid locale identifier,
     * otherwise an <code>IllegalArgumentException</code> is thrown. As an
     * extension, the empty string can be used to use resources common to all
     * locales.
     *
     * @param  baseName                     the base name, non-empty for
     *      application resources, or <code>DEVICE</code> (the empty string) for
     *      device resources
     * @param  locales                      the array of locale identifiers to
     *      try
     * @return                              the manager instance for the base
     *      name and the matched locale
     * @see                                 #DEVICE
     */
    public final static ResourceManager getManager(String baseName,
            String[] locales) {

        String[] norm_locs = new String[locales.length];
        for (int i = 0; i < locales.length; i++) {
            norm_locs[i] = LocaleHelpers.normalizeLocale(locales[i]);
        }

        // DevResourceManager
        if (baseName.equals(DEVICE)) {
            ResourceManagerFactory devMFactory =
                    abstractionLayer.getDevResourceManagerFactory();
            return devMFactory.getManager(DEVICE, norm_locs);
        }
        // appResourceManager
        else {
            ResourceManagerFactory appMFactory =
                    abstractionLayer.getAppResourceManagerFactory();
            return appMFactory.getManager(baseName, norm_locs);
        }

    }

    /**
     * Gets the locales supported by this resource manager for the given base
     * name. Returns an array of valid <code>microedition.locale</code> values.
     * If there are no supported locales, returns an empty array
     * (not <code>null</code>).
     * <p>
     *
     * The base name is used to distinguish between application and device
     * resources. If the base name is non-empty, this method returns the list
     * of locales for which application-specific resources are defined. If the
     * base name is {@link #DEVICE} (the empty string), the set of supported
     * device locales is returned instead. <p>
     *
     * The returned array MAY be empty if no locales are supported for the
     * specified base name. It MUST NOT be <code>null</code>. <p>
     *
     * Values in the array MUST be unique. The value <code>null</code> MUST NOT
     * appear in the returned array. However, if there are resources for this
     * base name which are common to all locales, the array MUST contain the
     * empty string. It SHOULD be first element in the array. <p>
     *
     * The supported locales for an application resource manager require a base
     * name specific meta-information file described in the "Enumerating
     * supported locales" section of the specification overview. If the
     * meta-information file is not found, this method MUST throw
     * a <code>ResourceException</code> with the error code
     * <code>METAFILE_NOT_FOUND</code>. <p>
     *
     * If the content of the meta-information file is not syntactically valid,
     * this method MUST throw a ResourceException with the error code
     * <code>DATA_ERROR</code>
     * <p>
     *
     * The list of locales supported by a resource manager for a base name MAY
     * be different than in a formatter, i.e. the implementation MAY support
     * formatting rules for a certain set of locales, but an application can
     * supply resources for an entirely different (or overlapping) set of
     * locales.
     *
     * @param  baseName               the base name, non-empty for application
     *      resources, <code>DEVICE</code> (the empty string) for device
     *      resources
     * @return                        the list of supported locales for the
     *      specified base name, possibly empty
     * @throws  NullPointerException  if the base name is <code>null</code>
     * @throws  ResourceException     if the meta-information file is not found
     *      or it is not of the specified format
     * @see    javax.microedition.global.Formatter#getSupportedLocales()
     * @see    javax.microedition.global.StringComparator#getSupportedLocales()
     * @see    javax.microedition.global.ResourceException
     */
    public static String[] getSupportedLocales(String baseName) {
        if (baseName == null) {
            throw new NullPointerException("Base name is null");
        }

        // DevResourceManager
        if (baseName.equals(DEVICE)) {
            ResourceManagerFactory devMFactory =
                    abstractionLayer.getDevResourceManagerFactory();
            return devMFactory.getSupportedLocales(baseName);
        }
        // appResourceManager
        else {
            ResourceManagerFactory appMFactory =
                    abstractionLayer.getAppResourceManagerFactory();
            return appMFactory.getSupportedLocales(baseName);
        }
    }

    /**
     * Creates a new instance of <code>ResourceManager</code>.
     */
    protected ResourceManager() { }

    /**
     * Sets base name for resource files used by this
     * <code>ResourceManager</code>.
     *
     * @param baseName  the base name, non-empty for application
     */
    protected void setBaseName(String baseName) {
        this.baseName = baseName;
    }

    /**
     * Sets locale code for this <code>ResourceManager</code>.
     *
     * @param locale  the locale code
     */
    protected void setLocale(String locale) {
        this.locale = locale;
    }

    /**
     * Gets the base name of this resource manager.
     *
     * @return    the base name (<code>DEVICE</code> if this resource manager is
     *      retrieving device-specific resources)
     */
    public String getBaseName() {
        return baseName;
    }

    /**
     * Gets current locale of this <code>ResourceManager</code>.
     *
     * @return    the locale identifier
     */
    public String getLocale() {
        return locale;
    }

    /**
     * Gets a resource with the specified resource ID as a byte array
     * containing arbitrary binary data. If the resource with the specified ID
     * is not binary data, or the resource does not exist, a ResourceException
     * is thrown with the appropriate error code. <p>
     *
     * This method performs a hierarchical lookup of the resource.
     *
     * @param  id                         the resource ID
     * @return                            byte array containing the data
     * @throws  ResourceException         if the resource does not exist, or if
     *      the resource is not the correct type
     * @throws  IllegalArgumentException  if the resource ID is invalid
     */
    public byte[] getData(int id) throws ResourceException {
        throw new ResourceException(ResourceException.UNKNOWN_ERROR, "");
    }

    /**
     * Gets a string with the specified resource ID. This is a convenience
     * method to avoid typecasting in applications. If the resource with the
     * specified ID is not a string, or the resource does not exist, a
     * ResourceException is thrown with the appropriate error code. <p>
     *
     * This method performs a hierarchical lookup of the resource.
     *
     * @param  id                         the resource ID
     * @return                            the string
     * @throws  ResourceException         if the resource does not exist, or if
     *      the resource is not the correct type
     * @throws  IllegalArgumentException  if the resource ID is invalid
     */
    public String getString(int id) throws ResourceException {
        throw new ResourceException(ResourceException.UNKNOWN_ERROR, "");
    }

    /**
     * Gets a resource with the specified ID. The resource is either an
     * application-specific resource or a device resource, depending on the
     * base name of the resource manager. <p>
     *
     * This method performs a hierarchical lookup of the resource.
     *
     * @param  id                         the resource ID
     * @return                            the resource object
     * @throws  ResourceException         if the resource is not found, or if an
     *      application resource has an unknown type
     * @throws  IllegalArgumentException  if the resource ID is invalid
     * @see                               #getString(int)
     * @see                               #getData(int)
     */
    public Object getResource(int id) throws ResourceException {
        throw new ResourceException(ResourceException.UNKNOWN_ERROR, "");
    }

    /**
     * Returns the caching status of this resource manager.
     *
     * @return    <code>true</code> if caching, <code>false</code> if not
     */
    public boolean isCaching() {
        return false;
    }

    /**
     * Determines if the given resource ID is valid in this implementation. In
     * this context "valid" means that a resource exists with the given
     * resource ID, and calling <code>getResource</code> with that ID returns a
     * non-<code>null</code> object instance. <p>
     *
     * The method does not discriminate between application and device
     * resources, since one resource manager can only handle one type of
     * resource.
     *
     * @param  id                         the resource ID
     * @return                            <code>true</code> if the resource ID
     *      is valid, <code>false</code> if not
     * @throws  IllegalArgumentException  if the resource ID is not in the
     *      defined range
     * @see                               #getResource(int)
     * @see                               #getString(int)
     * @see                               #getData(int)
     */
    public boolean isValidResourceID(int id) {
        return false;
    }
}
