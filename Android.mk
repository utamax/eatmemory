LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= eatmemory.c
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_MODULE_TAGS := debug
LOCAL_MODULE:= eatmemory

include $(BUILD_EXECUTABLE)

