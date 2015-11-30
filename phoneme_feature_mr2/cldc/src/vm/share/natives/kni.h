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

#ifndef _JAVASOFT_KNI_H_

#ifndef _JVM_CONFIG_H_
#include "jvmconfig.h"
#endif

#define _JAVASOFT_KNI_H_

/**
 * KNI is an implementation-level native function interface
 * for CLDC-category VMs.  KNI is intended to be significantly
 * more lightweight than JNI, so we have made some compromises:
 *
 * - Compile-time interface with static linking.
 * - Source-level (no binary level) compatibility.
 * - No argument marshalling. All native functions have
 *   signature void(*)(). Arguments are read explicitly,
 *   and return values are set explicitly.
 * - No invocation API (cannot call Java from native code).
 * - No class definition support.
 * - Limited object allocation support (strings only).
 * - Limited array region access for arrays of a primitive type.
 * - No monitorenter/monitorexit support.
 * - Limited error handling and exception support.
 *   KNI functions do not throw exceptions, but return error
 *   values/null instead, or go fatal for severe errors.
 * - Exceptions can be thrown explicitly, but no direct
 *   exception manipulation is supported.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * macros to be compatible with CDC-VM
 */
#define KNIDECLARGS 
#define KNIPASSARGS 
#define KNIDECL(methodname) Java_##methodname()

#ifndef KNI_PRIMITIVE_TYPES

#define KNI_PRIMITIVE_TYPES

#include "kni_md.h"

/**
 * KNI Basic Types
 *
 * Note: jbyte, jint and jlong are defined in the
 * machine-specific kni_md.h file.
 */
typedef unsigned char   jboolean;
typedef unsigned short  jchar;
typedef short           jshort;
typedef float           jfloat;
typedef double          jdouble;
typedef jint            jsize;

#endif // KNI_PRIMITIVE_TYPES

/**
 * KNI Reference Types
 *
 * Note: jfieldID and kjobject are intended to be opaque
 * types.  The programmer should not make any assumptions
 * about the actual type of these types, since the actual
 * type may vary from one KNI implementation to another.
 */
struct _jobject;
typedef struct _jobject* kjobject;
typedef kjobject kjclass;
typedef kjobject kjthrowable;
typedef kjobject kjstring;
typedef kjobject kjarray;
typedef kjarray  kjbooleanArray;
typedef kjarray  kjbyteArray;
typedef kjarray  kjcharArray;
typedef kjarray  kjshortArray;
typedef kjarray  kjintArray;
typedef kjarray  kjlongArray;
typedef kjarray  kjfloatArray;
typedef kjarray  kjdoubleArray;
typedef kjarray  kjobjectArray;
/**
 * KNI Field Type
 */
struct _jfieldID;
typedef struct _jfieldID* jfieldID;

/**
 * jboolean constants
 */
#define KNI_FALSE 0
#define KNI_TRUE  1

/**
 * Return values for KNI functions.
 * Values correspond to JNI.
 */
#define KNI_OK           0                 /* success */
#define KNI_ERR          (-1)              /* unknown error */
#define KNI_ENOMEM       (-4)              /* not enough memory */
#define KNI_EINVAL       (-6)              /* invalid arguments */

#ifdef __cplusplus
# define KNIEXPORT extern "C"
#else  /* __cplusplus */
# define KNIEXPORT extern
#endif /* __cplusplus */

/*
 * Version information
 */
#define KNI_VERSION      0x00010000        /* KNI version 1.0 */

/******************************************************************
 * KNI functions (refer to KNI Specification for details)
 ******************************************************************/

/**
 * Version information
 */
KNIEXPORT jint KNI_GetVersion();

/**
 * Class and interface operations
 */
KNIEXPORT void     KNI_FindClass(const char* name, kjclass classHandle);
KNIEXPORT void     KNI_GetSuperClass(kjclass classHandle,
                       kjclass superclassHandle);
KNIEXPORT jboolean KNI_IsAssignableFrom(kjclass classHandle1,
                       kjclass classHandle2);

/**
 * Exceptions and errors
 */
KNIEXPORT jint     KNI_ThrowNew(const char* name, const char* message);
KNIEXPORT void     KNI_FatalError(const char* message);

/**
 * Object operations
 */
