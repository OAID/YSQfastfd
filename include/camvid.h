#ifndef CAMVID_H_
#define CAMVID_H_

#include <sysdef.h>
#include <linux/videodev2.h>

#define VIDFB_STATUS_RELEASED 0
#define VIDFB_STATUS_INUSED   1

enum frameimage_size
{
	CAPFRAME_IMAGE_480P = 0,
	CAPFRAME_IMAGE_720P = 1,
};

struct framedata {
	struct v4l2_buffer kvbuf;         /* capture buffer info */
	struct v4l2_pix_format *pixfmt;   /* capture image pixel info */
	uint8_t *base;                    /* capture buffer based address mapped in userspace */
	int length;                       /* capture buffer lenght, the frame size */
};
typedef struct framedata captureCamera;

#define VID_CAPBUF_CNT    2
#define VID_CAPTURE_OFF   0
#define VID_CAPTURE_ON    1

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

	int (*open)(struct capturedev *camdev);
	int (*init)(struct capturedev *camdev);

	void (*close)(struct capturedev *camdev);
	void (*stream_on)(struct capturedev *camdev);
	void (*stream_off)(struct capturedev *camdev);
	void (*imagesize)(struct capturedev *camdev, int *iw, int *ih);
	struct framedata *(*capture)(struct capturedev *camdev);
};

int create_vidcapture(int id);
void destroy_vidcapture(int id);
void query_vidimgsize(int id, int *iw, int *ih);
struct framedata *capturevid(int id);

#endif
