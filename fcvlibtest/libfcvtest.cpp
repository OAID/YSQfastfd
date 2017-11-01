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
#include <video.h>

#define WINDOW_XPOS   100
#define WINDOW_YPOS   100
#define RGB_WINDOW    "RGB"
#define GREY_WINDOW   "GREY"

void fcv_line_test(OAID::fcvImage *vimg)
{
	fcv_line_t line;
	line.x = 10;
	line.y = 10;
	line.length = 50;

    OAID::fcvimage::fcv_xline_rgb(vimg, &line, 255, 0, 0);
    OAID::fcvimage::fcv_yline_rgb(vimg, &line, 0, 255, 0);
}

void fcv_rectangle_test(OAID::fcvImage *vimg)
{
	fcv_rectangle_t rect;
	rect.x = 100;
	rect.y = 50;
	rect.width = 64;
	rect.height = 48;

    OAID::fcvimage::fcv_rectangle(vimg, &rect, 0, 0, 255);
}


static uint8_t *grey8rawdata = NULL;
static uint8_t *grey8mem_init(int iw, int ih)
{
    grey8rawdata = (uint8_t *)malloc(iw * ih);
	return grey8rawdata;
}
static void grey8mem_release(void)
{
	if (grey8rawdata) {
		free(grey8rawdata);
		grey8rawdata = NULL;
	}
}

OAID::fcvImage *greyimg_from_frame(OAID::captureCamera *frame)
{
	int w, h;
    OAID::fcvImage *vimg;

	w = frame->pixfmt->width;
	h = frame->pixfmt->height;

    OAID::yuvproc::convert_yuyv_to_grey8(frame->base, grey8rawdata, w, h);
    vimg = OAID::fcvimage::vimage_from_grey8(grey8rawdata, w, h);
	return vimg;
}

int main(int argc, char **argv)
{
	int camfd;
	int camid = 0;
	char ikey;
    OAID::fcvImage *imgOrig;
    OAID::fcvImage *imgCopy;
    OAID::captureCamera *frame;
    OAID::video *_video = new OAID::video();
    OAID::window *_window = new OAID::window();
	int vidiw, vidih;

    camfd = _video->create_vidcapture(camid);
	if (camfd < 0) {
		assert_failure();
		return camfd;
	}

    _video->query_vidimgsize(camid, &vidiw, &vidih);
	grey8mem_init(vidiw, vidih);

    _window->named_window(RGB_WINDOW, WINDOW_NORMAL);
    _window->named_window(GREY_WINDOW,  WINDOW_NORMAL);
    _window->resize_window(RGB_WINDOW, 640, 480);
    _window->resize_window(GREY_WINDOW, 320, 240);
    _window->move_window(RGB_WINDOW, WINDOW_XPOS, WINDOW_YPOS);

	do {
        frame = _video->capturevid(camid);

        imgOrig = OAID::fcvimage::vimage_from_frame(frame);
        imgCopy = greyimg_from_frame(frame);

        fcv_line_test(imgOrig);
        fcv_rectangle_test(imgOrig);
        _window->imageshow(RGB_WINDOW, imgOrig);
        _window->imageshow(GREY_WINDOW, imgCopy);

        ikey = _window->waitkey(100);
		if (ikey == WINDOW_QUITKEY) break;
	}while(1);

    _video->destroy_vidcapture(camid);
	grey8mem_release();
    _window->destroy_window(RGB_WINDOW);
    _window->destroy_window(GREY_WINDOW);
	return 0;
}
