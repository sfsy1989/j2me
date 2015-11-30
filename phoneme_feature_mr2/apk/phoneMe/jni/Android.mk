# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MEHOME := ../..
LOCAL_MODULE    := FvJ2ME
LOCAL_SRC_FILES := fvj2me.c
LOCAL_LDFLAGS   := -Wl,--whole-archive $(MEHOME)/build_output/midp/bin/arm/libj2me.a $(MEHOME)/build_output/midp/obj/arm/libobj.a $(MEHOME)/build_output/cldc/linux_arm/dist/lib/libcldc_vm.a $(MEHOME)/build_output/pcsl/linux_arm/lib/libpcsl_file.a $(MEHOME)/build_output/pcsl/linux_arm/lib/libpcsl_memory.a $(MEHOME)/build_output/pcsl/linux_arm/lib/libpcsl_network.a $(MEHOME)/build_output/pcsl/linux_arm/lib/libpcsl_print.a $(MEHOME)/build_output/pcsl/linux_arm/lib/libpcsl_string.a -Wl,--no-whole-archive
LOCAL_STATIC_LIBRARIES := android_native_app_glue 
LOCAL_LDLIBS	:= -lm -lEGL -lGLESv2 -llog -landroid -lstdc++ -lz -lc $(MEHOME)/fvlib/freetype-2.4.8/target/lib/libfreetype.a  -L$(MEHOME)/build_output/javacall/lib -ljavacall -L$(MEHOME)/fvlib/syslib/lib -lflib -lolib -lnlib -liniparser -lcurl -lOpenSLES
LOCAL_CERTIFICATE := platform
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
