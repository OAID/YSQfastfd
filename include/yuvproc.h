#ifndef YUVPROC_H
#define YUVPROC_H

#include <oaid_base.h>

namespace OAID {

class yuvproc : public oaid_base
{
public:
    yuvproc();
public:
    static int packedyuv422_to_grey8(uint8_t *dest, const uint8_t *data,
                int width, int height, uint32_t pixelformat);
    static int luminance_of_packedyuv422(uint8_t *dest, struct framedata *frame);
    static void convert_yuyv_to_grey8(const uint8_t *src, uint8_t *dest,
                int width, int height);
    static void convert_grey8_to_rgb24(const uint8_t *src, uint8_t *dest,
                int width, int height);
    static void convert_yuyv_to_rgb24(const uint8_t *src, uint8_t *dest,
            int width, int height, int stride);
    static void convert_yuyv_to_rgb32(const uint8_t *src, uint8_t *dest,
            int width, int height, int stride);

};

}

#endif // YUVPROC_H
