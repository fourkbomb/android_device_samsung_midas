CUSTOM_IMAGE_MOUNT_POINT := bootdata
CUSTOM_IMAGE_PARTITION_SIZE := 587202560
CUSTOM_IMAGE_FILE_SYSTEM_TYPE := ext4

KERNEL_DTBS_PATH := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/arch/arm/boot/dts

CUSTOM_IMAGE_COPY_FILES := \
	$(INSTALLED_BOOTIMAGE_TARGET):boot.img \
	$(INSTALLED_RECOVERYIMAGE_TARGET):recovery.img \
	device/samsung/midas/boot/config.ini:config.ini

# copy all generated dtbs + dtb overlays
$(foreach dtb,$(wildcard $(KERNEL_DTBS_PATH)/*.dtb),$(eval CUSTOM_IMAGE_COPY_FILES += $(dtb):dtbs/$(notdir $(dtb))))
$(foreach dtb,$(wildcard $(KERNEL_DTBS_PATH)/overlay/*.dtbo),$(eval CUSTOM_IMAGE_COPY_FILES += $(dtb):dtbs/overlay/$(notdir $(dtb))))

