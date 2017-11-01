#ifndef MIDLAYER_H__
#define MIDLAYER_H__

#include <sysdef.h>
#include <algorithm_adp/libyushiqiface.h>

namespace OAID {


#ifdef __cplusplus
extern "C" {
#endif

int priv_ysq_facedetect(struct alg_config *c, struct alg_resource *r);


#ifdef __cplusplus
}
#endif

}

#endif
