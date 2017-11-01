#include "window.h"

namespace OAID {

#define vwindow_creatable() ((_windows.nr_free > 0) ? TRUE : FALSE)
volatile int keyval = 0;

window::window()
{
    gtk_initialized = 0;
    memset(&_windows, 0 ,sizeof(struct window_desc));
    _windows.nr_max = MAX_OPENED_WINDOW;
    _windows.nr_free = MAX_OPENED_WINDOW;
}

gboolean window::do_wait(gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

/* wait keyboard input events */
int window::waitkey(int msec)
{
    int key = 0;
    msec = (msec > WAITKEY_TIMEOUT) ? WAITKEY_TIMEOUT : msec;
    g_timeout_add(msec, (GSourceFunc)&window::do_wait, NULL);
    gtk_main();
    key = keyval;
    keyval = 0;
    return key;
}

void window::keyboard_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    keyval = (char)event->keyval;
}

gboolean window::delete_window(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    keyval = WINDOW_QUITKEY;
    return TRUE;
}

gboolean window::do_drawing(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    struct vwindow *vwin;
    GdkPixbuf *srcpixbuf;
    int w, h;

    vwin = (struct vwindow *)data;
    if (!vwin->pixbuf) {
        return FALSE;
    }

    srcpixbuf = (GdkPixbuf *)vwin->pixbuf;

    w = gtk_widget_get_allocated_width(widget);
    h = gtk_widget_get_allocated_height(widget);
    pr_debug("Dispaly area size : %d x %d\n", w, h);

    /*
     * create GdkPixbuf of window (pixbuf field) according to
     * the window size, it will be release when imageshow is
     * called or window destory routine is called
     */
    vwin->pixbuf = gdk_pixbuf_scale_simple(srcpixbuf,
                    w, h, GDK_INTERP_BILINEAR);

    gdk_cairo_set_source_pixbuf(cr, vwin->pixbuf, 0, 0);
    cairo_paint(cr);

    if (vwin->operationfunc)
        vwin->operationfunc(cr, vwin->opfunc_data);

    /* srcpixbuf is created in imageshow */
    g_object_unref(srcpixbuf);
    return FALSE;
}

struct vwindow *window::window_byname(const char *name, int *wid)
{
    int id;
    for (id = 0; id < _windows.nr_max; id++)
    {
        if (_windows.list[id])
        {
            if (strcmp(_windows.list[id]->name, name) == 0)
            {
                if (wid) *wid = id;
                return _windows.list[id];
            }
        }
    }
    return NULL;
}

void window::__vwindow_attach(struct vwindow *window)
{
    int id;
    for (id = 0; id < _windows.nr_max; id++) {
        if (_windows.list[id] == NULL) {
            _windows.list[id] = window;
            _windows.nr_free -= 1;
            break;
        }
    }
}

void window::__vwindow_deattach(struct vwindow *window)
{
    int id;

    for (id = 0; id < _windows.nr_max; id++) {
        if (_windows.list[id] == window) {
            _windows.list[id] = NULL;
            _windows.nr_free += 1;
            break;
        }
    }
}

void window::rectangle_onwindow(cairo_t *cr, gpointer data)
{
    fcv_rectangle_t *rect;

    rect = (fcv_rectangle_t *)data;

    cairo_set_source_rgb(cr, 0, 0, 255);
    cairo_rectangle(cr, (double)rect->x, (double)rect->y,
                    rect->width, rect->height);
    cairo_stroke(cr);
}

void window::text_osd_onwindow(cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.627, 0, 0);
    cairo_set_font_size(cr, 10.0);
    cairo_move_to(cr, 50.0, 130.0);
    cairo_show_text(cr, "OPEN AI LAB");
}



void window::vwindow_internal_init(struct vwindow *vwin)
{
    vwin->management = NULL;
    vwin->drawingerea = NULL;
    vwin->pixbuf = NULL;
    vwin->operationfunc = NULL;
    vwin->opfunc_data = NULL;
}

#if 0
#define WINDOW_ICON    "./icon/openailab.png"
void window::window_set_icon(GtkWindow *window)
{
    GdkPixbuf *icon;

    icon = gdk_pixbuf_new_from_file(WINDOW_ICON, NULL);
    if (icon == NULL) {
        printf("Error -- %d@%s\n", __LINE__, __func__);
        return;
    }
    gtk_window_set_icon(window, icon);
}
#else
void window::window_set_icon(GtkWindow *window){}
#endif