KNIEXPORT void     KNI_GetObjectClass(kjobject objectHandle,
                       kjclass classHandle);
KNIEXPORT jboolean KNI_IsInstanceOf(kjobject objectHandle, kjclass classHandle);
KNIEXPORT jboolean KNI_IsSameObject(kjobject obj1, kjobject obj2);

/**
 * Instance field access.
 *
 * Note specific to this VM: KNI_GetFieldID() must not be applied 
 * on a class whose 
 * fields have been renamed by the romizer. You need to use
 * the DontRenameNonPublicFields flag in your romizer configuration
 * file. See src/vm/cldc_rom.cfg in this VM source distribution
 * for an example.
 */
KNIEXPORT jfieldID KNI_GetFieldID(kjclass classHandle, const char* name,
                       const char* signature);

KNIEXPORT jboolean KNI_GetBooleanField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT jbyte    KNI_GetByteField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT jchar    KNI_GetCharField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT jshort   KNI_GetShortField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT jint     KNI_GetIntField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT jlong    KNI_GetLongField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT jfloat   KNI_GetFloatField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT jdouble  KNI_GetDoubleField(kjobject objectHandle, jfieldID fieldID);
KNIEXPORT void     KNI_GetObjectField(kjobject objectHandle, jfieldID fieldID,
                       kjobject toHandle);

KNIEXPORT void     KNI_SetBooleanField(kjobject objectHandle, jfieldID fieldID,
                       jboolean value);
KNIEXPORT void     KNI_SetByteField(kjobject objectHandle, jfieldID fieldID,
                       jbyte value);
KNIEXPORT void     KNI_SetCharField(kjobject objectHandle, jfieldID fieldID,
                       jchar value);
KNIEXPORT void     KNI_SetShortField(kjobject objectHandle, jfieldID fieldID,
                       jshort value);
KNIEXPORT void     KNI_SetIntField(kjobject objectHandle, jfieldID fieldID,
                       jint value);
KNIEXPORT void     KNI_SetLongField(kjobject objectHandle, jfieldID fieldID,
                       jlong value);
KNIEXPORT void     KNI_SetFloatField(kjobject objectHandle, jfieldID fieldID,
                       jfloat value);
KNIEXPORT void     KNI_SetDoubleField(kjobject objectHandle, jfieldID fieldID,
                       jdouble value);
KNIEXPORT void     KNI_SetObjectField(kjobject objectHandle, jfieldID fieldID,
                       kjobject fromHandle);

/**
 * Static field access
 */
KNIEXPORT jfieldID KNI_GetStaticFieldID(kjclass classHandle, const char* name,
                       const char* signature);

KNIEXPORT jboolean KNI_GetStaticBooleanField(kjclass classHandle,
                       jfieldID fieldID);
KNIEXPORT jbyte    KNI_GetStaticByteField(kjclass classHandle,
                       jfieldID fieldID);
KNIEXPORT jchar    KNI_GetStaticCharField(kjclass classHandle,
                       jfieldID fieldID);
KNIEXPORT jshort   KNI_GetStaticShortField(kjclass classHandle,
                       jfieldID fieldID);
KNIEXPORT jint     KNI_GetStaticIntField(kjclass classHandle, jfieldID fieldID);
KNIEXPORT jlong    KNI_GetStaticLongField(kjclass classHandle,
                       jfieldID fieldID);
KNIEXPORT jfloat   KNI_GetStaticFloatField(kjclass classHandle,
                       jfieldID fieldID);
KNIEXPORT jdouble  KNI_GetStaticDoubleField(kjclass classHandle,
                       jfieldID fieldID);
KNIEXPORT void     KNI_GetStaticObjectField(kjclass classHandle,
                       jfieldID fieldID, kjobject toHandle);

KNIEXPORT void     KNI_SetStaticBooleanField(kjclass classHandle,
                       jfieldID fieldID, jboolean value);
KNIEXPORT void     KNI_SetStaticByteField(kjclass classHandle,
                       jfieldID fieldID, jbyte value);
KNIEXPORT void     KNI_SetStaticCharField(kjclass classHandle,
                       jfieldID fieldID, jchar value);
KNIEXPORT void     KNI_SetStaticShortField(kjclass classHandle,
                       jfieldID fieldID, jshort value);
