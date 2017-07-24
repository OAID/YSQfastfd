#ifndef AAIDALGR_H__
#define AAIDALGR_H__

#include <sysdef.h>
#include <ysqfd.h>
#include <list.h>

#define AAID_CONFIG_RTMEM_NR    3
#define AAID_ALGR_YSQ_FACEDET   0x00000001

union alg_privconf{
	struct ysq_fdconfig ysqfd;
};

struct alg_config
{
	/* runtime memory regions for this algorithm */
	struct mem_region rtmem[AAID_CONFIG_RTMEM_NR];
	union alg_privconf config;
};

struct alg_resource
{
	void *inbuf;
	void *rtmem;
	void *resbuf;

	int status;
	int valid;
};

struct aaid_algorithm
{
	uint32_t algorithm_id;
	struct list_head list;

	struct alg_config config;
	struct alg_resource resource;

	int (*init)(struct alg_config *config);
	void (*clean)(struct alg_config *config);
	int (*process)(struct alg_config *config,
			struct alg_resource *resource);
};

int config_init_internal(struct alg_config *config);
void config_clean_internal(struct alg_config *config);

int algorithm_register(struct aaid_algorithm *algorithm);
void algorithm_unregister(struct aaid_algorithm *algorithm);
struct aaid_algorithm *algorithm_by_id(uint32_t aid);

int get_algconfig(uint32_t aid, struct alg_config *c);
int set_algconfig(uint32_t aid, union alg_privconf *c);
int call_algorithm(uint32_t aid, void *indata, void **outpptr);

#ifdef __cplusplus
extern "C" {
#endif
int priv_ysq_facedetect(struct alg_config *c, struct alg_resource *r);
#ifdef __cplusplus
}
#endif

#endif
