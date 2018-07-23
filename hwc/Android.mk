LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hwcomposer.midas
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := hwc.cpp

LOCAL_SHARED_LIBRARIES := libhardware liblog libcutils

include $(BUILD_SHARED_LIBRARY)
