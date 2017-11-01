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

#include <stdint.h>
#include <cairo.h>
#include <gtk/gtk.h>  
#include <window.h>
#include <perf.h>
#include <ysqfd.h>
#include <video.h>
#include <oaidsys.h>
#include <yuvproc.h>

using namespace OAID;

#define WINDOW_XPOS   100
#define WINDOW_YPOS   100
#define RGB_WINDOW    "RGB"

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

void greydata_from_frame(captureCamera *frame)
{
	int w, h;

	w = frame->pixfmt->width;
	h = frame->pixfmt->height;
    yuvproc::convert_yuyv_to_grey8(frame->base, grey8rawdata, w, h);
}

int main(int argc, char **argv)
{
	int camfd;
	int camid = 0;
	char ikey;
    int status = 1;
    fcvImage *image_orig;
    captureCamera *frame;
	int vidiw, vidih;

    video *_video = new video();
    window *_window = new window();
    ysqfd *_ysqfd = new ysqfd();
    fcvimage *_fcvimage = new fcvimage();

    camfd = _video->create_vidcapture(camid);
	if (camfd < 0) {
		assert_failure();
		return camfd;
	}

    _video->query_vidimgsize(camid, &vidiw, &vidih);
	grey8mem_init(vidiw, vidih);
    _ysqfd->algr_ysqfd_init(vidiw, vidih);

    _window->named_window(RGB_WINDOW, WINDOW_NORMAL);
    _window->resize_window(RGB_WINDOW, vidiw, vidih);
//	_window->resize_window(RGB_WINDOW, 320, 240);
    _window->move_window(RGB_WINDOW, WINDOW_XPOS, WINDOW_YPOS);

	do {
        frame = _video->capturevid(camid);
		
		greydata_from_frame(frame);
        image_orig = _fcvimage->vimage_from_frame(frame);

        _ysqfd->ysqfd_process(grey8rawdata, image_orig);
        _window->imageshow(RGB_WINDOW, image_orig);

        ikey = _window->waitkey(100);
        switch (ikey) {
        case WINDOW_QUITKEY:
            status = 0;
            break;
        case WINDOW_PHOTOKEY:
            _ysqfd->ysqfd_config_set(ELEMENT_PHOTO, VALUE_AUTO_REVERSAL);
            break;
        case WINDOW_SHOWKEY:
            _ysqfd->ysqfd_config_set(ELEMENT_SHOWMARK, VALUE_AUTO);
            break;
        default:
            break;
        }
    }while(status);

    _video->destroy_vidcapture(camid);
	grey8mem_release();
    _window->destroy_window(RGB_WINDOW);
    _ysqfd->algr_ysqfd_exit();
	return 0;
}
