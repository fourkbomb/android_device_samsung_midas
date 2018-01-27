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

# Filesystem
BOARD_NAND_PAGE_SIZE := 4096
BOARD_NAND_SPARE_SIZE := 128
BOARD_BOOTIMAGE_PARTITION_SIZE := 8388608
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 83886080
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1610612736
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12381585408
BOARD_FLASH_BLOCK_SIZE := 4096
TARGET_USERIMAGES_USE_EXT4 := true

BOARD_VENDORIMAGE_PARTITION_SIZE := 1075838976
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_USES_VENDORIMAGE := true
TARGET_COPY_OUT_VENDOR := vendor

# Kernel
BOARD_KERNEL_BASE := 0x40000000
BOARD_KERNEL_CMDLINE := console=ttySAC2,115200 androidboot.hardware=midas androidboot.selinux=permissive
BOARD_KERNEL_PAGESIZE := 2048
BOARD_KERNEL_IMAGE_NAME := zImage
TARGET_KERNEL_SOURCE := kernel/samsung/i9300
TARGET_KERNEL_CONFIG := midas_android_defconfig
TARGET_KERNEL_DTBS_DIR := install/bootdata/dtbs
NEED_KERNEL_MODULE_ROOT := true

# Mesa
BOARD_GPU_DRIVERS := swrast

# Platform
TARGET_BOARD_PLATFORM := exynos4
TARGET_SOC := exynos4412
TARGET_BOOTLOADER_BOARD_NAME := midas

TARGET_NO_BOOTLOADER := true

# Releasetools
TARGET_RELEASETOOLS_EXTENSIONS := $(LOCAL_PATH)/releasetools

# Recovery
TARGET_RECOVERY_FSTAB := $(LOCAL_PATH)/rootdir/fstab.midas
TARGET_RECOVERY_PIXEL_FORMAT :=  BGRA_8888
TARGET_RECOVERY_UI_BRIGHTNESS_FILE := /sys/class/backlight/panel/brightness
TARGET_RECOVERY_UI_MAX_BRIGHTNESS_FILE := /sys/class/backlight/panel/max_brightness
ifeq ($(WITH_TWRP),true)
include $(LOCAL_PATH)/twrp.mk
endif

# Treble
PRODUCT_FULL_TREBLE_OVERRIDE := true
BOARD_VNDK_VERSION := current
