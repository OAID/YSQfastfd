#ifndef OAID_BASE_H
#define OAID_BASE_H

#include <linux/videodev2.h>
#include <stdint.h>
#include <pixel.h>

namespace OAID {

#ifdef DEBUG
# define pr_debug(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
# define pr_debug(format, ...)
#endif

struct framedata {
    struct v4l2_buffer kvbuf;         /* capture buffer info */
    struct v4l2_pix_format *pixfmt;   /* capture image pixel info */
    uint8_t *base;                    /* capture buffer based address mapped in userspace */
    int length;                       /* capture buffer lenght, the frame size */
};
typedef struct framedata captureCamera;

class oaid_base
{
public:
    oaid_base();
};

}


#endif // OAID_BASE_H
