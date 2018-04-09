LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_SRC_FILES := testuInput.c
LOCAL_SHARED_LIBRARIES := liblog

LOCAL_MODULE:= testuInput

include $(BUILD_EXECUTABLE)