KNIEXPORT void     KNI_SetStaticIntField(kjclass classHandle,
                       jfieldID fieldID, jint value);
KNIEXPORT void     KNI_SetStaticLongField(kjclass classHandle,
                       jfieldID fieldID, jlong value);
KNIEXPORT void     KNI_SetStaticFloatField(kjclass classHandle,
                       jfieldID fieldID, jfloat value);
KNIEXPORT void     KNI_SetStaticDoubleField(kjclass classHandle,
                       jfieldID fieldID, jdouble value);
KNIEXPORT void     KNI_SetStaticObjectField(kjclass classHandle,
                       jfieldID fieldID, kjobject fromHandle);

/**
 * String operations
 */
KNIEXPORT jsize    KNI_GetStringLength(kjstring stringHandle);
KNIEXPORT void     KNI_GetStringRegion(kjstring stringHandle, jsize offset,
                       jsize n, jchar* jcharbuf);
KNIEXPORT void     KNI_NewStringUTF(const char* utf8chars,
                       kjstring stringHandle);
KNIEXPORT void     KNI_NewString(const jchar*, jsize, kjstring);

/**
 * Array operations
 */
KNIEXPORT jsize    KNI_GetArrayLength(kjarray array);

KNIEXPORT jboolean KNI_GetBooleanArrayElement(kjbooleanArray arrayHandle,
                       jsize index);
KNIEXPORT jbyte    KNI_GetByteArrayElement(kjbyteArray arrayHandle,
                       jsize index);
KNIEXPORT jchar    KNI_GetCharArrayElement(kjcharArray arrayHandle,
                       jsize index);
KNIEXPORT jshort   KNI_GetShortArrayElement(kjshortArray arrayHandle,
                       jsize index);
KNIEXPORT jint     KNI_GetIntArrayElement(kjintArray arrayHandle,
                       jsize index);
KNIEXPORT jlong    KNI_GetLongArrayElement(kjlongArray arrayHandle,
                       jsize index);
KNIEXPORT jfloat   KNI_GetFloatArrayElement(kjfloatArray arrayHandle,
                       jsize index);
KNIEXPORT jdouble  KNI_GetDoubleArrayElement(kjdoubleArray arrayHandle,
                       jsize index);
KNIEXPORT void     KNI_GetObjectArrayElement(kjobjectArray arrayHandle,
                       jsize index, kjobject toHandle);

KNIEXPORT void     KNI_SetBooleanArrayElement(kjbooleanArray arrayHandle,
                       jsize index, jboolean value);
KNIEXPORT void     KNI_SetByteArrayElement(kjbyteArray arrayHandle,
                       jsize index, jbyte value);
KNIEXPORT void     KNI_SetCharArrayElement(kjcharArray arrayHandle,
                       jsize index, jchar value);
KNIEXPORT void     KNI_SetShortArrayElement(kjshortArray arrayHandle,
                       jsize index, jshort value);
KNIEXPORT void     KNI_SetIntArrayElement(kjintArray arrayHandle,
                       jsize index, jint value);
KNIEXPORT void     KNI_SetLongArrayElement(kjlongArray arrayHandle,
                       jsize index, jlong value);
KNIEXPORT void     KNI_SetFloatArrayElement(kjfloatArray arrayHandle,
                       jsize index, jfloat value);
KNIEXPORT void     KNI_SetDoubleArrayElement(kjdoubleArray arrayHandle,
                       jsize index, jdouble value);
KNIEXPORT void     KNI_SetObjectArrayElement(kjobjectArray arrayHandle,
                       jsize index, kjobject fromHandle);

KNIEXPORT void     KNI_GetRawArrayRegion(kjarray arrayHandle, jsize offset,
                       jsize n, jbyte* dstBuffer);
KNIEXPORT void     KNI_SetRawArrayRegion(kjarray arrayHandle, jsize offset,
                       jsize n, const jbyte* srcBuffer);

/**
 * Parameter passing
 */

extern unsigned char * _kni_parameter_base;

