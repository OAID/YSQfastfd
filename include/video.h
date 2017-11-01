#ifndef VIDEO_H
#define VIDEO_H

#include <oaid_base.h>
#include <sysdef.h>
#include <linux/videodev2.h>

namespace OAID {

#define VIDFB_STATUS_RELEASED 0
#define VIDFB_STATUS_INUSED   1
#define VID_CAPBUF_CNT    2
#define VID_CAPTURE_OFF   0
#define VID_CAPTURE_ON    1

#ifdef CAM_FRAME_720P
# define DEFAULT_VIDSIZE   CAPFRAME_IMAGE_720P
#else
# define DEFAULT_VIDSIZE   CAPFRAME_IMAGE_480P
#endif

#define DEFAULT_VIDFMT    V4L2_PIX_FMT_YUYV
#define OPENDEV_FLAGS     (O_RDWR)

#define MAX_NAME_STRLEN      32
#define MAX_OPENED_CAM       8

enum frameimage_size
{
    CAPFRAME_IMAGE_480P = 0,
    CAPFRAME_IMAGE_720P = 1,
};

class video : public oaid_base
{
public:

    struct capturedev {
        char name[MAX_NAME_STRLEN];        /* device identification */

        v4l2_std_id std;                   /* device standard */
        enum v4l2_buf_type buftype;        /* capture buffer type */
        enum frameimage_size vidsize;      /* video stream image size */
        uint32_t vidfmt;                   /* video stream image format */

        struct v4l2_format imagefmt;       /* stream frame format information */
        struct v4l2_requestbuffers bufreqs;

        /* 2 capture buffer to each video device */
        struct framedata framemem[VID_CAPBUF_CNT];
        int status;
        int id;                             /* id of this capture device */
        int fd;                             /* opened device file descriptor */
    };

    /*
     * v4l2 compatible camera device description
     */
    int opened_camid;
    struct capturedev *camdev_list[MAX_OPENED_CAM];

public:
    video();
    void stream_on(struct capturedev *camdev);
    void stream_off(struct capturedev *camdev);
    int videodev_init(struct capturedev *camdev);
    int camdev_open(struct capturedev *camdev);
    void camdev_close(struct capturedev *camdev);
    void resources_release(struct capturedev *camdev);
    void camdev_imagesize(struct capturedev *camdev, int *width, int *height);
    int create_capturedev(int id);
    int create_vidcapture(int id);
    void destroy_vidcapture(int id);
    void query_vidimgsize(int id, int *iw, int *ih);
    struct framedata *capturevid(int id);
    captureCamera * stream_capture(struct capturedev *camdev);
    void show_imagfmtinfo(struct v4l2_format *imagefmt);
    void show_streamparm(struct v4l2_streamparm *stream_parm);
    void pixformt_setting(int vidsize, uint32_t fourcc, struct v4l2_format *imagefmt);

    static void test()
    {}

};

}

#endif // VIDEO_H
