LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie
LOCAL_SRC_FILES := event-player.c
LOCAL_MODULE := event-player
include $(BUILD_EXECUTABLE)
