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

  Authour : Haibing Xu <hxu@openailab.com>
*/
#include <cairo.h>
#include <gtk/gtk.h>  
#include <window.h>
#include <perf.h>

#define WINDOW_XPOS   100
#define WINDOW_YPOS   100
#define RGB_WINDOW    "RGB"
#define GREY_WINDOW   "GREY"
void text_osd_onwindow(cairo_t *cr, gpointer data);
void rectangle_onwindow(cairo_t *cr, gpointer data);

void fcv_line_test(fcvImage *vimg)
{
	fcv_line_t line;
	line.x = 10;
	line.y = 10;
	line.length = 50;

	fcv_xline_rgb(vimg, &line, 255, 0, 0);
	fcv_yline_rgb(vimg, &line, 0, 255, 0);
}

void fcv_rectangle_test(fcvImage *vimg)
{
	fcv_rectangle_t rect;
	rect.x = 100;
	rect.y = 50;
	rect.width = 64;
	rect.height = 48;

	fcv_rectangle(vimg, &rect, 0, 0, 255);
}

fcv_rectangle_t window_rect;
void imageshow_with_rect(const char *name, fcvImage *fcvimg)
{
	window_rect.x = 50;
	window_rect.y = 50;
	window_rect.width = 64;
	window_rect.height = 48;
	imageshow_ops(name, fcvimg, rectangle_onwindow, &window_rect);
}

static uint8_t *grey8rawdata = NULL;
static uint8_t *grey8mem_init(int iw, int ih)
{
	grey8rawdata = malloc(iw * ih);
	return grey8rawdata;
}
static void grey8mem_release(void)
{
	if (grey8rawdata) {
		free(grey8rawdata);
		grey8rawdata = NULL;
	}
}

fcvImage *greyimg_from_frame(captureCamera *frame)
{
	int w, h;
	fcvImage *vimg;

	w = frame->pixfmt->width;
	h = frame->pixfmt->height;

	convert_yuyv_to_grey8(frame->base, grey8rawdata, w, h);
	vimg = vimage_from_grey8(grey8rawdata, w, h);
	return vimg;
}

int main(int argc, char **argv)
{
	int camfd;
	int camid = 0;
	char ikey;
	fcvImage *imgOrig;
	fcvImage *imgCopy;
	captureCamera *frame;
	int vidiw, vidih;

	camfd = create_vidcapture(camid);
	if (camfd < 0) {
		assert_failure();
		return camfd;
	}

	query_vidimgsize(camid, &vidiw, &vidih);
	grey8mem_init(vidiw, vidih);

	named_window(RGB_WINDOW, WINDOW_NORMAL);
	named_window(GREY_WINDOW,  WINDOW_NORMAL);
	resize_window(RGB_WINDOW, 640, 480);
	resize_window(GREY_WINDOW, 320, 240);
	move_window(RGB_WINDOW, WINDOW_XPOS, WINDOW_YPOS);

	do {
		frame = capturevid(camid);

		imgOrig = vimage_from_frame(frame);
		imgCopy = greyimg_from_frame(frame);

		fcv_line_test(imgOrig);
		fcv_rectangle_test(imgOrig);
		imageshow(RGB_WINDOW, imgOrig);
		imageshow(GREY_WINDOW, imgCopy);

		ikey = waitkey(100);
		if (ikey == WINDOW_QUITKEY) break;
	}while(1);

	destroy_vidcapture(camid);
	grey8mem_release();
	destroy_window(RGB_WINDOW);
	destroy_window(GREY_WINDOW);
	return 0;
}
