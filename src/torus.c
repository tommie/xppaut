#include "torus.h"

#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "form_ode.h"
#include "ggets.h"
#include "load_eqn.h"
#include "main.h"
#include "many_pops.h"
#include "pop_list.h"
#include "base/timeutil.h"
#include "bitmap/info.bitmap"

/* --- Macros --- */
#define BUT_MASK                                                               \
  (ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask |      \
   LeaveWindowMask)

/* --- Types --- */
typedef int (*TorusBoxCommitFunc)(void *, const int *sel, int n);
typedef struct {
  /** Names of variables. */
  char *(names[12]);
  /** Bitmask of variables that has torus enabled. */
  int sel[MAXODE];
  /** Number of variables. */
  int n;

  TorusBoxCommitFunc commit_func;
  void *cookie;

  Window base, done, cancel;
  Window w[MAXODE];
} TorusBox;

/* --- Forward Declarations --- */
static void destroy_tor_box(TorusBox *torbox);

/* --- Data --- */
static TorusBox g_torbox;

static int commit_torus(void *cookie, const int *sel, int n) {
  for (int i = 0; i < n; i++)
    itor[i] = sel[i];

  TORUS = 0;
  for (int i = 0; i < NEQ; i++) {
    if (itor[i] == 1)
      TORUS = 1;
  }

  return 0;
}

static void draw_tor_var(TorusBox *torbox, int i) {
  char strng[15];
  XClearWindow(display, torbox->w[i]);
  if (torbox->sel[i] == 1)
    sprintf(strng, "X  %s", torbox->names[i]);
  else
    sprintf(strng, "   %s", torbox->names[i]);
  XDrawString(display, torbox->w[i], small_gc, 0, CURY_OFFs, strng,
              strlen(strng));
}

static void draw_torus_box(TorusBox *torbox, Window win) {
  int i;

  if (win == torbox->cancel) {
    XDrawString(display, win, small_gc, 5, CURY_OFFs, "Cancel", 6);
    return;
  }
  if (win == torbox->done) {
    XDrawString(display, win, small_gc, 5, CURY_OFFs, "Done", 4);
    return;
  }

  for (i = 0; i < torbox->n; i++) {
    if (win == torbox->w[i])
      draw_tor_var(torbox, i);
  }
}

/**
 * Processes an event in torus mode.
 */
static void tor_box_event(void *cookie, const XEvent *ev) {
  TorusBox *torbox = cookie;
  Window wt;

  switch (ev->type) {
  case Expose:
    draw_torus_box(torbox, ev->xany.window);
    break;

  case ButtonRelease:
    if (ev->xbutton.window == torbox->done) {
      if (!torbox->commit_func(torbox->cookie, torbox->sel, torbox->n)) {
        destroy_tor_box(torbox);
        return;
      }
    } else if (ev->xbutton.window == torbox->cancel) {
      destroy_tor_box(torbox);
      return;
    }

    for (int i = 0; i < torbox->n; i++) {
      if (ev->xbutton.window == torbox->w[i]) {
        torbox->sel[i] = 1 - torbox->sel[i];
        draw_tor_var(torbox, i);
        break;
      }
    }
    break;

  case EnterNotify:
    wt = ev->xcrossing.window;
    if (wt == torbox->done || wt == torbox->cancel)
      XSetWindowBorderWidth(display, wt, 2);
    break;

  case LeaveNotify:
    wt = ev->xcrossing.window;
    if (wt == torbox->done || wt == torbox->cancel)
      XSetWindowBorderWidth(display, wt, 1);
    break;
  }
}

static void make_tor_box(TorusBox *torbox, const char *title) {
  int ndn, nac, width, height;
  int nv;
  /*int nh; Not used anywhere*/
  int i, i1, j1, xpos, ypos;
  int xstart = DCURXs;
  int ystart = DCURYs;
  Window base;
  XTextProperty winname;
  XSizeHints size_hints;

  nv = 4 * DisplayHeight / (5 * (DCURYs + 8));
  /*nh=DisplayWidth/(18*DCURXs);*/

  if (torbox->n < nv)
    ndn = torbox->n;
  else
    ndn = nv;
  nac = torbox->n / ndn;
  if (nac * ndn < torbox->n)
    nac++;

  width = 24 * DCURXs * nac + 10;
  height = 3 * DCURYs + ndn * (DCURYs + 8);

  base = make_plain_window(RootWindow(display, screen), 0, 0, width, height, 4);

  torbox->base = base;
  XStringListToTextProperty((char **)&title, 1, &winname);
  size_hints.flags = PPosition | PSize | PMinSize | PMaxSize;
  size_hints.x = 0;
  size_hints.y = 0;
  size_hints.width = width;
  size_hints.height = height;
  size_hints.min_width = width;
  size_hints.min_height = height;
  size_hints.max_width = width;
  size_hints.max_height = height;

  XClassHint class_hints;
  class_hints.res_name = "";
  class_hints.res_class = "";

  make_icon((char *)info_bits, info_width, info_height, base);

  XSetWMProperties(display, base, &winname, NULL, NULL, 0, &size_hints, NULL,
                   &class_hints);
  for (i = 0; i < torbox->n; i++) {
    i1 = i / nv;
    j1 = i % nv;
    xpos = xstart + 18 * DCURXs * i1;
    ypos = ystart + j1 * (DCURYs + 8);
    torbox->w[i] = make_window(base, xpos, ypos, 15 * DCURXs, DCURYs, 1);
  }

  xpos = (width - 16 * DCURXs - 10) / 2;
  ypos = height - 3 * DCURYs / 2;

  torbox->cancel = make_window(base, xpos, ypos, 8 * DCURXs, DCURYs, 1);
  torbox->done =
      make_window(base, xpos + 8 * DCURXs + 10, ypos, 8 * DCURXs, DCURYs, 1);

  XSelectInput(display, torbox->cancel, BUT_MASK);
  XSelectInput(display, torbox->done, BUT_MASK);
  x11_events_listen(g_x11_events, X11_EVENTS_ANY_WINDOW, BUT_MASK, tor_box_event, torbox);

  XRaiseWindow(display, torbox->base);
}

static void destroy_tor_box(TorusBox *torbox) {
  x11_events_unlisten(g_x11_events, X11_EVENTS_ANY_WINDOW, BUT_MASK, tor_box_event, torbox);
  XDestroyWindow(display, torbox->base);
  torbox->base = 0;
}

void do_torus_com(int c) {
  /* Skip if dialog already open. */
  if (g_torbox.base)
    return;

  if (c == 1) {
    /* None */
    TORUS = 0;
    for (int i = 0; i < MAXODE; i++)
      itor[i] = 0;

    return;
  }

  new_float("Period :", &TOR_PERIOD);
  if (TOR_PERIOD <= 0.0) {
    err_msg("Choose positive period");
    return;
  }

  if (c == 0) {
    /* All */
    TORUS = 1;
    for (int i = 0; i < MAXODE; i++)
      itor[i] = 1;

    return;
  }

  /* Open dialog box. */
  for (int i = 0; i < NEQ; ++i) {
    g_torbox.names[i] = uvar_names[i];
    g_torbox.sel[i] = itor[i];
  }
  g_torbox.n = NEQ;
  g_torbox.commit_func = commit_torus;
  g_torbox.cookie = NULL;

  make_tor_box(&g_torbox, "Fold which");
}
