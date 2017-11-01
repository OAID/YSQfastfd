#ifndef WINDOW_H
#define WINDOW_H

#include <sysdef.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <fcvimage.h>
#include <oaid_base.h>

namespace OAID {

#define GTKINIT_DONE           1

#define MAX_OPENED_WINDOW      10
#define WINDOW_DEFNAME         "openailab"
#define WINDOW_NORMAL          0x00000001
#define WINDOW_AUTOSIZE        0x00000002
#define WINDOW_DEFWIDTH        320           /* window normal width */
#define WINDOW_DEFHEIGHT       240           /* window normal height */
#define WAITKEY_TIMEOUT        1000
#define WINDOW_QUITKEY         'q'
#define WINDOW_PHOTOKEY        'p'
#define WINDOW_SHOWKEY         's'

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

class window : public oaid_base
{
public:
    int gtk_initialized;
    struct window_desc _windows;

public:
    window();

    struct vwindow * window_byname(const char *name, int *wid);
    void __vwindow_attach(struct vwindow *window);
    void __vwindow_deattach(struct vwindow *window);
    void rectangle_onwindow(cairo_t *cr, gpointer data);
    void text_osd_onwindow(cairo_t *cr, gpointer data);

    void vwindow_internal_init(struct vwindow *vwin);
    void window_set_icon(GtkWindow *window);
    void named_window(const char *name, int flags);
    void resize_window(const char *name, int width, int height);

    void move_window(const char *name, int x, int y);
    void destroy_window(const char *name);

    void imageshow(const char *name, fcvImage *fcvimg);
    void imageshow_ops(const char *name, fcvImage *fcvimg,
                windowOperationFn opfunction, gpointer opfunc_data);

    static gboolean do_wait(gpointer data);
    static int waitkey(int msec);
    static void keyboard_press(GtkWidget *widget, GdkEventKey *event, gpointer data);
    static gboolean delete_window(GtkWidget *widget, GdkEvent *event, gpointer data);
    static gboolean do_drawing(GtkWidget *widget, cairo_t *cr, gpointer data);
};

}

#endif // WINDOW_H
