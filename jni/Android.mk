LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION:=.cpp
LOCAL_MODULE    := PcaBpnnHandwrittenDigitsRecog
LOCAL_SRC_FILES := PatternRecognition.cpp
include $(BUILD_SHARED_LIBRARY)