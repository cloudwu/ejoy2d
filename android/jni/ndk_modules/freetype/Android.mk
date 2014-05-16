# this is now the default FreeType build for Android
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libfreetype

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#
LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES:= \
	src/base/ftbbox.c \
	src/base/ftbitmap.c \
	src/base/ftglyph.c \
	src/base/ftstroke.c \
	src/base/ftxf86.c \
	src/base/ftbase.c \
	src/base/ftsystem.c \
	src/base/ftinit.c \
	src/base/ftgasp.c \
	src/raster/raster.c \
	src/sfnt/sfnt.c \
	src/smooth/smooth.c \
	src/autofit/autofit.c \
	src/truetype/truetype.c \
	src/cff/cff.c \
	src/psnames/psnames.c \
	src/pshinter/pshinter.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/builds \
	$(LOCAL_PATH)/include

LOCAL_MODULE:= freetype
LOCAL_MODULE_FILENAME:= libfreetype

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/builds \
	$(LOCAL_PATH)/include

#LOCAL_CFLAGS += -W -Wall
#LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"
#LOCAL_CFLAGS += -O2

include $(BUILD_STATIC_LIBRARY)
