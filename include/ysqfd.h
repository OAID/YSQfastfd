#ifndef YSQFD_H__
#define YSQFD_H__

#define YSQFD_RTMEM_SIZE 0x20000

#define YSQ_CONFIG_SCALE         1.2f
#define YSQ_CONFIG_MINNEIGHBORS  3
#define YSQ_CONFIG_MINOBJWDITH   32
#define YSQ_CONFIG_MAXOBJWDITH   0

struct ysq_fdconfig {
	int imgwidth;
	int imgheight;
	int min_neighbors;
	int min_object_width;
	int max_object_width;
	int step;
	int landmark;
	float scale;
};

struct ysq_fdresource {
	void *inbuf;
	void *resbuf;
	void *rtmem;
};

#define YSQFD_LANDMARK  68
struct ysq_oformat {
    short rect_x;
    short rect_y;
    short rect_width;
    short rect_heigth;
    short neightbors;
    short angle;

    struct {
        short x;
        short y;
    } landmark [YSQFD_LANDMARK];
};

#include <libyushiqiface.h>

#endif
