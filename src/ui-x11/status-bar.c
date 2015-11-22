#include "status-bar.h"

#include <stdlib.h>
#include <string.h>

#include "main.h"

/* --- Macros --- */
#define EV_MASK ExposureMask

/* --- Types --- */
struct X11StatusBar {
  Window w;
  char *text;
  void (*draw)(X11StatusBar *);
};

static void x11_status_bar_draw_normal(X11StatusBar *sb) {
  XClearWindow(display, sb->w);
  XSetForeground(display, gc, MyForeColor);
  XSetBackground(display, gc, MyBackColor);
  XDrawString(display, sb->w, gc, 5, CURY_OFF, sb->text, strlen(sb->text));
}

static void x11_status_bar_draw_small(X11StatusBar *sb) {
  XClearWindow(display, sb->w);
  XSetForeground(display, small_gc, MyForeColor);
  XSetBackground(display, small_gc, MyBackColor);
  XDrawString(display, sb->w, small_gc, 0, CURY_OFFs, sb->text,
              strlen(sb->text));
}

static void x11_status_bar_event(void *cookie, const XEvent *ev) {
  X11StatusBar *sb = cookie;

  switch (ev->type) {
  case Expose:
    sb->draw(sb);
    break;
  }
}

X11StatusBar *x11_status_bar_alloc(Window parent, int x, int y, unsigned int w,
                                   unsigned int h) {
  X11StatusBar *ret = malloc(sizeof(*ret));
  if (!ret)
    return NULL;

  ret->w = XCreateSimpleWindow(display, parent, x, y, w, h, 2, MyForeColor,
                               MyBackColor);
  ret->text = NULL;
  ret->draw =
      (h < DCURY ? x11_status_bar_draw_small : x11_status_bar_draw_normal);
  x11_events_listen(g_x11_events, ret->w, EV_MASK, x11_status_bar_event, ret);
  XMapWindow(display, ret->w);

  return ret;
}

void x11_status_bar_free(X11StatusBar *sb) {
  x11_events_unlisten(g_x11_events, sb->w, EV_MASK, x11_status_bar_event, sb);
  XDestroyWindow(display, sb->w);
  free(sb->text);
  free(sb);
}

void x11_status_bar_set_extents(X11StatusBar *sb, int x, int y, unsigned int w,
                                unsigned int h) {
  XMoveResizeWindow(display, sb->w, x, y, w, h);
}

void x11_status_bar_set_text(X11StatusBar *sb, const char *text) {
  sb->text = realloc(sb->text, strlen(text) + 1);
  strcpy(sb->text, text);
  sb->draw(sb);
}
