LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ndk-skeleton
LOCAL_CFLAGS=-std=gnu99 -W -Wall -Wextra -Werror
LOCAL_CFLAGS+=-g
LOCAL_SRC_FILES=main.c
LOCAL_LDLIBS=-landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES := android_native_app_glue testmod

include $(BUILD_SHARED_LIBRARY)

$(call import-module,testmod)
$(call import-module,android/native_app_glue)
