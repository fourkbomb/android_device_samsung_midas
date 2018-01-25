TW_HAS_NO_RECOVERY_PARTITION := true
TW_HAS_NO_BOOT_PARTITION := true
TW_NO_LEGACY_PROPS := true

TARGET_RECOVERY_DEVICE_DIRS += $(LOCAL_PATH)/twrp

TW_BRIGHTNESS_PATH := /sys/class/backlight/panel/brightness
TW_MAX_BRIGHTNESS := 32

# use max77693-haptic device for vibration feedback
TW_VIBRATOR_INPUT_DEV := /dev/input/event0

TARGET_RECOVERY_INITRC := $(LOCAL_PATH)/twrp/recovery/root/init.recovery.midas.rc
