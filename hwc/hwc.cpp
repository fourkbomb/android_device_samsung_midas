#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LOG_TAG "lamecomposer"
#include <log/log.h>
#include <hardware/hardware.h>
#include <hardware/hwcomposer.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

struct hwc_context {
	struct hwc_composer_device_1 dev;
	hwc_procs_t const *procs;
	int fb;
	int32_t vsync_period;
	int32_t xres;
	int32_t yres;
	int32_t xdpi;
	int32_t ydpi;
};

static int hwc_prepare(struct hwc_composer_device_1 *dev,
		size_t numDisplays, hwc_display_contents_1_t **displays)
{
	for (size_t i = 0; i < displays[0]->numHwLayers; i++) {
		displays[0]->hwLayers[i].compositionType = HWC_FRAMEBUFFER;
	}

	return 0;
}

static int hwc_set(struct hwc_composer_device_1 *dev,
		size_t numDisplays, hwc_display_contents_1_t **displays)
{

	ALOGI("I should hwc_set but i'm not");
	return 0;
}

static int hwc_setPowerMode(struct hwc_composer_device_1 *dev, int disp,
		int mode)
{
	ALOGI("%s: %d -> %d", __func__, disp, mode);
	return 0;
}

static int hwc_query(struct hwc_composer_device_1 *dev, int what, int *value)
{
	switch (what) {
		case HWC_VSYNC_PERIOD:
			return -EINVAL;
		case HWC_BACKGROUND_LAYER_SUPPORTED:
			*value = 0;
			break;
		case HWC_DISPLAY_TYPES_SUPPORTED:
			*value = HWC_DISPLAY_PRIMARY_BIT;
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static void hwc_registerProcs(struct hwc_composer_device_1 *dev,
		hwc_procs_t const *procs)
{
	struct hwc_context *ctx = (struct hwc_context *)dev;
	ctx->procs = procs;
}

static int hwc_eventControl(struct hwc_composer_device_1 *dev,
		int disp, int event, int enabled)
{
	return 0;
}

static int hwc_getDisplayConfigs(struct hwc_composer_device_1 *dev,
		int disp, uint32_t *configs, size_t *numConfigs)
{
	if (*numConfigs < 1)
		return 0;

	if (disp == HWC_DISPLAY_PRIMARY) {
		configs[0] = 0;
		*numConfigs = 1;
		return 0;
	}

	return -EINVAL;
}

static int hwc_getDisplayAttributes(struct hwc_composer_device_1 *dev,
		int disp, uint32_t config, const uint32_t *attributes, int32_t *values)
{
	struct hwc_context *ctx = (struct hwc_context *)dev;
	int i = 0;
	if (disp != 0)
		return -EINVAL;

	while (attributes[i] != HWC_DISPLAY_NO_ATTRIBUTE) {
		switch (attributes[i]) {
		case HWC_DISPLAY_VSYNC_PERIOD:
			values[i] = ctx->vsync_period;
			break;
		case HWC_DISPLAY_WIDTH:
			values[i] = ctx->xres;
			break;
		case HWC_DISPLAY_HEIGHT:
			values[i] = ctx->yres;
			break;
		case HWC_DISPLAY_DPI_X:
			values[i] = ctx->xdpi;
			break;
		case HWC_DISPLAY_DPI_Y:
			values[i] = ctx->ydpi;
			break;
		default:
			return -EINVAL;
		}
		i++;
	}
	return 0;
}

static int hwc_device_close(struct hw_device_t *dev)
{
	struct hwc_context *hwc = (struct hwc_context *)dev;
	free(hwc);
	return 0;
}

static int hwc_open(const struct hw_module_t *module, const char *name,
		struct hw_device_t **device)
{
	struct hwc_context *dev;
	dev = (struct hwc_context *)malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	memset(dev, 0, sizeof(*dev));

	dev->dev.common.tag = HARDWARE_DEVICE_TAG;
	dev->dev.common.version = HWC_DEVICE_API_VERSION_1_3;
	dev->dev.common.module = const_cast<hw_module_t*>(module);
	dev->dev.common.close = hwc_device_close;

	dev->dev.prepare = hwc_prepare;
	dev->dev.set = hwc_set;
	dev->dev.eventControl = hwc_eventControl;
	dev->dev.setPowerMode = hwc_setPowerMode;
	dev->dev.query = hwc_query;
	dev->dev.registerProcs = hwc_registerProcs;
	dev->dev.getDisplayConfigs = hwc_getDisplayConfigs;
	dev->dev.getDisplayAttributes = hwc_getDisplayAttributes;

	*device = &dev->dev.common;

	dev->fb = open("/dev/graphics/fb0", O_RDWR);
	if (dev->fb < 0) {
		ALOGE("%s: failed to open FB", __func__);
		return -EINVAL;
	}

	struct fb_var_screeninfo lcdinfo;

	if (ioctl(dev->fb, FBIOGET_VSCREENINFO, &lcdinfo) < 0) {
		ALOGE("%s: failed to get vscreeninfo", __func__);
		return -EINVAL;
	}

	ALOGI("%s: %d %d %d %d %d %d %d", __func__,lcdinfo.width, lcdinfo.height, lcdinfo.yres,
			lcdinfo.left_margin ,lcdinfo.right_margin, lcdinfo.xres, lcdinfo.pixclock);
/*	int refreshRate = 1000000000000LLU /
		(
		 uint64_t( lcdinfo.upper_margin + lcdinfo.lower_margin + lcdinfo.yres)
		 * ( lcdinfo.left_margin  + lcdinfo.right_margin + lcdinfo.xres)
		 * lcdinfo.pixclock
		);
*/
	int refreshRate = 60;

	dev->vsync_period = 1000000000UL / refreshRate;
	dev->xres = lcdinfo.xres;
	dev->yres = lcdinfo.yres;
	dev->xdpi = (lcdinfo.xres * 25.4f * 1000.0f) / lcdinfo.width;
	dev->ydpi = (lcdinfo.yres * 25.4f * 1000.0f) / lcdinfo.height;

	return 0;
}

static struct hw_module_methods_t hwc_module_methods = {
	.open = hwc_open,
};

hwc_module_t HAL_MODULE_INFO_SYM = {
	.common = {
		.tag = HARDWARE_MODULE_TAG,
		.version_major = 1,
		.version_minor = 0,
		.id = HWC_HARDWARE_MODULE_ID,
		.name = "Dummy hwcomposer",
		.author = "Simon Shields <simon@lineageos.org>",
		.methods = &hwc_module_methods,
	},
};
