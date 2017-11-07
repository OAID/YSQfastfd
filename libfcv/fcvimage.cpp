#include "fcvimage.h"

namespace OAID {

fcvimage::fcvimage()
{

}

fcvImage *fcvimage::vimage_from_file(const char *filename)
{
    fcvImage *fcvimg;

    fcvimg = gdk_pixbuf_new_from_file(filename, NULL);

    fcvimage_info_dump(fcvimg);

    return fcvimg;
}

fcvImage *fcvimage::vimage_from_frame(captureCamera *frame)
{
    fcvImage *fcvimg;
    guchar *rawdata;
    int w, h, rowstride;

    w = frame->pixfmt->width;
    h = frame->pixfmt->height;
    rowstride = frame->pixfmt->bytesperline;

    fcvimg = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                    RGB_HAS_ALPHA, SAMPLEBITS_RGB, w, h);
    if (!fcvimg)
        return NULL;

    rawdata = vimage_get_rawdata(fcvimg);
    if (RGB_HAS_ALPHA) {
        yuvproc::convert_yuyv_to_rgb32(frame->base, rawdata, w, h, rowstride);
    } else {
        yuvproc::convert_yuyv_to_rgb24(frame->base, rawdata, w, h, rowstride);
    }

    fcvimage_info_dump(fcvimg);
    return fcvimg;
}

fcvImage *fcvimage::vimage_dup(fcvImage *image)
{
    return gdk_pixbuf_copy(image);
}

fcvImage *fcvimage::vimage_from_data(const char *data, int width, int height)
{
    fcvImage *fcvimg;
    int rowstride;

    rowstride = width * BYTES_OF_BPP24;
    fcvimg = gdk_pixbuf_new_from_data((guchar *)data,
                GDK_COLORSPACE_RGB, FALSE,
                SAMPLEBITS_RGB, width, height,
                rowstride, NULL, NULL);

    return fcvimg;
}

fcvImage *fcvimage::vimage_from_grey8(const uint8_t *grey, int width, int height)
{
    fcvImage *fcvimg;
    guchar *rawdata;

    fcvimg = gdk_pixbuf_new(GDK_COLORSPACE_RGB, RGB_HAS_ALPHA,
                    SAMPLEBITS_RGB, width, height);
    if (!fcvimg)
        return NULL;

    rawdata = vimage_get_rawdata(fcvimg);
    yuvproc::convert_grey8_to_rgb24(grey, rawdata, width, height);

    fcvimage_info_dump(fcvimg);
    return fcvimg;

}

fcvImage *fcvimage::vimage_scaleto(fcvImage *vimg, int width, int height)
{
    return gdk_pixbuf_scale_simple(vimg,
                width, height, GDK_INTERP_BILINEAR);
}

#define locate_to_pixdata(sp, st, bop, x, y)   \
    ((sp) + (y) * st + (x) * (bop))

void fcvimage::fcv_point_rgb(fcvImage *fcvimg, fcv_point_t *point,
            uint8_t r, uint8_t g, uint8_t b)
{
    int bop;
    int rst;
    guchar *pos;

    rst = vimage_get_rowstride(fcvimg);
    bop = vimage_get_channels(fcvimg);

    pos = locate_to_pixdata(vimage_get_rawdata(fcvimg),
                rst, bop, point->x, point->y);
    rgb_write(pos, r, g, b);
}

void fcvimage::fcv_xline_rgb(fcvImage *fcvimg, fcv_line_t *line,
            uint8_t r, uint8_t g, uint8_t b)
{
    int i;
    int bop, rst;
    guchar *pos;

    rst = vimage_get_rowstride(fcvimg);
    bop = vimage_get_channels(fcvimg);

    pos = locate_to_pixdata(vimage_get_rawdata(fcvimg),
                rst, bop, line->x, line->y);

    for (i = 0; i < line->length; i++) {
        rgb_write(pos, r, g, b);
        pos += bop;
    }
}

void fcvimage::fcv_yline_rgb(fcvImage *fcvimg, fcv_line_t *line,
            uint8_t r, uint8_t g, uint8_t b)
{
    int i;
    int bop, rst;
    guchar *pos;

    rst = vimage_get_rowstride(fcvimg);
    bop = vimage_get_channels(fcvimg);
    pos = locate_to_pixdata(vimage_get_rawdata(fcvimg),
            rst, bop, line->x, line->y);

    for (i = 0; i < line->length; i++) {
        rgb_write(pos, r, g, b);
        pos += rst;
    }
}

void fcvimage::fcv_rectangle(fcvImage *fcvimg, fcv_rectangle_t *rect,
                uint8_t r, uint8_t g, uint8_t b)
{
    int i;
    int bop, rst;
    guchar *lpos;
    guchar *pos1, *pos2;

    rst = vimage_get_rowstride(fcvimg);
    bop = vimage_get_channels(fcvimg);

    lpos = locate_to_pixdata(vimage_get_rawdata(fcvimg),
                    rst, bop, rect->x, rect->y);

    pos1 = lpos;
    pos2 = lpos + (rect->height * rst);
    for (i = 0; i < rect->width; i++) {
        rgb_write(pos1, r, g, b);
        rgb_write(pos2, r, g, b);
        pos1 += bop;
        pos2 += bop;
    }

    pos1 = lpos;
    pos2 = lpos + (rect->width * bop);
    for (i = 0; i < rect->height; i++) {
        rgb_write(pos1, r, g, b);
        rgb_write(pos2, r, g, b);
        pos1 += rst;
        pos2 += rst;
    }
}

void fcvimage::fcv_saveface(fcvImage *fcvimg, fcv_rectangle_t *rect, char * filepath)
{

        fcvImage* temp = gdk_pixbuf_new_subpixbuf(fcvimg, rect->x, rect->y, rect->width, rect->height);

        gdk_pixbuf_save(temp, filepath, "jpeg", NULL, "quality", "100", NULL);
		
		g_object_unref(temp);

}


}
