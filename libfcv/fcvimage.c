/*
  Copyright (C) 2017 Open Intelligent Machines Co.,Ltd
 
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  AAID fcv image related functions
  Authour : Haibing Xu <hxu@openailab.com>
*/

#include <cairo.h>
#include <gtk/gtk.h>  
#include <window.h>

//#define RGB_HAS_ALPHA TRUE
#define RGB_HAS_ALPHA FALSE

static inline char *alloc_vimagemem(int pixels)
{
	int sz;
	sz = pixels * BYTES_OF_BPP24;
	return (char *)malloc(sz);
}

fcvImage *vimage_from_file(const char *filename)
{
	fcvImage *fcvimg;

	fcvimg = gdk_pixbuf_new_from_file(filename, NULL);

	fcvimage_info_dump(fcvimg);

	return fcvimg;
}

fcvImage *vimage_from_frame(captureCamera *frame)
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
		convert_yuyv_to_rgb32(frame->base, rawdata, w, h, rowstride);
	} else {
		convert_yuyv_to_rgb24(frame->base, rawdata, w, h, rowstride);
	}

	fcvimage_info_dump(fcvimg);
	return fcvimg;
}

fcvImage *vimage_dup(fcvImage *image)
{
	return gdk_pixbuf_copy(image);
}

fcvImage *vimage_from_data(const char *data, int width, int height)
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

fcvImage *vimage_from_grey8(const uint8_t *grey, int width, int height)
{
	fcvImage *fcvimg;
	guchar *rawdata;

	fcvimg = gdk_pixbuf_new(GDK_COLORSPACE_RGB, RGB_HAS_ALPHA,
					SAMPLEBITS_RGB, width, height);
	if (!fcvimg)
		return NULL;

	rawdata = vimage_get_rawdata(fcvimg);
	convert_grey8_to_rgb24(grey, rawdata, width, height);

	fcvimage_info_dump(fcvimg);
	return fcvimg;

}

fcvImage *vimage_scaleto(fcvImage *vimg, int width, int height)
{
	return gdk_pixbuf_scale_simple(vimg,
				width, height, GDK_INTERP_BILINEAR);
}

#define locate_to_pixdata(sp, st, bop, x, y)   \
	((sp) + (y) * st + (x) * (bop))

void fcv_point_rgb(fcvImage *fcvimg, fcv_point_t *point,
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

void fcv_xline_rgb(fcvImage *fcvimg, fcv_line_t *line,
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

void fcv_yline_rgb(fcvImage *fcvimg, fcv_line_t *line,
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

void fcv_rectangle(fcvImage *fcvimg, fcv_rectangle_t *rect,
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

#if 0
gboolean draw_onwindow(GtkWidget *widget, cairo_t *cr, gpointer data)
{  
	GdkPixbuf *src_pixbuf;
	GdkPixbuf *dst_pixbuf;
	int width, height;
	char picname[MAX_NAME_STRLEN];

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	//gtk_widget_get_size_request(widget, &width, &height);
	pr_debug("**Dispaly area size : %d x %d\n", width, height);
	pr_debug("**Data : 0x%x\n", *(int *)data);

	pigidx %= 2;
	snprintf(picname, MAX_NAME_STRLEN, "./image/backgroud%d.jpg", pigidx);
	pigidx++;

	pr_debug("file name : %s\n", picname);

	src_pixbuf = gdk_pixbuf_new_from_file(picname, NULL);
	if (!src_pixbuf) {
		g_print("*** Error - %d@%s\n", __LINE__, __func__);
	}

	// 指定图片大小  
	dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf,
					width, height, GDK_INTERP_BILINEAR);
	if (!dst_pixbuf) {
		g_print("*** Error - %d@%s\n", __LINE__, __func__);
	}

	gdk_cairo_set_source_pixbuf(cr, dst_pixbuf, 0, 0);
	cairo_paint(cr);

	// 释放资源  
	g_object_unref(dst_pixbuf);
	g_object_unref(src_pixbuf);

#if 0
	// 绘图与写字共存的测试 
	// 如果绘完图片后想继续写字或画线， 
	// 必须手动设置画笔颜色cairo_set_source_rgb() 
	// 否则，字体或线条会被图片覆盖。 
	cairo_set_source_rgb(cr, 0.627, 0, 0);  // 设置字体颜色 
	cairo_set_font_size(cr, 40.0);          // 设置字体大小 
	cairo_move_to(cr, 50.0, 130.0);         // 写字的起点坐标 
	cairo_show_text(cr, "This is a test");  // 写字 
#endif

	return FALSE;
}
#endif
