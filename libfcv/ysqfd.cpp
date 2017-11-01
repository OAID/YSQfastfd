#include <time.h>
#include <ysqfd.h>
#include <oaidsys.h>

namespace OAID {

ysqfd::ysqfd()
{
    savephoto = 0;
    showmark = 0;
}

void ysqfd::ysqfd_config_get(int key, int *value)
{
    switch(key)
    {
        case ELEMENT_PHOTO:
            *value = savephoto;
            break;
        case ELEMENT_SHOWMARK:
            *value = showmark;
            break;
    }
}

void ysqfd::ysqfd_config_set(int key, int value)
{
    int temp = value;

    switch(key)
    {
        case ELEMENT_PHOTO:
            if(value == VALUE_AUTO)
                temp = savephoto ? 0 : 1;
            savephoto = temp;
            break;
        case ELEMENT_SHOWMARK:
            if(value == VALUE_AUTO)
                temp = showmark ? 0 : 1;
            showmark = temp;
            break;
    }
}

void ysqfd::ysqfd_initconfig(int vidw, int vidh, struct ysq_fdconfig *config)
{
    config->imgwidth = vidw;
    config->imgheight = vidh;
    config->scale = YSQ_CONFIG_SCALE;
    config->min_neighbors = YSQ_CONFIG_MINNEIGHBORS;
    config->min_object_width = YSQ_CONFIG_MINOBJWDITH;
    config->max_object_width = YSQ_CONFIG_MAXOBJWDITH;
    config->landmark = LANDMARK_ENABLE;

    pr_debug("YSQ FD initialized\n");
}

int ysqfd::algr_ysqfd_init(int imgw, int imgh)
{
    struct oaid_algorithm *algr;
    struct mem_region *rtmem;

    algr = (struct oaid_algorithm *)calloc(1, sizeof(struct oaid_algorithm));
    if (algr == NULL) {
        return -EINVAL;
    }

    rtmem = algr->config.rtmem;

    algr->algorithm_id = OAID_ALGR_YSQ_FACEDET;
    rtmem[0].len = YSQFD_RTMEM_SIZE;
    rtmem[0].base = NULL;
    rtmem[1].len = 0;
    rtmem[1].base = NULL;
    rtmem[2].len = 0;
    rtmem[2].base = NULL;

    algr->init = &oaidsys::config_init_internal;
    algr->clean = &oaidsys::config_clean_internal;
    algr->process = priv_ysq_facedetect;

    algr->resource.status = 0;
    algr->resource.valid  = 0;
    oaidsys::algorithm_register(algr);

    ysqfd_initconfig(imgw, imgh, &algr->config.privconfig.ysqfd_config);

    return 0;
}

void ysqfd::algr_ysqfd_exit(void)
{
    struct oaid_algorithm *algr;

    algr = oaidsys::algorithm_by_id(OAID_ALGR_YSQ_FACEDET);
    if (!algr) {
        return;
    }
    oaidsys::algorithm_unregister(algr);
    free(algr);
}

int ysqfd::ysqfd_process(void *keyinfo, fcvImage *vimg)
{
    int ret, i;
    int kiresnr;
    int *kires;
    fcv_rectangle_t rectangle;
    fcv_point_t point;
    struct ysq_oformat *result;
    int showmark = 0;
    int savephoto = 0;
    time_t tt;
    struct tm  *time_tm;
    char filepath[100];

#ifdef PERF_STATISTICS
    struct timeval pretv, sumtv;
    sumtv.tv_sec = 0;
    sumtv.tv_usec = 0;
    perf_tv_start(&pretv);
#endif

    ret = oaidsys::call_algorithm(OAID_ALGR_YSQ_FACEDET, keyinfo, (void **)&kires);

#ifdef PERF_STATISTICS
    perf_tv_sample(&pretv, &sumtv);
#endif
    if (ret != 0) {
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

    ysqfd_config_get(ELEMENT_PHOTO, &savephoto);
    ysqfd_config_get(ELEMENT_SHOWMARK, &showmark);

    if(savephoto || showmark)
    {
        for (result = (struct ysq_oformat *)kires; kiresnr > 0; kiresnr--)
        {
            rectangle.x = (int)result->rect_x;
            rectangle.y = (int)result->rect_y;
            rectangle.width = (int)result->rect_width;
            rectangle.height = (int)result->rect_heigth;
            if(savephoto)
            {
                time( &tt );
                time_tm = localtime(&tt);

                sprintf(filepath, "./photo-%d-%02d-%02d %02d:%02d:%02d-%d.jpg",
                                    time_tm->tm_year + 1900,
                                    time_tm->tm_mon,
                                    time_tm->tm_mday,
                                    time_tm->tm_hour,
                                    time_tm->tm_mon,
                                    time_tm->tm_sec,
                                    kiresnr);
                fcvimage::fcv_saveface(vimg, &rectangle, filepath);
            }

            if (showmark)
            {
                fcvimage::fcv_rectangle(vimg, &rectangle, 0, 255, 0);
                for (i = 0; i < YSQFD_LANDMARK; i++) {
                    point.x = (int)result->landmark[i].x;
                    point.y = (int)result->landmark[i].y;
                    fcvimage::fcv_point_rgb(vimg, &point, 0, 255, 0);
                }
            }

            result++;
        }
    }

    if(savephoto == VALUE_AUTO_REVERSAL)
        ysqfd_config_set(ELEMENT_PHOTO, VALUE_NULL);

    return 0;
}

}
