// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * Based on code from U-Boot:
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2009
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 */
/* TODO: add checksum support */
#include <errno.h>
#include <fcntl.h>
#include <libfdt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ARCH "arm"
#define OS "linux"
#define KERNEL_NAME "kernel"
#define RAMDISK_NAME "ramdisk"

/* Represents a single device tree blob */
struct dtb {
	/* Is this an overlay? */
	bool is_overlay;
	/* If overlay: will this be loaded for all DTBs, or selected for some by the bootloader? */
	bool autoload;
	/* Path to DTB */
	char *path;
	/* Name of DTB node */
	char *node_name;
	/* Next DTB in linked list */
	struct dtb *next;
};

struct params {
	char *kernel_path;
	char *ramdisk_path;
	char *output_path;
	struct dtb *dtbs;
	uint32_t kernel_load;
	uint32_t dtb_load;
	uint32_t dtbo_load;
	char *os_version;
	char *patch_level;
	char *cmdline;
};

static int get_filesize(const char *file)
{
	struct stat buf;
	int ret;

	int fd = open(file, O_RDONLY);
	if (fd < 0) {
		printf("Failed to open '%s': %s\n", file, strerror(errno));
		return -1;
	}

	if (fstat(fd, &buf) < 0) {
		printf("Failed to stat '%s': %s\n", file, strerror(errno));
		ret = -1;
		goto error;
	}

	ret = buf.st_size;

error:
	close(fd);
	return ret;
}

static int fdt_property_file(void *fdt, const char *name,
		const char *file)
{
	int fd, ret;
	void *ptr;
	struct stat sbuf;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		printf("Failed to open '%s': %s\n", file, strerror(errno));
		return -1;
	}

	if (fstat(fd, &sbuf) < 0) {
		printf("Failed to stat '%s': %s\n", file, strerror(errno));
		goto error;
	}

	ptr = malloc(sbuf.st_size);
	if (!ptr) {
		printf("Can't allocate buffer with size=%d\n", sbuf.st_size);
		goto error;
	}

	ret = read(fd, ptr, sbuf.st_size);
	if (ret != sbuf.st_size) {
		printf("Can't read %s: %s\n", file, strerror(errno));
		goto error_malloc;
	}

	ret = fdt_property(fdt, name, ptr, sbuf.st_size);
	if (ret)
		goto error_malloc;

	free(ptr);
	close(fd);
	return 0;
error_malloc:
	free(ptr);
error:
	close(fd);
	return -1;
}

static int fit_write_images(struct params *params, char *fdt)
{
	char str[100];
	int ret;
	struct dtb *dtb;

	fdt_begin_node(fdt, "images");
	/* Add the kernel */
	fdt_begin_node(fdt, KERNEL_NAME);
	fdt_property_string(fdt, "description", KERNEL_NAME);
	fdt_property_string(fdt, "type", "kernel");
	fdt_property_string(fdt, "arch", ARCH);
	fdt_property_string(fdt, "os", OS);
	fdt_property_string(fdt, "compression", "none");
	fdt_property_u32(fdt, "load", params->kernel_load);
	fdt_property_u32(fdt, "entry", params->kernel_load);

	ret = fdt_property_file(fdt, "data", params->kernel_path);
	if (ret)
		return ret;
	fdt_end_node(fdt);

	/* next, do the ramdisk - the kernel should have decompression support */
	fdt_begin_node(fdt, RAMDISK_NAME);
	fdt_property_string(fdt, "description", RAMDISK_NAME);
	fdt_property_string(fdt, "type", "ramdisk");
	fdt_property_string(fdt, "arch", ARCH);
	fdt_property_string(fdt, "os", OS);
	fdt_property_string(fdt, "compression", "none");

	ret = fdt_property_file(fdt, "data", params->ramdisk_path);
	if (ret)
		return ret;
	fdt_end_node(fdt);

	/* Do all the DTBs. */
	for (dtb = params->dtbs; dtb != NULL; dtb = dtb->next) {
		fdt_begin_node(fdt, dtb->node_name);
		fdt_property_string(fdt, "description", dtb->node_name);
		fdt_property_string(fdt, "type", "flat_dt");
		fdt_property_string(fdt, "arch", ARCH);
		fdt_property_string(fdt, "compression", "none");
		if (dtb->is_overlay)
			fdt_property_u32(fdt, "load", params->dtbo_load);
		else
			fdt_property_u32(fdt, "load", params->dtb_load);

		ret = fdt_property_file(fdt, "data", dtb->path);
		if (ret)
			return ret;
		fdt_end_node(fdt);
	}

	fdt_end_node(fdt);	
	return 0;
}

