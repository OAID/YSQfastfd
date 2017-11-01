#include "video.h"
#include <sys/ioctl.h>
#include <sys/mman.h>

namespace OAID {

video::video()
{
    opened_camid = 0;
    memset(camdev_list, 0, sizeof(struct capturedev *)*MAX_OPENED_CAM);
}

/* Start stream frame capture, open video stream */
void video::stream_on(struct capturedev *camdev)
{
    if (camdev->status == VID_CAPTURE_ON)
        return;

    ioctl(camdev->fd, VIDIOC_STREAMON, &(camdev->bufreqs.type));
    camdev->status = VID_CAPTURE_ON;
}

/* stop stream frame capture, close video stream */
void video::stream_off(struct capturedev *camdev)
{
    if (camdev->status == VID_CAPTURE_OFF)
        return;

    /* Close frame stream capture */
    ioctl(camdev->fd, VIDIOC_STREAMOFF, &(camdev->bufreqs.type));
    camdev->status = VID_CAPTURE_OFF;
}

/*
 * Capture frame on video input device
 * it returns kernel mapped memory to caller, application
 * can use the memory directory
 */
captureCamera * video::stream_capture(struct capturedev *camdev)
{
    int ret;
    int deqbuf_id;
    struct v4l2_buffer buf;

    buf.type   = camdev->bufreqs.type;
    buf.memory = camdev->bufreqs.memory;
    ret = ioctl(camdev->fd, VIDIOC_DQBUF, &buf);
    if (ret < 0) {
        assert_failure();
        goto err;
    }

    deqbuf_id = buf.index;

    /*
     * queque the other frame buffer into the driver
     * one buffer id is '0', and the other id is '1'
     */
    buf.index = deqbuf_id ? 0 : 1;
    ret = ioctl(camdev->fd, VIDIOC_QBUF, &buf);
    if (ret < 0) {
        assert_failure();
    }

    return &(camdev->framemem[deqbuf_id]);

err:
    return NULL;
}

void video::show_imagfmtinfo(struct v4l2_format *imagefmt)
{
    printf("Video stream image info\n");
    printf("  Image pixel  : %d x %d\n",
        imagefmt->fmt.pix.width, imagefmt->fmt.pix.height);
    printf("  Pixel format : (hex : %08X) - %c%c%c%c\n",imagefmt->fmt.pix.pixelformat,
        (imagefmt->fmt.pix.pixelformat) & 0xFF,
        (imagefmt->fmt.pix.pixelformat >> 8) & 0xFF,
        (imagefmt->fmt.pix.pixelformat >> 16) & 0xFF,
        (imagefmt->fmt.pix.pixelformat >> 24) & 0xFF);
    pr_debug("  Image field  : %d\n", imagefmt->fmt.pix.field);
    pr_debug("  Bytes perline: %d\n", imagefmt->fmt.pix.bytesperline);
    pr_debug("  Image size   : %d\n", imagefmt->fmt.pix.sizeimage);
    pr_debug("  Color space  : %d\n", imagefmt->fmt.pix.colorspace);
    pr_debug("  Image flags  : %d\n", imagefmt->fmt.pix.flags);
    pr_debug("  Image cr_enc : %d\n", imagefmt->fmt.pix.ycbcr_enc);
    pr_debug("  Quantization : %d\n", imagefmt->fmt.pix.quantization);
    pr_debug("  Xfer_func    : %d\n", imagefmt->fmt.pix.xfer_func);
}

void video::show_streamparm(struct v4l2_streamparm *stream_parm)
{
    struct v4l2_fract *timeframe;
    timeframe = &(stream_parm->parm.capture.timeperframe);

    pr_debug("  Capabmode    : %08x\n", stream_parm->parm.capture.capability);
    pr_debug("  Capturemode  : %08x\n", stream_parm->parm.capture.capturemode);
    pr_debug("  Extendedmode : %08x\n", stream_parm->parm.capture.extendedmode);
    printf("  Frame/Time   : %d/%d\n", timeframe->denominator,timeframe->numerator);
}

/*
 * Set the capture video device image format
 * NOTE : default pixelformat is YUYV
 */
void video::pixformt_setting(int vidsize, uint32_t fourcc, struct v4l2_format *imagefmt)
{
    if (fourcc != V4L2_PIX_FMT_YUYV) {
        fprintf(stderr, "Unspoort pixel format : %c%c%c%c\n",
                (fourcc) & 0xFF, (fourcc >> 8) & 0xFF,
                (fourcc >> 16) & 0xFF, (fourcc >> 24) & 0xFF);
        return;
    }
    imagefmt->fmt.pix.pixelformat = fourcc;

    switch(vidsize) {
        case CAPFRAME_IMAGE_480P :
            imagefmt->fmt.pix.width = 640;
            imagefmt->fmt.pix.height = 480;
            break;
        case CAPFRAME_IMAGE_720P :
            imagefmt->fmt.pix.width = 1280;
            imagefmt->fmt.pix.height = 720;
            break;
        default :
            fprintf(stderr, "Unsupport image size, use defaults\n");
            break;
    }
}

int video::videodev_init(struct capturedev *camdev)
{
    uint32_t i;
    int ret;
    struct v4l2_streamparm stream_parm;
    struct v4l2_format fmtsetting;

    struct v4l2_requestbuffers *bufreqs;
    struct v4l2_format *imagefmt;
    struct v4l2_buffer *kvbuf;
    struct framedata *capturebuf;

    bufreqs = &camdev->bufreqs;
    imagefmt = &camdev->imagefmt;

    memset(imagefmt, 0x0, sizeof(struct v4l2_format));
    memset(&stream_parm, 0x0, sizeof(struct v4l2_streamparm));

    /* Query current stream data (image) format */
    fmtsetting.type = bufreqs->type;
    ioctl(camdev->fd, VIDIOC_G_FMT, &fmtsetting);
    show_imagfmtinfo(&fmtsetting);

    /*
     * set vidoe frame format and parameters
     * 1. Try to configurate with new setting, configure
     *    hardware with new configuration if it supports
     * 2. otherwise use the default setting
     */
    pixformt_setting(camdev->vidsize, camdev->vidfmt, &fmtsetting);

    ret = ioctl(camdev->fd, VIDIOC_TRY_FMT, &fmtsetting);
    if (ret < 0) {
        if( errno == EINVAL) {
            fprintf(stderr, "[*%s] does not support settings :\n"
            "  resolution   : %d x %d\n"
            "  pixel format : (Hex : %08X)\n",
                camdev->name, fmtsetting.fmt.pix.width, fmtsetting.fmt.pix.height,
                fmtsetting.fmt.pix.pixelformat);
                pixformt_setting(DEFAULT_VIDSIZE, DEFAULT_VIDFMT, &fmtsetting);
                camdev->vidsize = DEFAULT_VIDSIZE;
                camdev->vidfmt  = DEFAULT_VIDFMT;
        }
    }

    ret = ioctl(camdev->fd, VIDIOC_S_FMT, &fmtsetting);
    if (ret < 0) {
        assert_failure();
        goto err;
    }

    /* Query current stream data (image) format after setting enabled */
    imagefmt->type = bufreqs->type;
    ioctl(camdev->fd, VIDIOC_G_FMT, imagefmt);
    show_imagfmtinfo(imagefmt);

    /*
     * Query stream parameters
     * V4L2_MODE_HIGHQUALITY : 0x0001
     * V4L2_CAP_TIMEPERFRAME : 0x1000
     */
    stream_parm.type = bufreqs->type;
    ioctl(camdev->fd, VIDIOC_G_PARM, &stream_parm);
    show_streamparm(&stream_parm);

    /* Request buffer in kernel for frame capture */
    ret = ioctl(camdev->fd, VIDIOC_REQBUFS, bufreqs);
    if (ret < 0) {
        assert_failure();
        goto err;
    }

    /*
     * 1. Query the driver allocated frame buffer information
     * 2. Map driver allocated kernel memory into userpace
     * 3. Queue the frame buffer to v4l2 driver after the mapping
     */
    capturebuf = camdev->framemem;
#define MMAP_PORTS (PROT_READ|PROT_WRITE)
    for(i = 0; i < bufreqs->count; i++){
        memset(capturebuf, 0x0, sizeof(struct framedata));
        kvbuf = &capturebuf->kvbuf;
        memset(kvbuf, 0x0, sizeof(struct v4l2_buffer));

        kvbuf->index = i;
        kvbuf->type = bufreqs->type;
        kvbuf->memory = bufreqs->memory;
        ret = ioctl(camdev->fd, VIDIOC_QUERYBUF, kvbuf);
        if (ret < 0) {
            goto kmfree;
        }
        capturebuf->base  = (uint8_t*)mmap(NULL, kvbuf->length,
                MMAP_PORTS, MAP_SHARED, camdev->fd, kvbuf->m.offset);
        if (capturebuf->base == MAP_FAILED) {
            goto kmfree;
        }
        capturebuf->length = kvbuf->length;
        capturebuf->pixfmt = &(imagefmt->fmt.pix);
        capturebuf++;
    }

    /* Queue the first (index = 0) capture frame buffer into the driver */
    kvbuf = &(camdev->framemem[0].kvbuf);
    ret = ioctl(camdev->fd, VIDIOC_QBUF, kvbuf);
    if (ret < 0) {
            goto kmfree;
    }
    pr_debug("Init queued frame buffer id : %d\n",kvbuf->index);

    return 0;

kmfree:
    perror(__func__);
    /* set request count zerot to free allocated memory by v4l2 driver */
    bufreqs->count = 0;
    ioctl(camdev->fd, VIDIOC_REQBUFS, bufreqs);
err:
    perror(__func__);
    if (camdev->fd > 0) {
        close(camdev->fd);
    }
    return ret;
}

/*
 * open camera function, it is called during capture
 * device create operation.
 */
int video::camdev_open(struct capturedev *camdev)
{
    int ret;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_standard standdesc;
    struct v4l2_capability devcapb;

    memset(&devcapb, 0x0, sizeof(struct v4l2_capability));
    memset(&fmtdesc, 0x0, sizeof(struct v4l2_fmtdesc));

    camdev->bufreqs.count = VID_CAPBUF_CNT;
    camdev->bufreqs.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    camdev->bufreqs.memory = V4L2_MEMORY_MMAP;

    camdev->fd = open(camdev->name, OPENDEV_FLAGS, 0);
    if (camdev->fd < 0) {
        fprintf(stderr, "cannot open device %s\n", camdev->name);
        goto out;
    }

    /* Query video device supported standard */
    ret = ioctl(camdev->fd, VIDIOC_QUERYSTD, &camdev->std);
    if (ret < 0){
        printf("Cannot probe the device \"%s\"standard!\n", camdev->name);
    } else {
        printf("%s standard(Hex): %llX\n", camdev->name, camdev->std);
    }

    /* Enumlate the supportted standard list */
    printf("%s support standard list  :\n", camdev->name);
    standdesc.index = 0;
    while (ioctl(camdev->fd, VIDIOC_ENUMSTD, &standdesc) != -1) {
        printf("  <%d> :  %s\n"
            "    stdid      : %llX\n"
            "    frameperiod: %d/%d\n"
            "    framelines : %d\n",
            standdesc.index, standdesc.name, standdesc.id,
            standdesc.frameperiod.numerator,
            standdesc.frameperiod.denominator,
            standdesc.framelines);
        standdesc.index++;
    }

    /* Emulate support video support format */
    printf("%s support video format list  :\n", camdev->name);
    fmtdesc.type = camdev->bufreqs.type;
    fmtdesc.index = 0;
    while (ioctl(camdev->fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
        printf("  <%d> :  %s\n"
            "    type        : %d\n"
            "    flag        : %d\n"
            "    pixelformat : %08X\n",
            fmtdesc.index, fmtdesc.description,
            fmtdesc.type, fmtdesc.flags, fmtdesc.pixelformat);
        fmtdesc.index++;
    }

    /* Query video device capacibility */
    ioctl(camdev->fd, VIDIOC_QUERYCAP, &devcapb);
    pr_debug("%s capacibility information\n", camdev->name);
    pr_debug("  Driver name  : %s\n", devcapb.driver);
    pr_debug("  Card name    : %s\n", devcapb.card);
    pr_debug("  Bus info     : %s\n", devcapb.bus_info);
    pr_debug("  Version      : %u\n", devcapb.version);
    pr_debug("  Capabilities : %08X\n", devcapb.capabilities);
    pr_debug("  Device Caps  : %08X\n", devcapb.device_caps);

    /* check whether the device support capture */
    if (!devcapb.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        printf("%s does not support video capture\n", camdev->name);
        goto err;
    }
    camdev->status = VID_CAPTURE_OFF;

    return camdev->fd;
err:
    close(camdev->fd);
    camdev->fd = -EINVAL;
out:
    return camdev->fd;
}

/*
 * release all resources allocated for the capture device
 * it is called before close the device
 */
void video::resources_release(struct capturedev *camdev)
{
    uint32_t i;
    struct v4l2_requestbuffers *bufreqs;

    if (!camdev->framemem[0].base) {
        return;
    }

    bufreqs = &camdev->bufreqs;

    pr_debug("Munmap capture frame buffer : %d\n", bufreqs->count);
    for (i = 0; i < bufreqs->count; i++) {
        munmap(camdev->framemem[i].base, camdev->framemem[i].length);
        camdev->framemem[i].pixfmt = NULL;
        camdev->framemem[i].base = NULL;
        camdev->framemem[i].length = 0;
    }

    pr_debug("Free kernel frame buffer : %d\n", bufreqs->count);
    bufreqs->count = 0;
    ioctl(camdev->fd, VIDIOC_REQBUFS, bufreqs);
}

void video::camdev_close(struct capturedev *camdev)
{
    if(camdev->fd < 0)
        return;

    resources_release(camdev);
    close(camdev->fd);
    camdev->fd = -EINVAL;
}

/*
 * Query the video input device image size
 */
void video::camdev_imagesize(struct capturedev *camdev, int *width, int *height)
{
    if(camdev->fd > 0) {
        *width = camdev->imagefmt.fmt.pix.width;
        *height = camdev->imagefmt.fmt.pix.height;
    } else {
        *width = 0;
        *height = 0;
    }
}

int video::create_capturedev(int id)
{
    struct capturedev *thiscam;

    if (id >= MAX_OPENED_CAM) {
        assert_failure();
        return -EBUSY;
    }

    if (camdev_list[id]) {
        assert_failure();
        fprintf(stderr, "video device id %d opened\n", id);
        return -EBUSY;
    }

    thiscam = (struct capturedev *)calloc(1, sizeof(struct capturedev));
    if (thiscam == NULL) {
        assert_failure();
        return -ENOMEM;
    }

#ifdef VIDSIZE_SETTING
    thiscam->vidsize = VIDSIZE_SETTING;
#else
    thiscam->vidsize = DEFAULT_VIDSIZE;
#endif
    thiscam->vidfmt  = DEFAULT_VIDFMT;
    snprintf(thiscam->name, MAX_NAME_STRLEN, "/dev/video%d", id);
    camdev_list[id] = thiscam;
    opened_camid++;

    thiscam->id = id;
    thiscam->fd = -EINVAL;
    return thiscam->id;
}

/*
 * Create video capture device by id
 * @id : the system video device id (camera id)
 * return opened device fd on sccuess
 */
int video::create_vidcapture(int id)
{
    int ret;
    struct capturedev * camdev;

    ret = create_capturedev(id);
    if (ret < 0){
        assert_failure();
        goto err;
    }

    camdev = camdev_list[id];

    ret = camdev_open(camdev);
    if (ret < 0) {
        assert_failure();
        goto err;
    }

    ret = videodev_init(camdev);
    if (ret < 0) {
        assert_failure();
        goto err;
    }

    stream_on(camdev);

    return camdev->fd;

err :
    return ret;
}

void video::destroy_vidcapture(int id)
{
    struct capturedev * camdev;
    camdev = camdev_list[id];
    if (camdev == NULL) {
        fprintf(stderr, "No acatived device id is %d\n", id);
        return;
    }

    stream_off(camdev);
    camdev_close(camdev);

    free(camdev);
    camdev_list[id] = NULL;
}

/*
 * Capture frame on video device that specifed by id
 */
struct framedata *video::capturevid(int id)
{
    struct capturedev * camdev;

    camdev = camdev_list[id];
    if (camdev == NULL) {
        fprintf(stderr, "No acatived device id is %d\n", id);
        return NULL;
    }

    return stream_capture(camdev);
}

/*
 * Query the video input device image size information by id
 */
void video::query_vidimgsize(int id, int *iw, int *ih)
{
    struct capturedev * camdev;

    camdev = camdev_list[id];
    if (camdev == NULL) {
        return;
    }
    camdev_imagesize(camdev, iw, ih);
}


}