void window::named_window(const char *name, int flags)
{
    struct vwindow *vwin;

    if (!vwindow_creatable()) {
        assert_failure();
        return;
    }

    vwin = (struct vwindow *)calloc(1, sizeof(struct vwindow));
    if (NULL == vwin) {
        assert_failure();
        return;
    }

    vwindow_internal_init(vwin);

    if (name) {
        snprintf(vwin->name, MAX_NAME_STRLEN,"%s", name);
    } else {
        snprintf(vwin->name, MAX_NAME_STRLEN,"%s", WINDOW_DEFNAME);
    }

    if (gtk_initialized != GTKINIT_DONE) {
        gtk_init(0, NULL);
        gtk_initialized = GTKINIT_DONE;
    }

    vwin->management = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_opacity(vwin->management, 1.0f);
    gtk_widget_set_app_paintable(vwin->management, TRUE);

    gtk_window_set_title(GTK_WINDOW(vwin->management), vwin->name);
    gtk_container_set_border_width(GTK_CONTAINER(vwin->management), 0);
    gtk_window_set_decorated(GTK_WINDOW(vwin->management), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(vwin->management), TRUE);
    gtk_window_set_modal(GTK_WINDOW(vwin->management), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(vwin->management), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(vwin->management),
                WINDOW_DEFWIDTH, WINDOW_DEFHEIGHT);
    window_set_icon(GTK_WINDOW(vwin->management));

    vwin->drawingerea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(vwin->management),
                vwin->drawingerea);

    g_signal_connect(G_OBJECT(vwin->management), "delete-event",
                    G_CALLBACK(delete_window), NULL);
    g_signal_connect(G_OBJECT(vwin->drawingerea), "draw",
                G_CALLBACK(do_drawing), vwin);
    g_signal_connect(G_OBJECT(vwin->management), "key-press-event",
                    G_CALLBACK(keyboard_press), NULL);
#if 0
    g_signal_connect(G_OBJECT(vwin->management), "window_state_event",
                    G_CALLBACK(state_proc), vwin);
#endif

    vwin->flags = flags;

    __vwindow_attach(vwin);

    gtk_window_set_gravity(GTK_WINDOW(vwin->management),
                    GDK_GRAVITY_NORTH_WEST);
}

void window::resize_window(const char *name, int width, int height)
{
    struct vwindow *window;
    window = window_byname(name, NULL);
    if (!window)
        return;

    if (window->flags & WINDOW_AUTOSIZE)
        return;

    gtk_window_resize(GTK_WINDOW(window->management),
            width, height);
}

void window::move_window(const char *name, int x, int y)
{
    struct vwindow *window;

    window = window_byname(name, NULL);
    if (!window) return;

    gtk_window_move(GTK_WINDOW(window->management), x, y);
}

void window::destroy_window(const char *name)
{
    int id;
    struct vwindow *window;

    window = window_byname(name, &id);
    if (!window) {
        return;
    }
    pr_debug("Destory window \"%s\", id : %d\n",
                window->name, id);
    __vwindow_deattach(window);
    gtk_widget_destroy(window->management);

    if (window->pixbuf)
        g_object_unref(window->pixbuf);
    free(window);
}

void window::imageshow(const char *name, fcvImage *fcvimg)
{
    struct vwindow *vwin;
    int w, h;

    vwin = window_byname(name, NULL);
    if (!vwin) {
        return;
    }

    /* release previous fcvImage pixbuf attached to this window */
    if (vwin->pixbuf) {
        g_object_unref(vwin->pixbuf);
        vwin->pixbuf = NULL;
    }

    /*
     * AUTOSIZE flag indidicates that window size is
     * determianed by image automatically
     */
    if (vwin->flags & WINDOW_AUTOSIZE) {
        w = fcvimage::vimage_get_width(fcvimg);
        h = fcvimage::vimage_get_height(fcvimg);

        gtk_window_set_default_size(GTK_WINDOW(vwin->management), w, h);
        gtk_window_set_resizable(GTK_WINDOW(vwin->management), FALSE);
    }

    /*
     * attach new fcvImage pixbuf to this window
     * it maybe be released during drawing this window
     * if a new sclaed pixbuf is created by drawing routine
     */
    vwin->pixbuf = fcvimg;

    /* trigger draw on the top window drawing erea */
    gtk_widget_queue_draw(vwin->drawingerea);
    gtk_widget_show_all(vwin->management);
}

void window::imageshow_ops(const char *name, fcvImage *fcvimg,
            windowOperationFn opfunction, gpointer opfunc_data)
{
    int w, h;
    struct vwindow *vwin;

    vwin = window_byname(name, NULL);
    if (!vwin) {
        return;
    }

    /* release previous fcvImage pixbuf attached to this window */
    if (vwin->pixbuf) {
        g_object_unref(vwin->pixbuf);
        vwin->pixbuf = NULL;
    }

    if (vwin->flags & WINDOW_AUTOSIZE) {
        w = fcvimage::vimage_get_width(fcvimg);
        h = fcvimage::vimage_get_height(fcvimg);
        gtk_window_set_default_size(GTK_WINDOW(vwin->management), w, h);
        gtk_window_set_resizable(GTK_WINDOW(vwin->management), FALSE);
    }

    /*
     * attach new fcvImage pixbuf to this window
     * it maybe be released during drawing this window
     * if a new sclaed pixbuf is created by drawing routine
     */
    vwin->pixbuf = fcvimg;
    vwin->operationfunc = opfunction;
    vwin->opfunc_data = &opfunc_data;

    /* trigger draw on the top window drawing erea */
    gtk_widget_queue_draw(vwin->drawingerea);
    gtk_widget_show_all(vwin->management);
}

}
