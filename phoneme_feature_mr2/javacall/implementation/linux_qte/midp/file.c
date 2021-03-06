/*
 *  $LastChangedDate: 2006-03-31 12:00:00 +0300 (Fri, 31 Mar 2006) $
 *
 * Copyright 2006 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "javacall_file.h"
#include "javacall_memory.h"
#include "javacall_logging.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
//#include <pspiofilemgr.h>
#define MAX_FILE_NAME_LEN 256

char* javacall_UNICODEsToUtf8(const javacall_utf16* fileName, int fileNameLen) {
    static char result[MAX_FILE_NAME_LEN+1];
    if (fileNameLen >= MAX_FILE_NAME_LEN) {
        return NULL;	
    }

    result[fileNameLen] = 0;
    while(fileNameLen-- > 0) {
        result[fileNameLen] = fileName[fileNameLen];
    }
    return &result[0];
}
 
/**
 * Initializes the File System
 * @return <tt>JAVACALL_OK</tt> on success, <tt>JAVACALL_FAIL</tt> or negative value on error
 */
javacall_result javacall_file_init(void) {
    return JAVACALL_OK;
}
/**
 * Cleans up resources used by file system
 * @return <tt>JAVACALL_OK</tt> on success, <tt>JAVACALL_FAIL</tt> or negative value on error
 */ 
javacall_result javacall_file_finalize(void) {
    return JAVACALL_OK;
}

#define DEFAULT_CREATION_MODE_PERMISSION (0666)

/**
 * The open a file
 * @param unicodeFileName path name in UNICODE of file to be opened
 * @param fileNameLen length of file name
 * @param flags open control flags
 *        Applications must specify exactly one of the first three
 *        values (file access modes) below in the value of "flags"
 *        JAVACALL_FILE_O_RDONLY, JAVACALL_FILE_O_WRONLY, JAVACALL_FILE_O_RDWR
 *
 *        Any combination (bitwise-inclusive-OR) of the following may be used:
 *        JAVACALL_FILE_O_CREAT, JAVACALL_FILE_O_TRUNC, JAVACALL_FILE_O_APPEND,
 *
 * @param handle address of pointer to file identifier
 *        on successful completion, file identifier is returned in this 
 *        argument. This identifier is platform specific and is opaque
 *        to the caller.  
 * @return <tt>JAVACALL_OK</tt> on success, 
 *         <tt>JAVACALL_FAIL</tt> or negative value on error
 * 
 */
javacall_result javacall_file_open(const javacall_utf16 * unicodeFileName, int fileNameLen, int flags, /*OUT*/ javacall_handle * handle) {
        printf("javacall_file_open\n");
    char* pszOsFilename = javacall_UNICODEsToUtf8(unicodeFileName, fileNameLen);

    if (pszOsFilename == NULL) {
       return JAVACALL_FAIL;
    }
          
    int creationMode = (flags & JAVACALL_FILE_O_CREAT) ?  DEFAULT_CREATION_MODE_PERMISSION : 0;
    int nativeFlags = 0;
    if (flags & JAVACALL_FILE_O_CREAT) {
    	nativeFlags |= O_CREAT;
    }
    if (flags & JAVACALL_FILE_O_TRUNC) {
    	nativeFlags |= O_TRUNC;
    }
    nativeFlags |= flags & 0x03;

    int fd = open(pszOsFilename, nativeFlags, creationMode);
    

    if (fd < 0) {
        *handle = NULL;
        return JAVACALL_FAIL;
    }

    *handle = (javacall_handle)fd;
    return JAVACALL_OK;
}

/**
 * Closes the file with the specified handlei
 * @param handle handle of file to be closed
 * @return <tt>JAVACALL_OK</tt> on success, 
 *         <tt>JAVACALL_FAIL</tt> or negative value otherwise
 */
javacall_result javacall_file_close(javacall_handle handle) {
printf("javacall_file_close\n");
	int rc = close((int)handle);
    return (rc == 0) ? JAVACALL_OK : JAVACALL_FAIL;
}


/**
 * Reads a specified number of bytes from a file, 
 * @param handle handle of file 
 * @param buf buffer to which data is read
 * @param size number of bytes to be read. Actual number of bytes
 *              read may be less, if an end-of-file is encountered
 * @return the number of bytes actually read
 */
long javacall_file_read(javacall_handle handle, unsigned char *buf, long size) {
	return read((int)handle, buf, size);
}

/**
 * Writes bytes to file
 * @param handle handle of file 
 * @param buf buffer to be written
 * @param size number of bytes to write
 * @return the number of bytes actually written. This is normally the same 
 *         as size, but might be less (for example, if the persistent storage being 
 *         written to fills up).
 */
long javacall_file_write(javacall_handle handle, const unsigned char *buf, long size) {
        printf("javacall_file_write\n");
	return write((int)handle, buf, size);
}

/**
 * Deletes a file from the persistent storage.
 * @param unicodeFileName name of file to be deleted
 * @param fileNameLen length of file name
 * @return JAVACALL_OK on success, <tt>JAVACALL_FAIL</tt> or negative value otherwise
 */
javacall_result javacall_file_delete(const javacall_utf16 * unicodeFileName, int fileNameLen) {
    char* pszOsFilename = javacall_UNICODEsToUtf8(unicodeFileName, fileNameLen);

    if (pszOsFilename == NULL) {
    	return JAVACALL_FAIL;
    }

    int status = unlink(pszOsFilename);

    
    return (status == 0) ? JAVACALL_OK : JAVACALL_FAIL;
}

