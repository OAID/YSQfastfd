#include "oaidsys.h"

namespace OAID {

/* all the 3rd algorithms is list on this head */
LIST_HEAD(algrshead);

oaidsys::oaidsys()
{

}

struct oaid_algorithm *oaidsys::algorithm_by_id(uint32_t aid)
{
    struct list_head *nl;
    struct list_head *ll;
    struct oaid_algorithm *a;

    list_for_each_safe(ll, nl, &algrshead) {
        a = list_entry(ll, struct oaid_algorithm, list);
        if (aid == a->algorithm_id) {
            return a;
        }
    }
    return NULL;
}

void oaidsys::config_clean_internal(struct alg_config *config)
{
    int i;
    struct mem_region *rtmem;
    rtmem = config->rtmem;

    for (i = 0; i < OAID_CONFIG_RTMEM_NR; i++) {
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

int oaidsys::config_init_internal(struct alg_config *config)
{
    int i;
    struct mem_region *rtmem;
    rtmem = config->rtmem;

    for (i = 0; i < OAID_CONFIG_RTMEM_NR; i++) {
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

int oaidsys::algorithm_register(struct oaid_algorithm *algorithm)
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

void oaidsys::algorithm_unregister(struct oaid_algorithm *algorithm)
{
    pr_debug("algorithm unregister - algorithm id : %d\n",
                algorithm->algorithm_id);
    list_del(&algorithm->list);

    if (algorithm->clean)
        algorithm->clean(&algorithm->config);
}

int oaidsys::get_algconfig(uint32_t aid, union alg_privconf *c)
{
    struct oaid_algorithm *algorithm;
    int csz;

    csz = (int)sizeof(union alg_privconf);

    algorithm = algorithm_by_id(aid);
    if (!algorithm) {
        fprintf(stderr, "algorithm is not registered, id(%d)\n", aid);
        return -EINVAL;
    }

    memcpy(c, &(algorithm->config.privconfig), csz);
    return 0;
}

int oaidsys::set_algconfig(uint32_t aid, union alg_privconf *c)
{
    struct oaid_algorithm *algorithm;
    int csz;

    csz = (int)sizeof(union alg_privconf);

    algorithm = algorithm_by_id(aid);
    if (!algorithm) {
        fprintf(stderr, "algorithm is not registered, id(%d)\n", aid);
        return -EINVAL;
    }

    memcpy(&(algorithm->config.privconfig), c, csz);

    pr_debug("algorithm id %d private config size %d\n", aid, csz);

    return 0;
}

RV oaidsys::call_algorithm(uint32_t aid, void *indata, void **outpptr)
{
    int ret = 0;
    struct oaid_algorithm *algorithm;

    algorithm = algorithm_by_id(aid);
    if (!algorithm) {
        fprintf(stderr, "algorithm is not registered, id(%d)\n", aid);
        return (RV)EINVAL;
    }

    algorithm->resource.inbuf = indata;
    ret = algorithm->process(&algorithm->config, &algorithm->resource);

    if (outpptr) {
        *outpptr = algorithm->resource.resbuf;
    }
    return (RV)ret;
}

}
