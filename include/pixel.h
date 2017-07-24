#ifndef PIXEL_H__
#define PIXEL_H__

#include <stdint.h>
#include <sysdef.h>
#include <linux/videodev2.h>

/*
 * Packed YUV stream frame related marco definitions
 * Y, U and V samples are packed together into macropixels
 * which are stored in a single array (contigous memory)
 */
#define BYTES_PER_MPIX_YUV422   4
#define PIXELS_PER_MPIX_YUV422  2
#define BYTES_PER_PIX_YUV422    2
#define BITS_PER_PIXEL_YUV422   16

#define BYTES_OF_BPP32          4
#define BYTES_OF_BPP24          3

#define BPP_OF_RGB32            32
#define BPP_OF_RGB24            24
#define SAMPLEBITS_RGB          8

union rgb32_pixel
{
    uint32_t pixel;
    struct {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
    }m;
};
typedef union rgb32_pixel rgb32_pixel_t;

struct fcv_point {
	int x;
	int y;
};
typedef struct fcv_point fcv_point_t;

struct fcv_line {
	int x;
	int y;
	int length;
};
typedef struct fcv_line fcv_line_t;

struct fcv_rectangle {
	int x;
	int y;
	int width;
	int height;
};
typedef struct fcv_rectangle fcv_rectangle_t;

typedef uint32_t fcv_rgbpix_t;

#define rgb_write(p, r, g, b)                  \
	(p)[0] = (r); (p)[1] = (g); (p)[2] = (b);

#define RGB_R(rgb)    ((rgb >> 24) & 0x00FF)
#define RGB_G(rgb)    ((rgb >> 16) & 0x00FF)
#define RGB_B(rgb)    ((rgb >> 8) & 0x00FF)

static inline fcv_rgbpix_t fcv_rgbpixel(uint8_t r,
			uint8_t g, uint8_t b)
{
	fcv_rgbpix_t rgb;

	rgb = (b & 0x000000FF) << 8;
	rgb |= (g & 0x000000FF) << 16;
	rgb |= (r & 0x000000FF) << 24;

	return rgb;
}

#define FCV_RGB(r, g, b) \
	fcv_rgbpixel((r), (g), (b))

#endif
