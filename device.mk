#
# Copyright 2017 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := device/samsung/midas

# Graphics
PRODUCT_PACKAGES += \
	android.hardware.graphics.allocator@2.0-impl \
	android.hardware.graphics.composer@2.1-impl \
	android.hardware.graphics.mapper@2.0-impl \
	gralloc.gbm \
	hwcomposer.midas \
	libGLES_mesa \
	libstdc++.vendor

# FIXME: remove this.
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/prebuilt/libexpat.so:vendor/lib/libexpat.so

# Init
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/fstab.midas:root/fstab.midas \
    $(LOCAL_PATH)/rootdir/init.midas.rc:root/init.midas.rc \
    $(LOCAL_PATH)/rootdir/init.midas.usb.rc:root/init.midas.usb.rc \
	$(LOCAL_PATH)/recovery/init.recovery.midas.rc:root/init.recovery.midas.rc

# Screen
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xhdpi
TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720

# Treble
PRODUCT_PACKAGES += \
	vndk_package

DEVICE_MANIFEST_FILE := $(LOCAL_PATH)/manifest.xml

$(call inherit-product-if-exists, vendor/samsung/midas/device-vendor.mk)
