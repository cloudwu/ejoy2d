LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := native-activity
LOCAL_SRC_FILES := main.c winfont.c winfw.c \
	../../lib/shader.c \
	../../lib/lshader.c \
	../../lib/ejoy2dgame.c \
	../../lib/fault.c \
	../../lib/screen.c \
	../../lib/texture.c \
	../../lib/ppm.c \
	../../lib/spritepack.c \
	../../lib/sprite.c \
	../../lib/lsprite.c \
	../../lib/matrix.c \
	../../lib/lmatrix.c \
	../../lib/dfont.c \
	../../lib/label.c \
	../../lib/particle.c \
	../../lib/lparticle.c \
	../../lib/scissor.c

LOCAL_SRC_FILES += \
	../../lua/lapi.c \
	../../lua/lauxlib.c \
	../../lua/lbaselib.c \
	../../lua/lbitlib.c \
	../../lua/lcode.c \
	../../lua/lcorolib.c \
	../../lua/lctype.c \
	../../lua/ldblib.c \
	../../lua/ldebug.c \
	../../lua/ldo.c \
	../../lua/ldump.c \
	../../lua/lfunc.c \
	../../lua/lgc.c \
	../../lua/linit.c \
	../../lua/liolib.c \
	../../lua/llex.c \
	../../lua/lmathlib.c \
	../../lua/lmem.c \
	../../lua/loadlib.c \
	../../lua/lobject.c \
	../../lua/lopcodes.c \
	../../lua/loslib.c \
	../../lua/lparser.c \
	../../lua/lstate.c \
	../../lua/lstring.c \
	../../lua/lstrlib.c \
	../../lua/ltable.c \
	../../lua/ltablib.c \
	../../lua/ltm.c \
	../../lua/lundump.c \
	../../lua/lvm.c \
	../../lua/lzio.c

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/../../lib \
	$(LOCAL_PATH)/../../lua


LOCAL_LDLIBS  := -llog -landroid -lEGL -lGLESv2 -lz
LOCAL_STATIC_LIBRARIES := android_native_app_glue \
	freetype \
	miniunz

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,freetype)
$(call import-module,miniunz)

