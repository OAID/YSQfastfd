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

  AAID algorithm related functions
  Authour : Haibing Xu <hxu@openailab.com>
*/

#include <stdlib.h>
#include <stdint.h>
#include <list.h>
#include <aaidalgr.h>

/* all the 3rd algorithms is list on this head */
LIST_HEAD(algrshead);

struct aaid_algorithm *algorithm_by_id(uint32_t aid)
{
	struct list_head *nl;
	struct list_head *ll;
	struct aaid_algorithm *a;

	list_for_each_safe(ll, nl, &algrshead) {
		a = list_entry(ll, struct aaid_algorithm, list);
		if (aid == a->algorithm_id) {
			return a;
		}
	}
	return NULL;
}

void config_clean_internal(struct alg_config *config)
{
	int i;
	struct mem_region *rtmem;
	rtmem = config->rtmem;

	for (i = 0; i < AAID_CONFIG_RTMEM_NR; i++) {
		if (rtmem[i].len > 0) {
			if (rtmem[i].base) {
				pr_debug("alg_config_clean rtmem %d : 0x%lx@%p\n",
							i, rtmem[i].len, rtmem[i].base);
				free(rtmem[i].base);
				rtmem[i].base = NULL;
			}
		}
	}
}

int config_init_internal(struct alg_config *config)
{
	int i;
	struct mem_region *rtmem;
	rtmem = config->rtmem;

	for (i = 0; i < AAID_CONFIG_RTMEM_NR; i++) {
		if (rtmem[i].len > 0) {
			rtmem[i].base = malloc(rtmem[i].len);
			if (rtmem[i].base == NULL) {
				return -ENOMEM;
			}
			pr_debug("alg_config_init rtmem %d  : 0x%lx@%p\n",
					i, rtmem[i].len, rtmem[i].base);
		}
	}
	return 0;
}

int algorithm_register(struct aaid_algorithm *algorithm)
{
	int ret;
	struct alg_config *config;

	pr_debug("algorithm register - algorithm id : %d\n",
					algorithm->algorithm_id);

	config = &algorithm->config;
	ret = 0;

	INIT_LIST_HEAD(&algorithm->list);

	if (algorithm->init) {
		ret = algorithm->init(config);
		if (ret < 0) {
			if (algorithm->clean) {
				algorithm->clean(config);
			} else {
				config_clean_internal(config);
			}
			goto out;
		}
	}

	algorithm->resource.status = 1;
	algorithm->resource.valid  = 1;

	list_add(&algorithm->list, &algrshead);

out:
	return ret;
}

void algorithm_unregister(struct aaid_algorithm *algorithm)
{
	pr_debug("algorithm unregister - algorithm id : %d\n",
				algorithm->algorithm_id);
	list_del(&algorithm->list);

	if (algorithm->clean)
		algorithm->clean(&algorithm->config);
}

int get_algconfig(uint32_t aid, struct alg_config *c)
{
	struct aaid_algorithm *algorithm;
	int csz;

	csz = (int)sizeof(struct alg_config);

	algorithm = algorithm_by_id(aid);
	if (!algorithm) {
		fprintf(stderr, "algorithm is not registered, id(%d)\n", aid);
		return -EINVAL;
	}

	memcpy(c, &algorithm->config, csz);
	return csz;
}

int set_algconfig(uint32_t aid, union alg_privconf *c)
{
	struct aaid_algorithm *algorithm;
	int csz;

	csz = (int)sizeof(union alg_privconf);

	algorithm = algorithm_by_id(aid);
	if (!algorithm) {
		fprintf(stderr, "algorithm is not registered, id(%d)\n", aid);
		return -EINVAL;
	}

	memcpy(&(algorithm->config.config), c, csz);

	pr_debug("algorithm id %d private config size %d\n", aid, csz);

	return csz;
}

int call_algorithm(uint32_t aid, void *indata, void **outpptr)
{
	struct aaid_algorithm *algorithm;

	algorithm = algorithm_by_id(aid);
	if (!algorithm) {
		fprintf(stderr, "algorithm is not registered, id(%d)\n", aid);
		return -EINVAL;
	}

	algorithm->resource.inbuf = indata;
	algorithm->process(&algorithm->config, &algorithm->resource);

	if (outpptr) {
		*outpptr = algorithm->resource.resbuf;
	}
	return 0;
}
