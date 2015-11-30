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

/** \file KniUncommon.cpp
 *
 * This file implements the less commonly used features of KNI. If the
 * MIDP code doesn't use any of these functions, then this object file
 * will not be linked, thus reducing static code footprint.
 */

#include "incls/_precompiled.incl"
#include "incls/_KniUncommon.cpp.incl"


extern "C" void* kni_get_java_class_statics(kjclass classHandle) {
  JavaClassObj::Raw mirror = kni_read_handle(classHandle);
  GUARANTEE(mirror.not_null(), "null argument to kni_get_java_class");

#if ENABLE_ISOLATES
  JavaClass::Raw jc = mirror().java_class();
  OopDesc* java_class = jc().real_task_mirror();
#else
  OopDesc* java_class = mirror().java_class();
#endif
  return java_class;
}

KNIEXPORT jboolean KNI_GetBooleanField(kjobject objectHandle, jfieldID fieldID)
{
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_GetBooleanField");
  return *object->bool_field_addr((int)fieldID);
}

KNIEXPORT jbyte KNI_GetByteField(kjobject objectHandle, jfieldID fieldID) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_GetByteField");
  return *object->byte_field_addr((int)fieldID);
}

KNIEXPORT jchar KNI_GetCharField(kjobject objectHandle, jfieldID fieldID) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_GetCharField");
  return *object->char_field_addr((int)fieldID);
}

KNIEXPORT jshort KNI_GetShortField(kjobject objectHandle, jfieldID fieldID) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_GetShortField");
  return *object->short_field_addr((int)fieldID);
}

KNIEXPORT jlong KNI_GetLongField(kjobject objectHandle, jfieldID fieldID) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_GetLongField");
  return *object->long_field_addr((int)fieldID);
}

#if ENABLE_FLOAT

KNIEXPORT jfloat KNI_GetFloatField(kjobject objectHandle, jfieldID fieldID) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_GetFloatField");
  return *object->float_field_addr((int)fieldID);
}

KNIEXPORT jdouble KNI_GetDoubleField(kjobject objectHandle, jfieldID fieldID) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_GetDoubleField");
  return *object->double_field_addr((int)fieldID);
}

#endif // ENABLE_FLOAT

KNIEXPORT void
KNI_SetBooleanField(kjobject objectHandle, jfieldID fieldID, jboolean value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetBooleanField");
  *object->bool_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetByteField(kjobject objectHandle, jfieldID fieldID, jbyte value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetByteField");
  *object->byte_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetCharField(kjobject objectHandle, jfieldID fieldID, jchar value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetCharField");
  *object->char_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetShortField(kjobject objectHandle, jfieldID fieldID, jshort value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetShortField");
  *object->short_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetIntField(kjobject objectHandle, jfieldID fieldID, jint value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetIntField");
  *object->int_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetLongField(kjobject objectHandle, jfieldID fieldID, jlong value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetLongField");
  *object->long_field_addr((int)fieldID) = value;
}

#if ENABLE_FLOAT

KNIEXPORT void
KNI_SetFloatField(kjobject objectHandle, jfieldID fieldID, jfloat value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetFloatField");
  *object->float_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetDoubleField(kjobject objectHandle, jfieldID fieldID, jdouble value) {
  OopDesc* object = kni_read_handle(objectHandle);
  GUARANTEE(object != 0, "null argument to KNI_SetDoubleField");
  *object->double_field_addr((int)fieldID) = value;
}

#endif // ENABLE_FLOAT

//
// Static field access
//
KNIEXPORT jfieldID KNI_GetStaticFieldID(kjclass classHandle, const char* name,
                                        const char* signature) {
  return _KNI_field_lookup_helper(classHandle, name, signature, true);
}

KNIEXPORT jboolean
KNI_GetStaticBooleanField(kjclass classHandle, jfieldID fieldID) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return (jboolean) (*static_vars->int_field_addr((int)fieldID));
}

KNIEXPORT jbyte KNI_GetStaticByteField(kjclass classHandle, jfieldID fieldID) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return (jbyte) (*static_vars->int_field_addr((int)fieldID));
}

KNIEXPORT jchar KNI_GetStaticCharField(kjclass classHandle, jfieldID fieldID) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return (jchar) (*static_vars->uint_field_addr((int)fieldID));
}

KNIEXPORT jshort KNI_GetStaticShortField(kjclass classHandle, jfieldID fieldID)
{
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return (jshort) (*static_vars->int_field_addr((int)fieldID));
}

KNIEXPORT jint KNI_GetStaticIntField(kjclass classHandle, jfieldID fieldID) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return *static_vars->int_field_addr((int)fieldID);
}

KNIEXPORT jlong KNI_GetStaticLongField(kjclass classHandle, jfieldID fieldID) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return *static_vars->long_field_addr((int)fieldID);
}

#if ENABLE_FLOAT

KNIEXPORT jfloat KNI_GetStaticFloatField(kjclass classHandle, jfieldID fieldID)
{
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return *static_vars->float_field_addr((int)fieldID);
}

KNIEXPORT jdouble
KNI_GetStaticDoubleField(kjclass classHandle, jfieldID fieldID) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  return *static_vars->double_field_addr((int)fieldID);
}

#endif // ENABLE_FLOAT

// IMPL_NOTE: to save footprint, all 8, 16 and 32 bit static field get/set
// functions can be aliased into a single C function -- small static
// fields are internally expanded to 32-bit
KNIEXPORT void
KNI_SetStaticBooleanField(kjclass classHandle, jfieldID fieldID, jboolean value)
{
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->int_field_addr((int)fieldID) = (jint)value;
}

KNIEXPORT void
KNI_SetStaticByteField(kjclass classHandle, jfieldID fieldID, jbyte value) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->int_field_addr((int)fieldID) = (jint)value;
}

KNIEXPORT void
KNI_SetStaticCharField(kjclass classHandle, jfieldID fieldID, jchar value) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->uint_field_addr((int)fieldID) = (juint)value;
}

KNIEXPORT void
KNI_SetStaticShortField(kjclass classHandle, jfieldID fieldID, jshort value) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->int_field_addr((int)fieldID) = (jint)value;
}

KNIEXPORT void
KNI_SetStaticIntField(kjclass classHandle, jfieldID fieldID, jint value) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->int_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetStaticLongField(kjclass classHandle, jfieldID fieldID, jlong value) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->long_field_addr((int)fieldID) = value;
}

#if ENABLE_FLOAT

KNIEXPORT void
KNI_SetStaticFloatField(kjclass classHandle, jfieldID fieldID, jfloat value) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->float_field_addr((int)fieldID) = value;
}

KNIEXPORT void
KNI_SetStaticDoubleField(kjclass classHandle, jfieldID fieldID, jdouble value) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  *static_vars->double_field_addr((int)fieldID) = value;
}

#endif // ENABLE_FLOAT

KNIEXPORT void KNI_GetStaticObjectField(kjclass classHandle, jfieldID fieldID,
                                        kjobject toHandle) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  kni_set_handle(toHandle, *static_vars->obj_field_addr((int)fieldID));
}

KNIEXPORT void KNI_SetStaticObjectField(kjclass classHandle, jfieldID fieldID,
                                        kjobject fromHandle) {
  OopDesc* static_vars = (OopDesc*)kni_get_java_class_statics(classHandle);
  oop_write_barrier(static_vars->obj_field_addr((int)fieldID),
                   kni_read_handle(fromHandle));
}