static int estimate_size(struct params *params)
{
	int size, total = 0;
	struct dtb *dtb;

	size = get_filesize(params->kernel_path);
	if (size < 0)
		return size;
	total += size;

	size = get_filesize(params->ramdisk_path);
	if (size < 0)
		return size;
	total += size;

	for (dtb = params->dtbs; dtb != NULL; dtb = dtb->next) {
		size = get_filesize(dtb->path);
		if (size < 0)
			return size;
		total += size + 300;
	}

	total += 4096;

	return total;
}

static int add_fit_configs(struct params *params, void *fdt)
{
	struct dtb *cur = params->dtbs;
	char autoload_buf[128];
	char *next = autoload_buf;
	int size_left = sizeof(autoload_buf);
	int autoload_size = 0;

	memset(autoload_buf, 0, sizeof(autoload_buf));

	/* Figure out overlays to add: a string array in DTS is null-separated */
	while (cur != NULL) {
		if (cur->is_overlay && cur->autoload) {
			strncpy(next, cur->node_name, size_left);
			int amount = strlen(cur->node_name) + 1;
			autoload_size += amount;
			size_left -= amount;
			if (size_left < 0)
				return -1;
			next += amount;
		}
		cur = cur->next;
	}
	
	fdt_begin_node(fdt, "configurations");

	cur = params->dtbs;
	while (cur != NULL) {
		if (cur->is_overlay && cur->autoload) {
			cur = cur->next;
			continue;
		}

		fdt_begin_node(fdt, cur->node_name);
		fdt_property_string(fdt, "description", cur->node_name);
		if (!cur->is_overlay) {
			fdt_property_string(fdt, "kernel", KERNEL_NAME);
			fdt_property_string(fdt, "ramdisk", RAMDISK_NAME);
			size_t size = autoload_size + strlen(cur->node_name) + 1;
			char *buf = malloc(size);
			strcpy(buf, cur->node_name);
			if (autoload_size)
				memcpy(&buf[strlen(cur->node_name) + 1], autoload_buf, autoload_size);
			/* can't use fdt_property_string because of the null terminators */
			fdt_property(fdt, "fdt", buf, size);
		} else
			fdt_property_string(fdt, "fdt", cur->node_name);
		fdt_end_node(fdt);
		cur = cur->next;
	}
	fdt_end_node(fdt);
	return 0;
}

static int make_fdt(struct params *params, char *fdt, int size)
{
	int ret;
	char buf[100];

	ret = fdt_create(fdt, size);
	if (ret)
		return ret;
	fdt_finish_reservemap(fdt);
	fdt_begin_node(fdt, "");
	snprintf(buf, sizeof(buf), "Android %s image, patch level %s", params->os_version, params->patch_level);
	fdt_property_string(fdt, "description", buf);
	fdt_property_string(fdt, "creator", "mkfitimage");
	if (params->cmdline)
		fdt_property_string(fdt, "cmdline", params->cmdline);
	fdt_property_u32(fdt, "#address-cells", 1);
	ret = fit_write_images(params, fdt);
	if (ret)
		return ret;
	add_fit_configs(params, fdt);
	fdt_end_node(fdt);
	ret = fdt_finish(fdt);
	if (ret)
		return ret;

	return fdt_totalsize(fdt);
}

static int add_dtb(struct params *params, bool overlay, bool autoload, char *path)
{
	char *end, *filename;
	struct dtb *new = calloc(1, sizeof(*new));
	if (!new)
		return -1;

	new->is_overlay = overlay;
	new->autoload = autoload;
	new->path = path;

	/* figure out node name */
	end = strrchr(new->path, '.');

	if (end)
		*end = '\0';
	filename = strrchr(new->path, '/');
	if (!filename)
		filename = new->path;
	else
		filename++;

	new->node_name = strdup(filename);

	if (end)
		*end = '.';

	/* prepend to list */
	new->next = params->dtbs;
	params->dtbs = new;
	return 0;
}

