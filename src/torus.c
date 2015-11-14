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
static void choose_torus(char names[MAXODE][12], int *sel, int n,
                         TorusBoxCommitFunc commit_func, void *cookie);

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

void do_torus_com(int c) {
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

  choose_torus(uvar_names, itor, NEQ, commit_torus, NULL);
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

  XRaiseWindow(display, torbox->base);
}

static void destroy_tor_box(TorusBox *torbox) {
  XDestroySubwindows(display, torbox->base);
  XDestroyWindow(display, torbox->base);
}

/**
 * Processes an event in torus mode.
 *
 * @return zero to continue, 1 to close the box.
 */
static int do_torus_event(TorusBox *torbox, const XEvent *ev) {
  Window wt;

  switch (ev->type) {
  case Expose:
    draw_torus_box(torbox, ev->xany.window);
    break;

  case ButtonRelease:
    if (ev->xbutton.window == torbox->done) {
      if (!torbox->commit_func(torbox->cookie, torbox->sel, torbox->n)) {
        destroy_tor_box(torbox);
        return 1;
      }
    } else if (ev->xbutton.window == torbox->cancel) {
      destroy_tor_box(torbox);
      return 1;
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

  return 0;
}

/**
 * Handle events until the torus box is closed.
 */
static void run_tor_box(TorusBox *torbox) {
  for (;;) {
    XEvent ev;

    XNextEvent(display, &ev);
    if (ev.type == Expose)
      do_expose(ev);
    if (do_torus_event(torbox, &ev))
      return;
  }
}

static void choose_torus(char names[MAXODE][12], int *sel, int n,
                         TorusBoxCommitFunc commit_func, void *cookie) {
  TorusBox torbox;

  for (int i = 0; i < n; ++i) {
    torbox.names[i] = names[i];
    torbox.sel[i] = sel[i];
  }
  torbox.n = n;
  torbox.commit_func = commit_func;
  torbox.cookie = cookie;

  make_tor_box(&torbox, "Fold which");
  run_tor_box(&torbox);
}
