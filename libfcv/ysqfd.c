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

  YSQ face detect related functions (wrapped routines)
  Authour : Haibing Xu <hxu@openailab.com>
*/
#include <sysdef.h>
#include <fcvimage.h>
#include <aaidalgr.h>
#include <perf.h>

#define LANDMARK_ENABLE      1
#define LANDMARK_DISABLE     0

static void ysqfd_initconfig(int vidw, int vidh, struct alg_config *c)
{
	struct ysq_fdconfig *config;

	config = &(c->config.ysqfd);

	config->imgwidth = vidw;
	config->imgheight = vidh;
	config->scale = YSQ_CONFIG_SCALE;
	config->min_neighbors = YSQ_CONFIG_MINNEIGHBORS;
	config->min_object_width = YSQ_CONFIG_MINOBJWDITH;
	config->max_object_width = YSQ_CONFIG_MAXOBJWDITH;
	config->landmark = LANDMARK_ENABLE;

	pr_debug("YSQ FD initialized\n");
}

int algr_ysqfd_init(int imgw, int imgh)
{
	struct aaid_algorithm *algr;
	struct mem_region *rtmem;

	algr = calloc(1, sizeof(struct aaid_algorithm));
	if (algr == NULL) {
		return -EINVAL;
	}

	rtmem = algr->config.rtmem;

	algr->algorithm_id = AAID_ALGR_YSQ_FACEDET;
	rtmem[0].len = YSQFD_RTMEM_SIZE;
	rtmem[0].base = NULL;
	rtmem[1].len = 0;
	rtmem[1].base = NULL;
	rtmem[2].len = 0;
	rtmem[2].base = NULL;

	algr->init = config_init_internal;
	algr->clean = config_clean_internal;
	algr->process = priv_ysq_facedetect;

	algr->resource.status = 0;
	algr->resource.valid  = 0;
	algorithm_register(algr);

	ysqfd_initconfig(imgw, imgh, &algr->config);

	return 0;
}

void algr_ysqfd_exit(void)
{
	struct aaid_algorithm *algr;

	algr = algorithm_by_id(AAID_ALGR_YSQ_FACEDET);
	if (!algr) {
		return;
	}
	algorithm_unregister(algr);
	free(algr);
}

int ysqfd_process(void *keyinfo, fcvImage *vimg)
{
	int ret, i;
	int kiresnr;
	int *kires;
	fcv_rectangle_t rectangle;
	fcv_point_t point;
    struct ysq_oformat *result;
	int dolandmark = LANDMARK_DISABLE;

#ifdef PERF_STATISTICS
	struct timeval pretv, sumtv;
	sumtv.tv_sec = 0;
	sumtv.tv_usec = 0;
	perf_tv_start(&pretv);
#endif

	ret = call_algorithm(AAID_ALGR_YSQ_FACEDET, keyinfo, (void **)&kires);

#ifdef PERF_STATISTICS
	perf_tv_sample(&pretv, &sumtv);
#endif
	if (ret < 0) {
		fprintf(stderr, "Error\n");
		return -ENOMEM;
	}

	kiresnr = *kires++;

#ifdef PERF_STATISTICS
	printf("Detected %d face(s)"
			" : Took %ld sec %ld usec\n",
			kiresnr, sumtv.tv_sec, sumtv.tv_usec);
#else
	printf("Detected %d face(s)", kiresnr);
#endif

	if (kiresnr <= 0) {
		return 0;
	}

    for (result = (struct ysq_oformat *)kires; kiresnr > 0; kiresnr--) {
        rectangle.x = (int)result->rect_x;
        rectangle.y = (int)result->rect_y;
        rectangle.width = (int)result->rect_width;
        rectangle.height = (int)result->rect_heigth;
        fcv_rectangle(vimg, &rectangle, 0, 255, 0);

        if (dolandmark) {
            for (i = 0; i < YSQFD_LANDMARK; i++) {
                point.x = (int)result->landmark[i].x;
                point.y = (int)result->landmark[i].y;
                fcv_point_rgb(vimg, &point, 0, 255, 0);
            }
        }

        result++;
    }

    return 0;
}
