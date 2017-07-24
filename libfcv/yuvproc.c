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

  YUV to RGB convertion routines
  Authour : Haibing Xu <hxu@openailab.com>
*/


#include <string.h>
#include <camvid.h>
#include <pixel.h>

#define PIXFIX(c) (uint8_t)(((c) > 0xFF) ? 0xff : (((c) < 0) ? 0 : (c)))

/*
 * Extract luminance information from yuyv stream frame
 *   Effective bits per pixel         : 16
 *   Effective pixels per macro pixel : 2
 *   Effective bytes per pixel        : 2
 */
int packedyuv422_to_grey8(uint8_t *dest, const uint8_t *data,
			int width, int height, uint32_t pixelformat)
{
	int length, i;
	uint8_t *src;

	pr_debug("Frame size : %d x %d\n", width, height);
	
	switch(pixelformat)
	{
		/*
		 * Byte ordering : Y0 U0 Y1 V0
		 * Byte ordering : Y0 V0 Y1 U0
		 */
		case V4L2_PIX_FMT_YUYV :
		case V4L2_PIX_FMT_YVYU :
			src = (uint8_t *)data;
			break;
		/*
		 * Byte ordering : U0 Y0 V1 Y1
		 */
		case V4L2_PIX_FMT_UYVY :
			src = (uint8_t *)data + 1;
			break;
		default :
			fprintf(stderr,"Unsupport stream frame format :%c%c%c%c\n",
				(pixelformat) & 0xFF, (pixelformat >> 8) & 0xFF,
				(pixelformat >> 16) & 0xFF, (pixelformat >> 24) & 0xFF);
			length = -EINVAL;
			goto out;
	}

	length = 0;
	while (height-- > 0) {
		for (i = 0; i < width; i += BYTES_PER_PIX_YUV422) {
			/* extrace luminance of a marco pixel */
			dest[0] = src[0];
			dest[1] = src[2];

			/* pointer to the next macro pixel */
			src  += BYTES_PER_MPIX_YUV422;
			dest += BYTES_PER_PIX_YUV422;

			/* two luminance in one macro pixel */
			length += BYTES_PER_PIX_YUV422;
		}
	}

	pr_debug("luminance of uyvy length = %d\n", length);
out:
	return length;
}

int luminance_of_packedyuv422(uint8_t *dest, struct framedata *frame)
{
	return packedyuv422_to_grey8(dest, frame->base, frame->pixfmt->width,
				frame->pixfmt->height, frame->pixfmt->pixelformat);
}

void convert_yuyv_to_grey8(const uint8_t *src, uint8_t *dest,
			int width, int height)
{
	int i;
	int len = 0;

	while (--height >= 0) {
		for (i = 0; i < width; i += BYTES_PER_PIX_YUV422) {
			/* extrace luminance of a marco pixel */
			dest[0] = src[0];
			dest[1] = src[2];

			/* pointer to the next macro pixel */
			src  += BYTES_PER_MPIX_YUV422;
			dest += BYTES_PER_PIX_YUV422;

			/* two luminance in one macro pixel */
			len += BYTES_PER_PIX_YUV422;
		}
	}
	pr_debug("Grey8 data length : %d\n", len);
}

void convert_grey8_to_rgb24(const uint8_t *src, uint8_t *dest,
			int width, int height)
{
	int i;

	while(--height >= 0) {
		for (i = 0; i < width; i++) {
			rgb_write(dest, *src, *src, *src);
			dest += BYTES_OF_BPP24;
			src++;
		}
	}
}

void convert_yuyv_to_rgb24(const uint8_t *src, uint8_t *dest,
        int width, int height, int stride)
{
    int j;

    while (--height >= 0) {
        for (j = 0; j + 1 < width; j += 2) {
            int u = src[1];
            int v = src[3];
            int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
            int rg = (((u - 128) << 1) +  (u - 128) +
                    ((v - 128) << 2) + ((v - 128) << 1)) >> 3;
            int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

            *dest++ = PIXFIX(src[0] + v1);
            *dest++ = PIXFIX(src[0] - rg);
            *dest++ = PIXFIX(src[0] + u1);

            *dest++ = PIXFIX(src[2] + v1);
            *dest++ = PIXFIX(src[2] - rg);
            *dest++ = PIXFIX(src[2] + u1);
            src += 4;
        }
        src += stride - (width * 2); 
    }
}

void convert_yuyv_to_rgb32(const uint8_t *src, uint8_t *dest,
        int width, int height, int stride)
{
	uint32_t *d;
	int mpixsperline, i;
	rgb32_pixel_t rgb32;

	d = (uint32_t *)dest;
	mpixsperline = stride / BYTES_PER_MPIX_YUV422;

	pr_debug("vimage size : %d x %d\n", width, height);
	pr_debug("macro pixels perline : %d\n", mpixsperline);

	rgb32.m.a = 0xff;
	while (--height >= 0) {
        for (i = 0; i < mpixsperline; i++) {
			int u = src[1];
            int v = src[3];
            int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
            int rg = (((u - 128) << 1) +  (u - 128) +
                    ((v - 128) << 2) + ((v - 128) << 1)) >> 3;
            int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

            rgb32.m.b = PIXFIX(src[0] + v1);
            rgb32.m.g = PIXFIX(src[0] - rg);
            rgb32.m.r = PIXFIX(src[0] + u1);
			*d++ = rgb32.pixel;

            rgb32.m.b = PIXFIX(src[2] + v1);
            rgb32.m.g = PIXFIX(src[2] - rg);
            rgb32.m.r = PIXFIX(src[2] + u1);
			*d++ = rgb32.pixel;
            src += 4;
		}
	}
}
