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
#include <jsr238_resources.h>
#include <midp_logging.h>
#include <midpString.h>
#include <midpStorage.h>
#include <pcsl_file.h>

#ifndef NULL
#define NULL    0
#endif
#define JSR238_FILE_O_RDONLY             0x00
#define WORDS_DELIMITER             0x20            /* Code for the words delimiter (space) */
#define QUOTATION_MARK              0x22            /* Code for the quatation mark */

typedef struct {
    int resourceID;
    unsigned char resourceType;
    int resourceOffset;
}ResourceTableEntryType;

typedef struct {
    unsigned char signature[4];
    int headerLength;
}ResourceFileHeaderType;

#define JSR238_STORAGE_ROOT "global/"      /* path to the "platform storage" */
#define JSR238_BASENAME "common.res"       /* basename" */
#define JSR238_METAFILENAME "_common"      /* metafile name for basename */

ResourceFileHeaderType ResourceFileHeader;

static int jsr238_file_open(char *fileName, int flags, void **handle);
static int jsr238_read_word(void *handle, unsigned  char *buf);
static int jsr238_devresource_file_open(int locale_index, void **handle);
static void jsr238_fillInt(int* dst, unsigned char* src);
static jboolean jsr238_seekEntry(void *handle, int entry_id, int rn, ResourceTableEntryType * entry);
static int jsr238_skip_space(void * handle, unsigned  char *buf);

/* Public API */

/**
 * Gets the number of supported locales for device resources.
 *
 * @return the number of supported locales or 0 if something is wrong
 */
jint jsr238_get_resource_locales_count() {
    int res = 0;
    void *handle;

    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_resource_locales_count()\n");
    jsr238_file_open(JSR238_METAFILENAME, 0, &handle);
    if (handle != NULL) {
        while (jsr238_read_word(handle, NULL) >= 0) {
            res++;
        }
        pcsl_file_close(handle);
    }
    return res;
}

/**
 * Gets a locale name for device resources for the index.
 *
 * @param loc    buffer for the locale
 * @param index  index of the locale
 * @return 0 on success, -1 otherwise
 */
JSR238_STATUSCODE jsr238_get_resource_locale(jbyte* loc, jint index) {
    int res = 0;
    int i;
    void *handle;

    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_resource_locale()\n");
    jsr238_file_open(JSR238_METAFILENAME, 0, &handle);
    if (handle != NULL) {
        for (i = 0; i < index; i++) {
            res = jsr238_read_word(handle, NULL);
        }
        if (res >= 0) {
            res = jsr238_read_word(handle, loc);
            if (res > 0) {
                res = JSR238_STATUSCODE_OK;
            }
        } else {
            res = JSR238_STATUSCODE_FAIL;
        }

        pcsl_file_close(handle);
    }
    return res;
}

/**
 * Gets a resource for pointed resource identifier and locale.
 *
 * @param resource      buffer for the resource
 * @param res_len       length of the resource buffer
 * @param resource_id   resource identifier
 * @param locale_index  index of the locale
 * @return 0 in case successful operation and -1 if something is wrong
 */
JSR238_STATUSCODE jsr238_get_resource(jbyte* resource, jint res_len, jint resource_id, jint locale_index) {
    void * handle;
    ResourceTableEntryType entry;
    unsigned char buf[4];
    int res = JSR238_STATUSCODE_FAIL;

    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_resource()\n");
    if (jsr238_devresource_file_open(locale_index, &handle) >= 0)
    {
        pcsl_file_read(handle, (char*)&ResourceFileHeader, 4);
        pcsl_file_read(handle, buf, 4);
        jsr238_fillInt(&ResourceFileHeader.headerLength, buf);
        if (jsr238_seekEntry(handle, resource_id, ResourceFileHeader.headerLength / 8, &entry)){
            pcsl_file_seek(handle, entry.resourceOffset, 0);
            pcsl_file_read(handle, resource, res_len);
            res = JSR238_STATUSCODE_OK;
        }
        pcsl_file_close(handle);
    }
    return res;
}

