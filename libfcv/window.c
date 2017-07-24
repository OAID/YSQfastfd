/*
  Copyright (C) 2017 Open Intelligent Machines Co.,Ltd
 
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  AAID gtk window management related functions
  Authour : Haibing Xu <hxu@openailab.com>
*/
#include <sysdef.h>
#include <window.h>

static int gtk_initialized = 0;
static volatile int keyval = 0;

static struct window_desc _windows = {
	.nr_max  = MAX_OPENED_WINDOW,
	.nr_free = MAX_OPENED_WINDOW,
};

#define vwindow_creatable() \
	((_windows.nr_free > 0) ? TRUE : FALSE)

static gboolean do_wait(gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

static void keyboard_press(GtkWidget *widget,
			GdkEventKey *event, gpointer data)
{
	keyval = (char)event->keyval;
}

/* wait keyboard input events */
int waitkey(int msec)
{
	msec = (msec > WAITKEY_TIMEOUT) ? WAITKEY_TIMEOUT : msec;
	g_timeout_add(msec, (GSourceFunc)do_wait, NULL);
	gtk_main();
	return (int)keyval;
}

static gboolean delete_window(GtkWidget *widget,
				GdkEvent *event, gpointer data)
{
	keyval = WINDOW_QUITKEY;
	return TRUE;
}

#if 0
static gboolean state_proc(GtkWidget *widget,
				GdkEventWindowState *event, gpointer data)
{
	printf("GdkWindowState : 0x%X\n", event->new_window_state);
	printf("Changed_mask = 0x%X\n", event->changed_mask);

	if (event->new_window_state 
					== GDK_WINDOW_STATE_ICONIFIED) {
		gtk_window_iconify(GTK_WINDOW(widget));
	}

	return FALSE;
}
#endif

static inline struct vwindow *window_byname(const char *name, int *wid)
{
	int id;
	for (id = 0; id < _windows.nr_max; id++) {
		if (_windows.list[id]) {
			if (strcmp(_windows.list[id]->name, name) == 0) {
				if (wid) *wid = id;
				return _windows.list[id];
			}
		}
	}
	return NULL;
}

static void __vwindow_attach(struct vwindow *window)
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

static void __vwindow_deattach(struct vwindow *window)
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

void rectangle_onwindow(cairo_t *cr, gpointer data)
{
	fcv_rectangle_t *rect;

	rect = (fcv_rectangle_t *)data;

	cairo_set_source_rgb(cr, 0, 0, 255);
	cairo_rectangle(cr, (double)rect->x, (double)rect->y,
					rect->width, rect->height);
	cairo_stroke(cr);
}

void text_osd_onwindow(cairo_t *cr, gpointer data)
{
	cairo_set_source_rgb(cr, 0.627, 0, 0);
	cairo_set_font_size(cr, 10.0);
	cairo_move_to(cr, 50.0, 130.0);
	cairo_show_text(cr, "OPEN AI LAB");
}

static gboolean do_drawing(GtkWidget *widget, cairo_t *cr, gpointer data)
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

static void vwindow_internal_init(struct vwindow *vwin)
{
	vwin->management = NULL;
	vwin->drawingerea = NULL;
	vwin->pixbuf = NULL;
	vwin->operationfunc = NULL;
	vwin->opfunc_data = NULL;
}

#if 0
#define WINDOW_ICON    "./icon/openailab.png"
static void window_set_icon(GtkWindow *window)
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
#define window_set_icon(...)
#endif

void named_window(const char *name, int flags)
{
	struct vwindow *vwin;

	if (!vwindow_creatable()) {
		assert_failure();
		return;
	}

	vwin = calloc(1, sizeof(struct vwindow));
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

void resize_window(const char *name, int width, int height)
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

void move_window(const char *name, int x, int y)
{
	struct vwindow *window;

	window = window_byname(name, NULL);
	if (!window) return;

	gtk_window_move(GTK_WINDOW(window->management), x, y);
#if 0
	gint x_coord, y_coord;
	gtk_widget_show(window->management);
	gtk_window_get_position(GTK_WINDOW(window->management),
					&x_coord, &y_coord);
	g_print("%s position : %d - %d\n", name, x_coord, y_coord);
#endif
}

void destroy_window(const char *name)
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

void imageshow(const char *name, fcvImage *fcvimg)
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
		w = vimage_get_width(fcvimg);
		h = vimage_get_height(fcvimg);
#if 0
		gtk_widget_set_size_request(vwin->management, w, h);
#else
		gtk_window_set_default_size(GTK_WINDOW(vwin->management), w, h);
		gtk_window_set_resizable(GTK_WINDOW(vwin->management), FALSE);
#endif
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

void imageshow_ops(const char *name, fcvImage *fcvimg,
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
		w = vimage_get_width(fcvimg);
		h = vimage_get_height(fcvimg);
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
	vwin->opfunc_data = opfunc_data;

	/* trigger draw on the top window drawing erea */
	gtk_widget_queue_draw(vwin->drawingerea);
	gtk_widget_show_all(vwin->management);
}