static int usage(void)
{
	printf("Usage: mkfitimage --output <FILE> --kernel <FILE> --ramdisk <FILE> --dtb <FILE> [--dtb <FILE>]...\n");
	printf("Arguments:\n"
			"	--output <FILE>		- FIT output file\n"
			"	--kernel <FILE>		- kernel zImage\n"
			"	--ramdisk <FILE>	- kernel ramdisk\n"
			"	--cmdline \"string\"	- kernel cmdline\n"
			"	--base <address>	- kernel load address, in hex\n"
			"	--dtb-base <address>	- dtb load address, in hex\n"
			"	--dtbo-base <address>	- dtbo load address, in hex\n"
			"Following options can be specified multiple times:\n"
			"	--dtb <FILE>		- base DTB for board\n"
			"	--dtbo <FILE>		- DTB overlay to be included just in case bootloader decides to apply it\n"
			"	--dtbo-auto <FILE>	- DTB overlay to be applied to all base DTBs by u-boot\n");
	return 2;
}

int main(int argc, char *argv[])
{
	struct params params;
	struct dtb *curdtb;
	char *buf;
	int fd, size, ret;
	memset(&params, 0, sizeof(params));

	params.kernel_load = 0x40000000;
	params.dtb_load = 0x60000000;
	params.dtbo_load = 0x6fc00000;
/*
	for (int i = 0; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
*/
	argc--;
	argv++;
	while (argc > 0) {
		char *arg = argv[0];
		argc--;
		argv++;
		if (!strcmp(arg, "--id"))
			continue;
		if (argc >= 1) {
			char *val = argv[0];
			argc--;
			argv++;
			if (!strcmp(arg, "--output")) {
				params.output_path = val;
			} else if (!strcmp(arg, "--ramdisk")) {
				params.ramdisk_path = val;
			} else if (!strcmp(arg, "--kernel")) {
				params.kernel_path = val;
			} else if (!strcmp(arg, "--cmdline")) {
				params.cmdline = val;
			} else if (!strcmp(arg, "--base")) {
				params.kernel_load = strtoul(val, 0, 16);
			} else if (!strcmp(arg, "--dtb-load")) {
				params.dtb_load = strtoul(val, 0, 16);
			} else if (!strcmp(arg, "--dtbo-load")) {
				params.dtbo_load = strtoul(val, 0, 16);
			} else if (!strcmp(arg, "--dtb")) {
				if (add_dtb(&params, false, false, val))
					return 1;
			} else if (!strcmp(arg, "--dtbo-auto")) {
				if (add_dtb(&params, true, true, val))
					return 1;
			} else if (!strcmp(arg, "--dtbo")) {
				if (add_dtb(&params, true, false, val))
					return 1;
			} else if (!strcmp(arg, "--pagesize")) {
				/* ignored */
			} else if (!strcmp(arg, "--os_version")) {
				params.os_version = val;
			} else if (!strcmp(arg, "--os_patch_level")) {
				params.patch_level = val;
			} else {
				printf("Unrecognised argument '%s'.\n", arg);
				return usage();
			}
		} else {
			printf("Expected more arguments, only %d left\n", argc);
			return usage();
		}
	}

	if (!params.kernel_path || !params.ramdisk_path || !params.output_path || !params.dtbs) {
		printf("Missing important argument! %s %s %s %p\n", params.kernel_path, params.ramdisk_path, params.output_path, params.dtbs);
		return usage();
	}

	size = estimate_size(&params);
	if (size < 0) {
		printf("%s: Failed to estimate size\n", argv[0], size);
		return 1;
	}
	buf = malloc(size);
	if (!buf) {
		printf("%s: Out of memory while allocating %d bytes\n", argv[0], size);
		return 1;
	}

	ret = make_fdt(&params, buf, size);
	if (ret < 0) {
		printf("%s: Failed to create FIT image\n", argv[0]);
		return 1;
	}

	fd = open(params.output_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		perror("Failed to open output file");
		return 1;
	}

	size = ret;
	ret = write(fd, buf, size);
	if (ret != size) {
		perror("Failed to write FDT");
		return 1;
	}

	close(fd);
	free(buf);
	return 0;
}
