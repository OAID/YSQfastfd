#ifndef YSQFD_H
#define YSQFD_H

#include <sysdef.h>
#include <fcvimage.h>
#include <perf.h>
#include <midlayer.h>
#include <oaid_base.h>

namespace OAID {

#define YSQFD_RTMEM_SIZE 0x20000

#define YSQ_CONFIG_SCALE         1.2f
#define YSQ_CONFIG_MINNEIGHBORS  3
#define YSQ_CONFIG_MINOBJWDITH   32
#define YSQ_CONFIG_MAXOBJWDITH   0

#define LANDMARK_ENABLE      1
#define LANDMARK_DISABLE     0

class ysqfd : public oaid_base
{
public:

    int savephoto;
    int showmark;

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

public:
    ysqfd();

public:

    void ysqfd_initconfig(int vidw, int vidh, struct ysq_fdconfig *c);
    int algr_ysqfd_init(int imgw, int imgh);
    void algr_ysqfd_exit(void);
    int ysqfd_process(void *keyinfo, fcvImage *vimg);
    void ysqfd_config_set(int key, int value);
    void ysqfd_config_get(int key, int *value);
};

}

#endif // YSQFD_H
