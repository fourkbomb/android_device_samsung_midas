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

# DTBs
TARGET_DTBS := \
	exynos4412-i9300.dtb \
	exynos4412-i9305.dtb \
	exynos4412-n710x.dtb

TARGET_DTB_OVERLAYS := \
	exynos4412-midas-android.dtb \
	exynos4412-t0-ea8061.dtb \
	exynos4412-t0-s6evr02.dtb

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/boot/config.ini:install/bootdata/config.ini

# EGL
PRODUCT_PACKAGES += \
    libGLES_android

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

$(call inherit-product-if-exists, vendor/samsung/midas/device-vendor.mk)
