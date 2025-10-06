LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../lua
LOCAL_MODULE := duktape
LOCAL_SRC_FILES := lua_duktape.c duktape.c
LOCAL_STATIC_LIBRARIES := luajava

include $(BUILD_SHARED_LIBRARY)

