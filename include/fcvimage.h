#ifndef FCVIMAGE_H
#define FCVIMAGE_H

#include <stdbool.h>
//#include <camvid.h>
#include <pixel.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <oaid_base.h>
#include <yuvproc.h>

namespace OAID {

//#define RGB_HAS_ALPHA TRUE
#define RGB_HAS_ALPHA FALSE

#define VIMAGE_FMT_GREY8   v4l2_fourcc('G', 'R', 'E', 'Y')
#define VIMAGE_FMT_RGB16   v4l2_fourcc('R', 'G', 'B', '2')
#define VIMAGE_FMT_RGB24   v4l2_fourcc('R', 'G', 'B', '3')
#define VIMAGE_FMT_RGB32   v4l2_fourcc('R', 'G', 'B', '4')

#define VIMAGE_FMT_YUYV    v4l2_fourcc('Y', 'U', 'Y', 'V')
#define VIMAGE_FMT_YVYU    v4l2_fourcc('Y', 'V', 'Y', 'U')
#define VIMAGE_FMT_UYVY    v4l2_fourcc('U', 'Y', 'V', 'Y')

typedef GdkPixbuf fcvImage;
typedef GdkColorspace ColorSpace;

class fcvimage : public oaid_base
{
public:
    fcvimage();

public:
    static int vimage_get_width(fcvImage *vimg)
    {
        return gdk_pixbuf_get_width(vimg);
    }

    static int vimage_get_height(fcvImage *vimg)
    {
        return gdk_pixbuf_get_height(vimg);
    }

    static int vimage_get_length(fcvImage *vimg)
    {
        return gdk_pixbuf_get_byte_length(vimg);
    }

    static int vimage_get_bps(fcvImage *vimg)
    {
        return gdk_pixbuf_get_bits_per_sample(vimg);
    }

    static ColorSpace vimage_get_colorspace(fcvImage *vimg)
    {
        return gdk_pixbuf_get_colorspace(vimg);
    }

    static int vimage_get_channels(fcvImage *vimg)
    {
        return gdk_pixbuf_get_n_channels(vimg);
    }

    static bool vimage_has_alpha(fcvImage *vimg)
    {
        return gdk_pixbuf_get_has_alpha(vimg);
    }

    static int vimage_get_rowstride(fcvImage *vimg)
    {
        return gdk_pixbuf_get_rowstride(vimg);
    }

    static guchar *vimage_get_rawdata(fcvImage *vimg)
    {
        return gdk_pixbuf_get_pixels(vimg);
    }

    static int vimage_get_bpp(fcvImage *vimg)
    {
        int c, b;
        b = gdk_pixbuf_get_bits_per_sample(vimg);
        c = vimage_get_channels(vimg);
        return (c * b);
    }

    #ifdef DEBUG
    static void fcvimage_info_dump(fcvImage *fcvimg)
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
    static void fcvimage_info_dump(fcvImage *fcvimg)
    {
    }
    #endif

    static fcvImage *vimage_from_file(const char *filename);
    static fcvImage *vimage_from_frame(captureCamera *frame);
    static fcvImage *vimage_dup(fcvImage *image);
    static fcvImage *vimage_scaleto(fcvImage *vimg, int width, int height);
    static fcvImage *vimage_from_grey8(const uint8_t *grey, int width, int height);
    static fcvImage *vimage_from_data(const char *data, int width, int height);

    static void fcv_point_rgb(fcvImage *fcvimg, fcv_point_t *point,
                uint8_t r, uint8_t g, uint8_t b);
    static void fcv_rectangle(fcvImage *fcvimg, fcv_rectangle_t *rect,
                uint8_t r, uint8_t g, uint8_t b);
    static void fcv_xline_rgb(fcvImage *fcvimg, fcv_line_t *line,
                uint8_t r, uint8_t g, uint8_t b);
    static void fcv_yline_rgb(fcvImage *fcvimg, fcv_line_t *line,
                uint8_t r, uint8_t g, uint8_t b);
    static void fcv_saveface(fcvImage *fcvimg, fcv_rectangle_t *rect, char * filepath);
};

}

#endif // FCVIMAGE_H
