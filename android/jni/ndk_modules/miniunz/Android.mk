LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	ioapi.c \
	unzip.c \
	miniunz.c \

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)

LOCAL_MODULE:= miniunz
LOCAL_MODULE_FILENAME:= libminiunz

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
 
include $(BUILD_STATIC_LIBRARY)
