#ifndef SYSDEF_H__
#define SYSDEF_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syscall.h>
//#include <gtk/gtk.h>

#ifdef DEBUG
# define pr_debug(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
# define pr_debug(format, ...)
#endif

#define assert_failure()    \
	fprintf(stderr, "[*Error*]Failure @ line %d in function \"%s\"\n",\
		   	__LINE__, __func__)

#define BITS_BYTE_SHIFT      3
#define BITS_PER_BYTE        8

#define MAX_NAME_STRLEN      32
#define MAX_OPENED_CAM       8

#define gettid() syscall(SYS_gettid)

struct coordinate
{
	int vt_x;
	int vt_y;
};

struct imagesize
{
	int width;                   /* image width in pixel */
	int height;                  /* image height in pixel */
};
typedef struct imagesize fcv_imagesize_t;

struct fcv_config
{
	struct {
		char name[MAX_NAME_STRLEN];
		int id;
		int fd;
		int imwidth;
		int imheight;
	}cam[MAX_OPENED_CAM];
};

struct mem_region {
	long len;
	void *base;
};
typedef struct mem mem_region_t;

struct runtime_params {
	int camid;
	int winsys_status;
};

#endif
