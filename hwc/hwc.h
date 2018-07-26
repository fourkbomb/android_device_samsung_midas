#ifndef _MIDAS_HWC_H
#define _MIDAS_HWC_H
#include <hardware/hardware.h>
#include <hardware/hwcomposer.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

struct hwc_drm_info;
struct gralloc_handle_t;

struct hwc_context {
	struct hwc_composer_device_1 dev;
	hwc_procs_t const *procs;
	struct hwc_drm_info *drm;
	int fb;
	/* TODO: derive this from DRM, not legacy FB API */
	int32_t vsync_period;
	int32_t xres;
	int32_t yres;
	int32_t xdpi;
	int32_t ydpi;
};


int hwc_drm_init(struct hwc_context *ctx);
int hwc_drm_show(struct hwc_context *ctx, struct gralloc_handle_t *handle, int acquireFD);
#endif /* _MIDAS_HWC_H */