KNIEXPORT jboolean KNI_GetParameterAsBoolean(jint index);
KNIEXPORT jbyte    KNI_GetParameterAsByte(jint index);
KNIEXPORT jchar    KNI_GetParameterAsChar(jint index);
KNIEXPORT jshort   KNI_GetParameterAsShort(jint index);
KNIEXPORT jint     KNI_GetParameterAsInt(jint index);
KNIEXPORT jlong    KNI_GetParameterAsLong(jint index);
KNIEXPORT jfloat   KNI_GetParameterAsFloat(jint index);
KNIEXPORT jdouble  KNI_GetParameterAsDouble(jint index);
KNIEXPORT void     KNI_GetParameterAsObject(jint index, kjobject toHandle);

KNIEXPORT void     KNI_GetThisPointer(kjobject toHandle);
KNIEXPORT void     KNI_GetClassPointer(kjclass toHandle);

#define            KNI_ReturnVoid()         return
#define            KNI_ReturnBoolean(value) return ((jint)((value) ? 1 : 0))
#define            KNI_ReturnByte(value)    return ((jint)((jbyte)(value)))
#define            KNI_ReturnChar(value)    return ((jint)((jchar)(value)))
#define            KNI_ReturnShort(value)   return ((jint)((jshort)(value)))
#define            KNI_ReturnInt(value)     return ((jint)(value))
#define            KNI_ReturnFloat(value)   return ((jfloat)(value))
#define            KNI_ReturnLong(value)    return ((jlong)(value))
#define            KNI_ReturnDouble(value)  return ((jdouble)(value))

typedef struct __KNI_HandleInfo {
  struct __KNI_HandleInfo *prev;  /* A _KNI_HandleInfo that nests
                                   * this one. */
  int total_count;                /* Number of total handles */
  int declared_count;             /* Number of handles that has been 
                                   * declared using KNI_DeclareHandle() */
  kjobject *handles;               /* Points to __handles__ in this scope */
} _KNI_HandleInfo;

/*
 * Internal functions, do not use directly.
 */
KNIEXPORT int      _KNI_push_handles(int, _KNI_HandleInfo*, kjobject*);
KNIEXPORT void     _KNI_pop_handles(_KNI_HandleInfo*);

/**
 * Handle operations
 */
#define KNI_StartHandles(n) \
{ \
  _KNI_HandleInfo __handle_info__; \
  kjobject __handles__[n]; \
  int __dummy__ = _KNI_push_handles(n, &__handle_info__, __handles__)

#define KNI_DeclareHandle(x) \
  kjobject x = (kjobject)(void*)&__handles__[__handle_info__.declared_count++]

#define KNI_IsNullHandle(x) \
  (*(kjobject*)x == 0)

#define KNI_ReleaseHandle(x) \
  *(kjobject*)x = 0

#define KNI_EndHandles() \
  (void)_KNI_pop_handles(&__handle_info__); \
  (void)__dummy__; \
}

#define KNI_EndHandlesAndReturnObject(x) \
    (void)_KNI_pop_handles(&__handle_info__); \
    (void)__dummy__; \
    return *(kjobject*)x; \
}

#ifdef __ARMCC_VERSION
/*
 * For ADS, we need to declare __softfp for all floating point Java
 * native methods, as well as all C floating point routines (such as
 * jvm_fadd) that are called directly from the interpreter, which
 * always uses integer registers for passing parameters and return
 * values.
 */
#define JVM_SOFTFP_LINKAGE __softfp
#else
/*
 * Change this if your C/C++ compiler uses a special keyword to indicate
 * floating point linkage.
 */
#define JVM_SOFTFP_LINKAGE
#endif

/**
 * Type macros
 */
#define KNI_RETURNTYPE_VOID    void
#define KNI_RETURNTYPE_BOOLEAN jint
#define KNI_RETURNTYPE_BYTE    jint
#define KNI_RETURNTYPE_CHAR    jint
#define KNI_RETURNTYPE_SHORT   jint
#define KNI_RETURNTYPE_INT     jint
#define KNI_RETURNTYPE_FLOAT   JVM_SOFTFP_LINKAGE jfloat
#define KNI_RETURNTYPE_LONG    jlong
#define KNI_RETURNTYPE_DOUBLE  JVM_SOFTFP_LINKAGE jdouble
#define KNI_RETURNTYPE_OBJECT  kjobject

#ifdef __cplusplus
}
#endif

#endif /* !_JAVASOFT_KNI_H_ */
