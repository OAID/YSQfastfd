#ifndef FCVIDIMG_H__
#define FCVIDIMG_H__
#include <stdbool.h>
#include <camvid.h>
#include <pixel.h>
#include <cairo.h>
#include <gtk/gtk.h>  

#define VIMAGE_FMT_GREY8   v4l2_fourcc('G', 'R', 'E', 'Y')
#define VIMAGE_FMT_RGB16   v4l2_fourcc('R', 'G', 'B', '2')
#define VIMAGE_FMT_RGB24   v4l2_fourcc('R', 'G', 'B', '3')
#define VIMAGE_FMT_RGB32   v4l2_fourcc('R', 'G', 'B', '4')

#define VIMAGE_FMT_YUYV    v4l2_fourcc('Y', 'U', 'Y', 'V')
#define VIMAGE_FMT_YVYU    v4l2_fourcc('Y', 'V', 'Y', 'U')
#define VIMAGE_FMT_UYVY    v4l2_fourcc('U', 'Y', 'V', 'Y')

typedef GdkPixbuf fcvImage;
typedef GdkColorspace ColorSpace;

static inline int vimage_get_width(fcvImage *vimg)
{
	return gdk_pixbuf_get_width(vimg);
}

static inline int vimage_get_height(fcvImage *vimg)
{
	return gdk_pixbuf_get_height(vimg);
}

static inline int vimage_get_length(fcvImage *vimg)
{
	return gdk_pixbuf_get_byte_length(vimg);
}

static inline int vimage_get_bps(fcvImage *vimg)
{
	return gdk_pixbuf_get_bits_per_sample(vimg);
}

static inline ColorSpace vimage_get_colorspace(fcvImage *vimg)
{
	return gdk_pixbuf_get_colorspace(vimg);
}

static inline int vimage_get_channels(fcvImage *vimg)
{
	return gdk_pixbuf_get_n_channels(vimg);
}

static inline bool vimage_has_alpha(fcvImage *vimg)
{
	return gdk_pixbuf_get_has_alpha(vimg);
}

static inline int vimage_get_rowstride(fcvImage *vimg)
{
	return gdk_pixbuf_get_rowstride(vimg);
}

static inline guchar *vimage_get_rawdata(fcvImage *vimg)
{
	return gdk_pixbuf_get_pixels(vimg);
}

static inline int vimage_get_bpp(fcvImage *vimg)
{
	int c, b;
	b = gdk_pixbuf_get_bits_per_sample(vimg);
	c = vimage_get_channels(vimg);
	return (c * b);
}

#ifdef DEBUG
static inline void fcvimage_info_dump(fcvImage *fcvimg)
{
	fprintf(stderr, "fcvimage info\n"
			"  image size     : %d x %d\n"
			"  image stride   : %d\n"
			"  color space    : %d\n"
			"  bits per sample: %d\n"
			"  Channels       : %d\n"
			"  bits per pixel : %d\n"
			"  has alpha      : %c\n"
			"  raw data size  : %d\n",
			vimage_get_width(fcvimg), vimage_get_height(fcvimg),
			vimage_get_rowstride(fcvimg), vimage_get_colorspace(fcvimg),
			vimage_get_bps(fcvimg), vimage_get_channels(fcvimg),
			vimage_get_bpp(fcvimg),
			vimage_has_alpha(fcvimg) ? 'Y' : 'N',
			vimage_get_length(fcvimg));
}
#else
static inline void fcvimage_info_dump(fcvImage *fcvimg)
{
}
#endif

fcvImage *vimage_from_file(const char *filename);
fcvImage *vimage_from_frame(captureCamera *frame);
fcvImage *vimage_dup(fcvImage *image);
fcvImage *vimage_scaleto(fcvImage *vimg, int width, int height);
fcvImage *vimage_from_grey8(const uint8_t *grey, int width, int height);

void convert_yuyv_to_rgb32(const uint8_t *src, uint8_t *dest,
        int width, int height, int stride);
void convert_yuyv_to_rgb24(const uint8_t *src, uint8_t *dest,
        int width, int height, int stride);
void convert_grey8_to_rgb24(const uint8_t *src, uint8_t *dest,
			int width, int height);
void convert_yuyv_to_grey8(const uint8_t *src, uint8_t *dest,
			int width, int height);
void fcv_point_rgb(fcvImage *fcvimg, fcv_point_t *point,
		   	uint8_t r, uint8_t g, uint8_t b);
void fcv_xline_rgb(fcvImage *fcvimg, fcv_line_t *line,
			uint8_t r, uint8_t g, uint8_t b);
void fcv_yline_rgb(fcvImage *fcvimg, fcv_line_t *line,
			uint8_t r, uint8_t g, uint8_t b);
void fcv_rectangle(fcvImage *fcvimg, fcv_rectangle_t *rect,
			uint8_t r, uint8_t g, uint8_t b);
#endif
