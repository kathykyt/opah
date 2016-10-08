LOCAL_PATH:=$(call my-dir)
TARGET_NAME  := sample1
PROJECT_NAME := project/$(TARGET_NAME)



NDK_ROOT := $(PWD)
ANDROID_BUILD_TOP := $(NDK_ROOT)/samples/opah/jni/lib
#PROJECT_ROOT := $(PWD)
PROJECT_ROOT := $(PWD)/samples/opah/jni

# This two variable must be set exclusive, one to be true another must be false and vice verse. ARGB is for the real device.
OUTPUT_FRAMEBUFFER_ARGB := false
OUTPUT_FRAMEBUFFER_RGB565 := true

INPUT_MODEL:= GOLDFISH

#************************Freetype2****************************
include $(CLEAR_VARS)
LOCAL_MODULE := libFreeType2-static
LOCAL_SRC_FILES := $(PROJECT_ROOT)/../freetype2-android-master/Android/obj/local/armeabi/libfreetype2-static.a
include $(PREBUILT_STATIC_LIBRARY)


#************************libBmpDataArray.so****************************
include $(CLEAR_VARS)
define all-pic-c-files-under-project
$(patsubst ./%,%, $(shell find $(LOCAL_PATH)/$(PROJECT_NAME)/out/BmpDataArray -name "*.c"))  
endef 
C_PICFILE_LIST := $(call all-pic-c-files-under-project)
LOCAL_SRC_FILES := $(C_PICFILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_MODULE := libImageData
include $(BUILD_SHARED_LIBRARY)


#************************demo****************************
include $(CLEAR_VARS) 




ifeq ($(OUTPUT_FRAMEBUFFER_ARGB),true)
LOCAL_MODULE := $(TARGET_NAME)
else
LOCAL_MODULE := $(TARGET_NAME)_rgb565
endif


LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE:= true

# to rotate LCD by 270, add option -DLCD_ROTATE_270 and set -DLCDW=272 -DLCDH=480

LOCAL_CFLAGS += -g -D$(INPUT_MODEL) -DCOLORPIXEL565=0 -DLCDBUFFER=2 -DI2CSUPPORT=0 -DVIDEOSUPPORT=0 -DSERIALSUPPORT=0 -DSLEEPSUPPORT=0 -DDEBUGSUPPORT=0 -DLCDW=480 -DLCDH=272 -I$(PROJECT_ROOT)/lib -I$(PROJECT_ROOT)/../freetype2-android-master/include

define all-cpp-files-under-project
$(patsubst ./%,%, $(shell find $(LOCAL_PATH)/$(PROJECT_NAME) -maxdepth 1 -name "*.cpp"))  
endef
define all-cpp-files-under-lib
$(patsubst ./%,%, $(shell find $(LOCAL_PATH)/lib -name "*.cpp"))  
endef
define all-c-files-under-project
$(patsubst ./%,%, $(shell find $(LOCAL_PATH)/$(PROJECT_NAME) -maxdepth 1 -name "*.c"))  
endef 
define all-c-files-under-lib
$(patsubst ./%,%, $(shell find $(LOCAL_PATH)/lib -name "*.c"))  
endef 
define all-h-dir-under-project
$(sort $(patsubst ./%,%, $(dir $(shell find $(LOCAL_PATH)/$(PROJECT_NAME -maxdepth 1 -name "*.h")))))
endef
define all-h-dir-under-lib
$(sort $(patsubst ./%,%, $(dir $(shell find $(LOCAL_PATH)/lib -name "*.h"))))
endef

CPP_FILE_LIST := $(call all-cpp-files-under-lib)
CPP_FILE_LIST += $(call all-cpp-files-under-project)
C_FILE_LIST := $(call all-c-files-under-lib)
C_FILE_LIST += $(call all-c-files-under-project)
H_DIR_LIST := $(call all-h-dir-under-lib)
H_DIR_LIST += $(call all-h-dir-under-project)
LOCAL_SRC_FILES := $(CPP_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(C_FILE_LIST:$(LOCAL_PATH)/%=%)

ifeq ($(OUTPUT_FRAMEBUFFER_ARGB),true)
LOCAL_SRC_FILES += $(PROJECT_ROOT)/gdi/devdraw.cpp $(PROJECT_ROOT)/gdi/screen.cpp
else 
LOCAL_CFLAGS += -DOUTPUT_RGB565_FB
LOCAL_SRC_FILES += $(PROJECT_ROOT)/gdi/devdraw_rgb565.cpp  $(PROJECT_ROOT)/gdi/screen.cpp
endif


LOCAL_C_INCLUDES += $(H_DIR_LIST:%/=$(PROJECT_ROOT)/lib/%)  \
	$(PROJECT_ROOT)/lib/log $(PROJECT_ROOT)/lib/memory  $(PROJECT_ROOT)/lib \
	$(PROJECT_ROOT)/lib/2Dctrl $(PROJECT_ROOT)/lib/draw $(PROJECT_ROOT)/gdi $(PROJECT_ROOT)/lib/key $(PROJECT_ROOT)/lib/msg \
	$(PROJECT_ROOT)/lib/pic $(PROJECT_ROOT)/lib/specialeffect $(PROJECT_ROOT)/lib/timer $(PROJECT_ROOT)/lib/ts \
	$(PROJECT_ROOT)/lib/sensor \
	$(PROJECT_ROOT)/lib/pm \
	$(PROJECT_ROOT)/lib/wnd  $(PROJECT_ROOT)/$(PROJECT_NAME)  \
	$(NDK_ROOT)/platforms/android-19/arch-arm/usr/include/android \
	$(PROJECT_ROOT)/inc\
	$(PROJECT_ROOT)/lib/connectivity \
	$(PROJECT_ROOT)/include \
	$(PROJECT_ROOT)/../freetype2-android-master/jni/lib \
	$(PROJECT_ROOT)/lib/curl \
	$(PROJECT_ROOT)/lib/Cjson \
	$(PROJECT_ROOT)/../openssl \
	$(PROJECT_ROOT)/lib/fsk/



LOCAL_LDFLAGS += -L$(PROJECT_ROOT)/libs -L$(ANDROID_BUILD_TOP)/../../libs/armeabi/ -L$(NDK_ROOT)/platforms/android-19/arch-arm/usr/lib  \
	-lImageData -lhardware_legacy  \
	$(PROJECT_ROOT)/../freetype2-android-master/Android/obj/local/armeabi/libfreetype2-static.a \
	$(PROJECT_ROOT)/lib/curl/libcurl.so -lcr8 -lcrypto -llog
	 

LOCAL_SHARED_LIBRARIES := libcr8 libjpeg libcutils libImageData libhardware_legacy_ct3 libcrypto

LOCAL_STATIC_LIBRARIES += FreeType2

LOCAL_LDFLAGS += -lImageData


include $(BUILD_EXECUTABLE)




