#ifndef OAIDSYS_H
#define OAIDSYS_H
#include <sysdef.h>
#include <list.h>
#include <ysqfd.h>
#include <oaid_base.h>


namespace OAID {

#define OAID_CONFIG_RTMEM_NR    5

enum ALGR
{
    OAID_ALGR_NULL = 0,
    OAID_ALGR_YSQ_FACEDET,
    OAID_ALGR_MAX,
};

enum ALGR_ELEMENT
{
    ELEMENT_NULL = 0,
    ELEMENT_PHOTO,
    ELEMENT_SHOWMARK,
    ELEMENT_MAX,
};

enum ALGR_ELEMENT_VALUE
{
    VALUE_NULL = 0,
    VALUE_TURE,
    VALUE_FALSE,
    VALUE_AUTO,
    VALUE_AUTO_REVERSAL,
    VALUE_MAX,
};


union alg_privconf
{
    struct ysqfd::ysq_fdconfig ysqfd_config;
};

struct alg_config
{
    /* runtime memory regions for this algorithm */
    struct mem_region rtmem[OAID_CONFIG_RTMEM_NR];
    union alg_privconf privconfig;
};

struct alg_resource
{
    void *inbuf;
    void *rtmem;
    void *resbuf;

    int status;
    int valid;
};

struct oaid_algorithm
{
    uint32_t algorithm_id;
    struct list_head list;

    struct alg_config config;
    struct alg_resource resource;

    int (*init)(struct alg_config *config);
    void (*clean)(struct alg_config *config);
    int (*process)(struct alg_config *config, struct alg_resource *resource);
};

class oaidsys : public oaid_base
{
public:

public:
    oaidsys();

    static int config_init_internal(struct alg_config *config);
    static void config_clean_internal(struct alg_config *config);

    static int algorithm_register(struct oaid_algorithm *algorithm);
    static void algorithm_unregister(struct oaid_algorithm *algorithm);
    static struct oaid_algorithm *algorithm_by_id(uint32_t aid);

    static int get_algconfig(uint32_t aid, union alg_privconf *c);
    static int set_algconfig(uint32_t aid, union alg_privconf *c);
    static RV call_algorithm(uint32_t aid, void *indata, void **outpptr);
};

}
#endif // OAIDSYS_H
