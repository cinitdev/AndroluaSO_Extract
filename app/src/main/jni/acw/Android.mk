LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../lua
LOCAL_MODULE := acw
LOCAL_SRC_FILES := lua_acw.c
LOCAL_LDFLAGS += -Wl,--strip-all

LOCAL_STATIC_LIBRARIES := luajava
include $(BUILD_SHARED_LIBRARY)