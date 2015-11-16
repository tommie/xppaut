#include "edit-box.h"

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "ggets.h"
#include "main.h"
#include "pop_list.h"

/* --- Macros --- */
#define BUT_MASK                                                               \
  (ButtonPressMask | ButtonReleaseMask | KeyPressMask | ExposureMask |         \
   StructureNotifyMask | EnterWindowMask | LeaveWindowMask)

/* --- Types --- */
typedef struct {
  EditBoxCommitFunc commit_func;
  void *cookie;

  Window base, ok, cancel, reset;
  Window win[MAX_N_EBOX];
  char name[MAX_N_EBOX][MAX_LEN_EBOX], value[MAX_N_EBOX][MAX_LEN_EBOX],
      rval[MAX_N_EBOX][MAX_LEN_EBOX];
  int n, hot;
  int pos, col;
} EDIT_BOX;

/* --- Forward Declarations --- */
static void edit_box_destroy(EDIT_BOX *sb);

static void reset_ebox(EDIT_BOX *sb) {
  int n = sb->n;
  int i, l;
  Window w;
  for (i = 0; i < n; i++) {
    strcpy(sb->value[i], sb->rval[i]);
    w = sb->win[i];
    l = strlen(sb->name[i]);
    XClearWindow(display, w);
    XDrawString(display, w, gc, 0, CURY_OFF, sb->name[i], l);
    XDrawString(display, w, gc, l * DCURX, CURY_OFF, sb->value[i],
                strlen(sb->value[i]));
  }
  XFlush(display);
  sb->hot = 0;
  sb->pos = strlen(sb->value[0]);
  sb->col = (sb->pos + strlen(sb->name[0])) * DCURX;
  put_cursor_at(sb->win[0], DCURX * strlen(sb->name[0]), sb->pos);
}

static void expose_ebox(EDIT_BOX *sb, Window w) {
  int i, flag;

  if (w == sb->ok) {
    XDrawString(display, w, gc, 0, CURY_OFF, "Ok", 2);
    return;
  }
  if (w == sb->cancel) {
    XDrawString(display, w, gc, 0, CURY_OFF, "Cancel", 6);
    return;
  }
  if (w == sb->reset) {
    XDrawString(display, w, gc, 0, CURY_OFF, "Reset", 5);
    return;
  }
  for (i = 0; i < sb->n; i++) {
    if (w != sb->win[i])
      continue;
    flag = 0;
    if (i == sb->hot)
      flag = 1;
    do_hilite_text(sb->name[i], sb->value[i], flag, w, sb->pos, sb->col);
  }
}

static void ereset_hot(int inew, EDIT_BOX *sb) {
  int i = sb->hot;
  sb->hot = inew;
  XClearWindow(display, sb->win[inew]);
  do_hilite_text(sb->name[inew], sb->value[inew], 1, sb->win[inew],
                 strlen(sb->value[inew]), 0);
  XClearWindow(display, sb->win[i]);
  do_hilite_text(sb->name[i], sb->value[i], 0, sb->win[i], strlen(sb->value[i]),
                 0);
}

static void enew_editable(EDIT_BOX *sb, int inew, int *done, Window *w) {
  ereset_hot(inew, sb);
  sb->pos = strlen(sb->value[inew]);
  sb->col = (sb->pos + strlen(sb->name[inew])) * DCURX;
  *done = 0;
  *w = sb->win[inew];
}