/**
 * Gets a resource type for pointed resource identifier and locale.
 *
 * @param resource_id   resource identifier
 * @param locale_index  index of the locale
 * @return resource type in case successful operation and -1 if something is wrong
 */
JSR238_STATUSCODE jsr238_get_resource_type(jint* resType /* OUT */, jint resource_id, jint locale_index) {
    ResourceTableEntryType entry;
    unsigned char buf[4];
    void * handle;
    jint res = JSR238_STATUSCODE_FAIL;

    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_resource_type()\n");
    if (jsr238_devresource_file_open(locale_index, &handle) >= 0)
    {
        pcsl_file_read(handle, (char*)&ResourceFileHeader, 4);
        pcsl_file_read(handle, buf, 4);
        jsr238_fillInt(&ResourceFileHeader.headerLength, buf);
        if (jsr238_seekEntry(handle, resource_id, ResourceFileHeader.headerLength / 8, &entry)){
            *resType = entry.resourceType;
            res = JSR238_STATUSCODE_OK;
        }
        pcsl_file_close(handle);
    }
    return res;
}

/**
 * Checks if resource with given identifier exists.
 *
 * @param resource_id   resource identifier
 * @param locale_index  index of the locale
 * @return 1 if resource ID is valid and 0 if something is wrong
 */
jboolean jsr238_is_valid_resource_id(jint resource_id, jint locale_index) {
    ResourceTableEntryType entry;
    unsigned char buf[4];
    void * handle;
    jboolean res = PCSL_FALSE;

    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_is_valid_resource_id()\n");
    if (jsr238_devresource_file_open(locale_index, &handle) >= 0) {
        pcsl_file_read(handle, (char*)&ResourceFileHeader, 4);
        pcsl_file_read(handle, buf, 4);
        jsr238_fillInt(&ResourceFileHeader.headerLength, buf);
        res = jsr238_seekEntry(handle, resource_id, ResourceFileHeader.headerLength / 8, &entry);
        pcsl_file_close(handle);
    }

    return res;
}

/**
 * Gets a resource length for pointed reource identifier and locale.
 *
 * @param resource_id   resource identifier
 * @param locale_index  index of the locale
 * @return resource length in case successful operation and -1 if something is wrong
 */
JSR238_STATUSCODE jsr238_get_resource_length(jsize* length /* OUT */, jint resource_id, jint locale_index) {
    ResourceTableEntryType entry;
    unsigned char buf[4];
    void * handle;
    JSR238_STATUSCODE res = JSR238_STATUSCODE_FAIL;

    REPORT_CALL_TRACE(LC_LOWUI, "LF:STUB:jsr238_get_resource_length()\n");
    if (jsr238_devresource_file_open(locale_index, &handle) >= 0)
    {
        pcsl_file_read(handle, (char*)&ResourceFileHeader, 4);
        pcsl_file_read(handle, buf, 4);
        jsr238_fillInt(&ResourceFileHeader.headerLength, buf);
        if (jsr238_seekEntry(handle, resource_id, ResourceFileHeader.headerLength / 8, &entry)){
            *length = entry.resourceOffset;
            pcsl_file_read(handle, buf, 4);
            pcsl_file_read(handle, buf, 4);
            buf[0] = 0;
            jsr238_fillInt(&entry.resourceOffset, buf);
            *length = entry.resourceOffset - *length;
            res = JSR238_STATUSCODE_OK;
        }
        pcsl_file_close(handle);
    }
    return res;
}

/* Helper-functions */

/**
 * (Internal) Open file.
 * fileName - File name.
 * flags - flags for open file.
 * handle - pointer for the file handler.
 */
static int jsr238_file_open(char *fileName, int flags, void **handle)
{
    MidpString midp_fname = {0,0};
    int  res = -1;

    midp_fname = midpStringCat(midp_fname, storageGetRoot());
    midp_fname = midpStringCat(midp_fname, midpCharsToJchars(JSR238_STORAGE_ROOT));
    midp_fname = midpStringCat(midp_fname, midpCharsToJchars(fileName));

    if (pcsl_file_open(midp_fname.data, midp_fname.len, flags, handle) == 0) {
        if (*handle != NULL){
            res = 0;
        }
    }
       
    return res;
}

