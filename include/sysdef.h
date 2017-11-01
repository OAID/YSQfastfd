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
#include <oaid_base.h>


#define assert_failure()    \
	fprintf(stderr, "[*Error*]Failure @ line %d in function \"%s\"\n",\
		   	__LINE__, __func__)

#define BITS_BYTE_SHIFT      3
#define BITS_PER_BYTE        8

#define MAX_NAME_STRLEN      32
#define MAX_OPENED_CAM       8

#define gettid() syscall(SYS_gettid)

enum _RV
{
    RV_SUCCESS  =    0,
    RV_EPERM	=	 1,	/* Operation not permitted */
    RV_ENOENT	=	 2,	/* No such file or directory */
    RV_ESRCH	=	 3,	/* No such process */
    RV_EINTR	=	 4,	/* Interrupted system call */
    RV_EIO		=    5,	/* I/O error */
    RV_ENXIO	=	 6,	/* No such device or address */
    RV_E2BIG	=	 7,	/* Argument list too long */
    RV_ENOEXEC	=	 8,	/* Exec format error */
    RV_EBADF	=	 9,	/* Bad file number */
    RV_ECHILD	=	10,	/* No child processes */
    RV_EAGAIN	=	11,	/* Try again */
    RV_ENOMEM	=	12,	/* Out of memory */
    RV_EACCES	=	13,	/* Permission denied */
    RV_EFAULT	=	14,	/* Bad address */
    RV_ENOTBLK	=	15,	/* Block device required */
    RV_EBUSY	=	16,	/* Device or resource busy */
    RV_EEXIST	=	17,	/* File exists */
    RV_EXDEV	=	18,	/* Cross-device link */
    RV_ENODEV	=	19,	/* No such device */
    RV_ENOTDIR	=	20,	/* Not a directory */
    RV_EISDIR	=	21,	/* Is a directory */
    RV_EINVAL	=	22,	/* Invalid argument */
    RV_ENFILE	=	23,	/* File table overflow */
    RV_EMFILE	=	24,	/* Too many open files */
    RV_ENOTTY	=	25,	/* Not a typewriter */
    RV_ETXTBSY	=	26,	/* Text file busy */
    RV_EFBIG	=	27,	/* File too large */
    RV_ENOSPC	=	28,	/* No space left on device */
    RV_ESPIPE	=	29,	/* Illegal seek */
    RV_EROFS	=	30,	/* Read-only file system */
    RV_EMLINK	=	31,	/* Too many links */
    RV_EPIPE	=	32,	/* Broken pipe */
    RV_EDOM		=   33,	/* Math argument out of domain of func */
    RV_ERANGE	=	34,	/* Math result not representable */
    RV_MAX
};

typedef enum _RV RV;

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
