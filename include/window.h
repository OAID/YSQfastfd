#ifndef WINDOW_H__
#define WINDOW_H__

#include <sysdef.h>
#include <cairo.h>
#include <gtk/gtk.h>  
#include <fcvimage.h>

#define GTKINIT_DONE           1

#define MAX_OPENED_WINDOW      10
#define WINDOW_DEFNAME         "openailab"
#define WINDOW_NORMAL          0x00000001
#define WINDOW_AUTOSIZE        0x00000002
#define WINDOW_DEFWIDTH        320           /* window normal width */
#define WINDOW_DEFHEIGHT       240           /* window normal height */
#define WAITKEY_TIMEOUT        1000
#define WINDOW_QUITKEY         'q'

typedef void (* windowOperationFn)(cairo_t *cr, gpointer data);

struct vwindow {
	char name[MAX_NAME_STRLEN];
	GtkWidget *management;
	GtkWidget *drawingerea;
	GdkPixbuf *pixbuf;

	windowOperationFn operationfunc;
	gpointer *opfunc_data;

	uint32_t flags;
};

struct window_desc {
	int nr_max;
	int nr_free;
	struct vwindow *list[MAX_OPENED_WINDOW];
};

void rectangleOnWindow(cairo_t *cr, gpointer data);

void named_window(const char *name, int flags);
void resize_window(const char *name, int width, int height);
void move_window(const char *name, int x, int y);
void destroy_window(const char *name);
int waitkey(int msec);

void imageshow(const char *name, fcvImage *fcvimg);
void imageshow_ops(const char *name, fcvImage *fcvimg,
			windowOperationFn opfunction, gpointer opfunc_data);

#endif