/**
 * The  truncate function is used to truncate the size of an open file in 
 * the filesystem storage.
 * @param handle identifier of file to be truncated
 *         This is the identifier returned by javacall_file_open()
 *         The handle may be optionally modified by the implementation
 *         of this function
 * @param size size to truncate to
 * @return <tt>JAVACALL_OK</tt> on success, 
 *         <tt>JAVACALL_FAIL</tt> or negative value on error
 */
javacall_result javacall_file_truncate(javacall_handle handle, javacall_int64 size) {
/*
    int status = ftruncate((int)handle, (off_t)size);
    return (status == 0) ? JAVACALL_OK : JAVACALL_FAIL;
    */
    javacall_print("[JAVACALL STUB]: javacall_file_truncate()\n");
    return JAVACALL_OK;
}

/**
 * Sets the file pointer associated with a file identifier 
 * @param handle identifier of file
 *               This is the identifier returned by javacall_file_open()
 * @param offset number of bytes to offset file position by
 * @param flag controls from where offset is applied, from 
 *                 the beginning, current position or the end
 *                 Can be one of JAVACALL_FILE_SEEK_CUR, JAVACALL_FILE_SEEK_SET 
 *                 or JAVACALL_FILE_SEEK_END
 * @return on success the actual resulting offset from beginning of file
 *         is returned, otherwise -1 is returned
 */
javacall_int64 javacall_file_seek(javacall_handle handle, javacall_int64 offset, javacall_file_seek_flags flag) {

    int whence = 
        (flag == JAVACALL_FILE_SEEK_SET) ? SEEK_SET :
        (flag == JAVACALL_FILE_SEEK_CUR) ? SEEK_CUR :
        (flag == JAVACALL_FILE_SEEK_END) ? SEEK_END : 0;
    
	off_t status = lseek((int)handle, offset, whence);
    return (long)status;
}


/**
 * Get file size 
 * @param handle identifier of file
 *               This is the identifier returned by pcsl_file_open()
 * @return size of file in bytes if successful, -1 otherwise
 */
javacall_int64 javacall_file_sizeofopenfile(javacall_handle handle) {

    struct stat stat_buf;
    int status = fstat((int)handle, &stat_buf);

    return (status == 0) ? stat_buf.st_size : -1;

}

/**
 * Get file size
 * @param fileName name of file in unicode format
 * @param fileNameLen length of file name
 * @return size of file in bytes if successful, -1 otherwise 
 */
javacall_int64 javacall_file_sizeof(const javacall_utf16 * fileName, int fileNameLen) {
/*
    javacall_handle handle = 0;
    int flags = 0;
    
    javacall_result rslt1 = javacall_file_open(fileName, fileNameLen, flags, &handle);
    if (rslt1 != JAVACALL_OK) {
        return -1;
    }
    long size = javacall_file_sizeofopenfile(handle);
    javacall_file_close(handle);

    return size;
*/
#if 0
    int res;
    SceIoStat stat;  
    char* pszOsFilename = javacall_UNICODEsToUtf8(fileName, fileNameLen);

    if (pszOsFilename == NULL) {
    	return JAVACALL_FAIL;
    }
    res = sceIoGetstat(pszOsFilename, &stat);
    if (res < 0) {
    	printf("File %s doesn't exist\n", pszOsFilename);
    	return -1;
    } else {
       return stat.st_size;
    }
#endif
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Check if the file exists in file system storage.
 * @param fileName name of file in unicode format
 * @param fileNameLen length of file name
 * @return <tt>JAVACALL_OK </tt> if it exists and is a regular file, 
 *         <tt>JAVACALL_FAIL</tt> or negative value otherwise (eg: 0 returned if it is a directory)
 */
javacall_result javacall_file_exist(const javacall_utf16 * fileName, int fileNameLen) {

    long size = javacall_file_sizeof(fileName, fileNameLen);
    return (size == -1) ? JAVACALL_FAIL : JAVACALL_OK;

}


/** 
 * Force the data to be written into the file system storage
 * @param handle identifier of file
 *               This is the identifier returned by javacall_file_open()
 * @return JAVACALL_OK  on success, <tt>JAVACALL_FAIL</tt> or negative value otherwise
 */
javacall_result javacall_file_flush(javacall_handle handle) {
    return JAVACALL_OK;
}

/**
 * Renames the filename.
 * @param unicodeOldFilename current name of file
 * @param oldNameLen current name length
 * @param unicodeNewFilename new name of file
 * @param newNameLen length of new name
 * @return <tt>JAVACALL_OK</tt>  on success, 
 *         <tt>JAVACALL_FAIL</tt> or negative value otherwise
 */
javacall_result javacall_file_rename(const javacall_utf16 * unicodeOldFilename, int oldNameLen, 
        const javacall_utf16 * unicodeNewFilename, int newNameLen) {
    //TODO: It looks not work properly, need to investigate. Currently it won't be called
    #if 0
    static char oldfn[MAX_FILE_NAME_LEN+1];
    char* pszOldFilename = javacall_UNICODEsToUtf8(unicodeOldFilename, oldNameLen);
    if (pszOldFilename == NULL) {
        return JAVACALL_FAIL;
    }
    
    strcpy(oldfn, pszOldFilename);

    char* pszNewFilename = javacall_UNICODEsToUtf8(unicodeNewFilename, newNameLen);
    if (pszNewFilename == NULL) {        
        return JAVACALL_FAIL;
    }

    int status = sceIoRename(oldfn, pszNewFilename);
    return (status == 0) ? JAVACALL_OK : JAVACALL_FAIL;
    #endif
    return JAVACALL_NOT_IMPLEMENTED;

}


#ifdef __cplusplus
}
#endif