/**
 * (Internal)
 * Read word from a text file,
 * word is a sequence of symbols not equal ' '.
 * handle - file handler,
 * buf - buffer for input,
 * return number of read symbols or -1 if something wrong
 */
int jsr238_read_word(void *handle, unsigned  char *buf)
{
    int res;
    unsigned char lbuf[MAX_LOCALE_LENGTH];

    unsigned char *buf_ptr;

    if (buf == NULL)
    {
        buf_ptr = lbuf;
    }
    else
    {
        buf_ptr = buf;
    }
    if ((res = jsr238_skip_space(handle, buf_ptr)) > 0)
    {
        if (*buf_ptr == QUOTATION_MARK) {
            if (pcsl_file_read(handle, buf_ptr, 1) <= 0) {
                return -1;
            }
        }
        res = 0;
        while (*buf_ptr > QUOTATION_MARK) {
            buf_ptr++;
            res++;
            if (pcsl_file_read(handle, buf_ptr, 1) <= 0) {
                break;
            }
        }
        *buf_ptr = 0;
        return res;
    }
    return -1;
}

/**
 * (Internal)
 * Skip spaces in  a text file.
 * handle - file handler,
 * buf - buffer for input,
 * return 1 if non-delimiting symbol is read, 0 on EOF, -1 on error
 */
int jsr238_skip_space(void * handle, unsigned  char *buf)
{
    int res;

    do {
        res = pcsl_file_read(handle, buf, 1);
    } while ((*buf <= WORDS_DELIMITER) && (res > 0));
    return res;
}


/**
 * (Internal) Open device resource file for the locale.
 * locale_index - index of the locale.
 * handle - pointer for the file handler.
 */
int jsr238_devresource_file_open(int locale_index, void **handle) {
    unsigned char locale[MAX_LOCALE_LENGTH];
    MidpString midp_fname = {0,0};
    int res = -1;

    if (jsr238_get_resource_locale(locale, locale_index) >= 0)
    {
        midp_fname = midpStringCat(midp_fname, midpCharsToJchars(JSR238_STORAGE_ROOT));
        midp_fname = midpStringCat(midp_fname, midpCharsToJchars(locale));
        midp_fname = midpStringCat(midp_fname, midpCharsToJchars("/"));
        midp_fname = midpStringCat(midp_fname, midpCharsToJchars(JSR238_BASENAME));
        midp_fname = midpStringCat(storageGetRoot(), midp_fname);
    
        if (pcsl_file_open(midp_fname.data, midp_fname.len, PCSL_FILE_O_RDONLY, handle) == 0) {
            if (*handle != NULL){
                res = 0;
            }
        }
    }
    return res;
}

/**
 * (Internal) Fill the integer from the string.
 * dst - pointer to the destination integer.
 * src - pointer to the source string.
 */
void jsr238_fillInt(int* dst, unsigned char* src){
    *dst = ((int)*src)<<24;
    src++;
    *dst += ((int)*src<<16);
    src++;
    *dst += ((int)*src<<8);
    src++;
    *dst += (int)*src;
}

/**
 * (Internal) Seek required entry in the device resource file header.
 * handle - device resource file handler.
 * entry_id - ID of the required entry.
 * rn - number of entries in the device resource file.
 * entry - pointer to the buffer for the entry.
 */
jboolean jsr238_seekEntry(void *handle, int entry_id, int rn, ResourceTableEntryType * entry){
    int i;
    unsigned char bufEntry[8];
    jboolean res = PCSL_FALSE;

    for (i=0;i < rn;i++) {
        pcsl_file_read(handle, bufEntry, 8);
        jsr238_fillInt(&entry->resourceID, bufEntry);
        if (entry->resourceID == entry_id) {
            entry->resourceType = bufEntry[4];
            bufEntry[4] = 0;
            jsr238_fillInt(&entry->resourceOffset, &bufEntry[4]);
            res = PCSL_TRUE;
            break;
        }
    }
    return res;
}
