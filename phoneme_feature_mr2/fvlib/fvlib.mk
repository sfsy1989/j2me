CFLAGS+=-I$(MEHOME)/fvlib/syslib/olib/include/ \
        -I$(MEHOME)/fvlib/syslib/nlib/include/ \
        -I$(MEHOME)/fvlib/syslib/flib/include/ \
        -I$(MEHOME)/fvlib/syslib/opensource/iniparser/
EXTRA_LDFLAGS+=-L$(MEHOME)/fvlib/syslib/lib/

ifeq ($(NDK_COMPILER), true)
#CFLAGS+=--sysroot=$(NDK)/platforms/android-9/arch-arm 
#EXTRA_LDFLAGS+=--sysroot=$(NDK)/platforms/android-9/arch-arm 
endif