static void edit_box_event(void *cookie, const XEvent *ev) {
  EDIT_BOX *sb = cookie;
  int inew;
  int nn = sb->n;
  int done = 0, i;
  char ch;
  int ihot = sb->hot;
  Window wt;
  Window w = sb->win[ihot]; /* active window */
  char *s = sb->value[ihot];

  switch (ev->type) {
  case ConfigureNotify:
  case Expose:
  case MapNotify:
    expose_ebox(sb, ev->xany.window);
    break;

  case ButtonRelease:
    if (ev->xbutton.window == sb->ok) {
      if (!sb->commit_func(sb->cookie, (char *)sb->value, sb->n)) {
        edit_box_destroy(sb);
        return;
      }
    } else if (ev->xbutton.window == sb->cancel) {
      edit_box_destroy(sb);
      return;
    } else if (ev->xbutton.window == sb->reset) {
      reset_ebox(sb);
    }
    break;

  case ButtonPress:
    for (i = 0; i < nn; i++) {
      if (ev->xbutton.window == sb->win[i]) {
        XSetInputFocus(display, sb->win[i], RevertToParent, CurrentTime);
        if (i != sb->hot)
          enew_editable(sb, i, &done, &w);
        break;
      }
    }
    break;

  case EnterNotify:
    wt = ev->xcrossing.window;
    if (wt == sb->ok || wt == sb->cancel || wt == sb->reset)
      XSetWindowBorderWidth(display, wt, 2);
    break;

  case LeaveNotify:
    wt = ev->xcrossing.window;
    if (wt == sb->ok || wt == sb->cancel || wt == sb->reset)
      XSetWindowBorderWidth(display, wt, 1);
    break;

  case KeyPress:
    ch = get_key_press(ev);
    edit_window(w, &sb->pos, s, &sb->col, &done, ch);
    if (done != 0) {
      if (done == 2) {
        edit_box_destroy(sb);
        return;
      }
      inew = (sb->hot + 1) % nn;
      enew_editable(sb, inew, &done, &w);
    }
    break;
  }
}

static void make_ebox_windows(EDIT_BOX *sb, const char *title) {
  int width, height;
  int i;
  int xpos, ypos, n = sb->n;
  int xstart, ystart;

  XTextProperty winname;
  XSizeHints size_hints;
  Window base;
  width = (MAX_LEN_EBOX + 4) * DCURX;
  height = (n + 4) * (DCURY + 16);
  base = make_plain_window(DefaultRootWindow(display), 0, 0, width, height, 4);
  size_hints.flags = PPosition | PSize | PMinSize | PMaxSize;
  size_hints.x = 0;
  size_hints.y = 0;
  size_hints.width = width;
  size_hints.height = height;
  size_hints.min_width = width;
  size_hints.min_height = height;
  size_hints.max_width = width;
  size_hints.max_height = height;
  XStringListToTextProperty((char **)&title, 1, &winname);
  XSetWMProperties(display, base, &winname, NULL, NULL, 0, &size_hints, NULL,
                   NULL);
  XFree(winname.value);
  sb->base = base;

  ystart = DCURY;
  xstart = DCURX;
  for (i = 0; i < n; i++) {
    xpos = xstart;
    ypos = ystart + i * (DCURY + 10);
    sb->win[i] = make_window(base, xpos, ypos, MAX_LEN_EBOX * DCURX, DCURY, 1);
  }

  ypos = height - 2 * DCURY;
  xpos = (width - 19 * DCURX) / 2;
  sb->ok = make_window(base, xpos, ypos, 2 * DCURX, DCURY, 1);
  sb->cancel = make_window(base, xpos + 4 * DCURX, ypos, 6 * DCURX, DCURY, 1);
  sb->reset = make_window(base, xpos + 12 * DCURX, ypos, 5 * DCURX, DCURY, 1);

  XSelectInput(display, sb->cancel, BUT_MASK);
  XSelectInput(display, sb->ok, BUT_MASK);
  XSelectInput(display, sb->reset, BUT_MASK);
  x11_events_listen(g_x11_events, X11_EVENTS_ANY_WINDOW, BUT_MASK,
                    edit_box_event, sb);

  XRaiseWindow(display, base);
}

static void edit_box_destroy(EDIT_BOX *sb) {
  x11_events_unlisten(g_x11_events, X11_EVENTS_ANY_WINDOW, BUT_MASK,
                      edit_box_event, sb);
  XDestroyWindow(display, sb->base);
  sb->base = 0;
}

int x11_edit_box_open(int n, const char *title, char *const *names,
                      char *const *values, EditBoxCommitFunc commit,
                      void *cookie) {
  EDIT_BOX *sb = malloc(sizeof(*sb));

  if (!sb)
    return 1;

  sb->commit_func = commit;
  sb->cookie = cookie;

  for (int i = 0; i < n; i++) {
    sprintf(sb->name[i], "%s=", names[i]);
    strcpy(sb->value[i], values[i]);
    strcpy(sb->rval[i], values[i]);
  }
  sb->n = n;
  sb->hot = 0;
  sb->pos = strlen(sb->value[0]);
  sb->col = (sb->pos + strlen(sb->name[0])) * DCURX;

  make_ebox_windows(sb, title);

  return 0;
}
