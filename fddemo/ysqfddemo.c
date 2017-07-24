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
#include <ysqfd.h>
#include <aaidalgr.h>

#define WINDOW_XPOS   100
#define WINDOW_YPOS   100
#define RGB_WINDOW    "RGB"
void text_osd_onwindow(cairo_t *cr, gpointer data);
void rectangle_onwindow(cairo_t *cr, gpointer data);

int algr_ysqfd_init(int imgw, int imgh);
void algr_ysqfd_exit(void);
int ysqfd_process(void *keyinfo, fcvImage *vimg);

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

void greydata_from_frame(captureCamera *frame)
{
	int w, h;

	w = frame->pixfmt->width;
	h = frame->pixfmt->height;
	convert_yuyv_to_grey8(frame->base, grey8rawdata, w, h);
}

/*
 * Only for verication of get_algconfig & set_algconfig
 *
 * change the minimum detect face size
 */
void change_face_size(int size)
{
	struct alg_config c;

	get_algconfig(AAID_ALGR_YSQ_FACEDET,&c);

	c.config.ysqfd.min_object_width = size;

	set_algconfig(AAID_ALGR_YSQ_FACEDET,&c.config);
}

int main(int argc, char **argv)
{
	int camfd;
	int camid = 0;
	char ikey;
	fcvImage *image_orig;
	captureCamera *frame;
	int vidiw, vidih;

	camfd = create_vidcapture(camid);
	if (camfd < 0) {
		assert_failure();
		return camfd;
	}

	query_vidimgsize(camid, &vidiw, &vidih);
	grey8mem_init(vidiw, vidih);
	algr_ysqfd_init(vidiw, vidih);

	named_window(RGB_WINDOW, WINDOW_NORMAL);
	resize_window(RGB_WINDOW, vidiw, vidih);
//	resize_window(RGB_WINDOW, 320, 240);
	move_window(RGB_WINDOW, WINDOW_XPOS, WINDOW_YPOS);

	do {

		frame = capturevid(camid);
		
		greydata_from_frame(frame);
		image_orig = vimage_from_frame(frame);

		ysqfd_process(grey8rawdata, image_orig);
		imageshow(RGB_WINDOW, image_orig);

		ikey = waitkey(100);
		if (ikey == WINDOW_QUITKEY) break;

	}while(1);

	destroy_vidcapture(camid);
	grey8mem_release();
	destroy_window(RGB_WINDOW);
	algr_ysqfd_exit();
	return 0;
}
