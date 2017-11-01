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
*/

#include <stdint.h>
#include <stdlib.h>
#include <ysqfd.h>
#include <oaidsys.h>
#include <midlayer.h>

namespace OAID {

#ifdef __cplusplus
extern "C" {
#endif

int priv_ysq_facedetect(struct alg_config *c, struct alg_resource *r)
{
    struct ysqfd::ysq_fdconfig *conf;

	pr_debug("Call YSQ FD algorithm\n");

    conf = (struct ysqfd::ysq_fdconfig *)&c->privconfig.ysqfd_config;
	r->rtmem = c->rtmem[0].base;

	pr_debug("YSQ FD process info :\n"
			"  data in ptr     : %p\n"
			"  data out ptr    : %p\n"
			"  runtime memory  : %p\n",
			r->inbuf, r->resbuf, r->rtmem);

	pr_debug("YSQ FD config info :\n"
			"  image width      : %d\n"
			"  image height     : %d\n"
            "  min_neighbors    : %d\n"
			"  min_object_width : %d\n"
			"  max_object_width : %d\n"
			"  step     : %d\n"
			"  landmark : %d\n"
			"  scale    : %f\n",
			conf->imgwidth, conf->imgheight,
			conf->min_neighbors, conf->min_object_width,
			conf->max_object_width, conf->step,
			conf->landmark, conf->scale);

	r->resbuf = facedetect_frontal((unsigned char *)r->rtmem, (unsigned char *)r->inbuf,
                    conf->imgwidth, conf->imgheight, conf->imgwidth,
					conf->scale, conf->min_neighbors,
					conf->min_object_width, conf->max_object_width,
					conf->landmark);

	if (r->resbuf == NULL)
        return -EINVAL;

    return 0;
}


#ifdef __cplusplus
}
#endif


}
