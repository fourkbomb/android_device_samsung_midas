#
# Copyright 2014 The Android Open-Source Project
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

# Use the non-open-source parts, if they're present
-include vendor/samsung/midas/BoardConfigVendor.mk

LOCAL_PATH := device/samsung/midas

# Architecture
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_VARIANT := cortex-a9
ARCH_ARM_HAVE_NEON := true

# Binder
TARGET_USES_64_BIT_BINDER := true

# Filesystem
BOARD_NAND_PAGE_SIZE := 4096
BOARD_NAND_SPARE_SIZE := 128
BOARD_BOOTIMAGE_PARTITION_SIZE := 25165824
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 25165824
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1073741824
BOARD_USERDATAIMAGE_PARTITION_SIZE := 13417558016
BOARD_FLASH_BLOCK_SIZE := 4096
TARGET_USERIMAGES_USE_EXT4 := true

BOARD_VENDORIMAGE_PARTITION_SIZE := 1073741824
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_USES_VENDORIMAGE := true
TARGET_COPY_OUT_VENDOR := vendor

# Kernel
SUPPORTED_BOARDS := i9300 i9305 n7100 n7105
BOARD_AUTO_DT_OVERLAYS := midas-android
BOARD_DT_OVERLAYS := n710x-ea8061 n710x-s6evr02
BOARD_CUSTOM_MKBOOTIMG = $(HOST_OUT_EXECUTABLES)/mkfitimage$(HOST_EXECUTABLE_SUFFIX)
BOARD_MKBOOTIMG_ARGS = $(foreach dtb,$(strip $(SUPPORTED_BOARDS)),--dtb $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/arch/arm/boot/dts/exynos4412-$(dtb).dtb) \
					   $(foreach dtb,$(strip $(BOARD_AUTO_DT_OVERLAYS)),--dtbo-auto $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/arch/arm/boot/dts/overlay/exynos4412-$(dtb).dtbo) \
					   $(foreach dtb,$(strip $(BOARD_DT_OVERLAYS)),--dtbo $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/arch/arm/boot/dts/overlay/exynos4412-$(dtb).dtbo)
BOARD_KERNEL_BASE := 0x40000000
BOARD_KERNEL_CMDLINE := console=ttySAC2,115200 androidboot.hardware=midas androidboot.selinux=permissive
BOARD_KERNEL_PAGESIZE := 2048
BOARD_KERNEL_IMAGE_NAME := zImage
TARGET_KERNEL_SOURCE := kernel/samsung/midas
TARGET_KERNEL_CONFIG := midas_android_defconfig
NEED_KERNEL_MODULE_ROOT := true

# Mesa
BOARD_GPU_DRIVERS := exynos lima swrast

# Platform
TARGET_BOARD_PLATFORM := exynos4
TARGET_SOC := exynos4412
TARGET_BOOTLOADER_BOARD_NAME := midas

TARGET_NO_BOOTLOADER := true

# Recovery
TARGET_RECOVERY_FSTAB := $(LOCAL_PATH)/rootdir/fstab.midas
TARGET_RECOVERY_PIXEL_FORMAT := BGRA_8888
TARGET_RECOVERY_BACKLIGHT_PATH := /sys/class/backlight/panel
TARGET_RECOVERY_UI_BRIGHTNESS_FILE := /sys/class/backlight/panel/brightness
TARGET_RECOVERY_UI_MAX_BRIGHTNESS_FILE := /sys/class/backlight/panel/max_brightness
ifeq ($(WITH_TWRP),true)
include $(LOCAL_PATH)/twrp.mk
endif

# Treble
PRODUCT_FULL_TREBLE_OVERRIDE := true
# FIXME: mesa/gbm-gralloc (which are SP-HALs) depend on libexpat (which is non-SP only)
BOARD_VNDK_RUNTIME_DISABLE := true
BOARD_VNDK_VERSION := current
