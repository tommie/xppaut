#include "calc.h"

#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "form_ode.h"
#include "ggets.h"
#include "init_conds.h"
#include "load_eqn.h"
#include "main.h"
#include "parserslow.h"
#include "pop_list.h"
#include "base/timeutil.h"

/* --- Macros --- */
#define MYMASK                                                                 \
  (ButtonPressMask | ButtonReleaseMask | KeyPressMask | ExposureMask |         \
   StructureNotifyMask | LeaveWindowMask | EnterWindowMask)

/* --- Types --- */
typedef struct {
  Window base, quit, answer;
  double last_val;
  int use;

  char value[80], name[10];
  int pos, col;
} Calc;

/* --- Forward Declarations --- */
static void quit_calc(Calc *calc);
static double calculate(char *expr, int *ok);
static int has_eq(char *z, char *w, int *where);

/* --- Data --- */
static Calc g_calc;

static void draw_calc(Window w) {
  char bob[100];
  if (w == g_calc.answer) {
    XClearWindow(display, w);
    sprintf(bob, "%.16g", g_calc.last_val);
    XDrawString(display, w, small_gc, 0, CURY_OFFs, bob, strlen(bob));
    return;
  }
  if (w == g_calc.quit) {
    XDrawString(display, w, small_gc, 0, CURY_OFFs, "Quit", 4);
    return;
  }
}

static void ini_calc_string(Calc *calc) {
  strcpy(calc->value, " ");
  strcpy(calc->name, "Formula:");
  calc->pos = strlen(calc->value);
  calc->col = (calc->pos + strlen(calc->name)) * DCURX;
  clr_command();
  display_command(calc->name, calc->value, 2, 0);
}

static void calc_event(void *cookie, const XEvent *ev) {
  Calc *calc = cookie;
  int done = 0;

  edit_command_string(*ev, calc->name, calc->value, &done, &calc->pos,
                      &calc->col);
  if (done == 1) {
    double z = 0;
    int flag = do_calc(calc->value, &z);

    if (flag != -1) {
      calc->last_val = z;
      draw_calc(calc->answer);
    }
    ini_calc_string(calc);
  } else if (done == -1) {
    quit_calc(calc);
    return;
  }

  switch (ev->type) {
  case Expose:
    draw_calc(ev->xexpose.window);
    break;

  case ButtonRelease:
    if (ev->xbutton.window == calc->quit)
      quit_calc(calc);
    break;

  case EnterNotify:
    if (ev->xcrossing.window == calc->quit)
      XSetWindowBorderWidth(display, ev->xcrossing.window, 2);
    break;

  case LeaveNotify:
    if (ev->xcrossing.window == calc->quit)
      XSetWindowBorderWidth(display, ev->xcrossing.window, 1);
    break;
  }
}

static void make_calc(Calc *calc) {
  int width, height;
  static char *name[] = {"Answer"};
  Window base;
  XTextProperty winname;
  XSizeHints size_hints;

  calc->last_val = 0;
  width = 20 + 24 * DCURXs;
  height = 4 * DCURYs;
  base =
    make_plain_window(RootWindow(display, screen), 0, 0, width, height, 4);
  calc->base = base;
  size_hints.flags = PPosition | PSize | PMinSize | PMaxSize;
  size_hints.x = 0;
  size_hints.y = 0;
  size_hints.width = width;
  size_hints.height = height;
  size_hints.min_width = width;
  size_hints.min_height = height;
  size_hints.max_width = width;
  size_hints.max_height = height;

  XStringListToTextProperty(name, 1, &winname);
  XSetWMProperties(display, base, &winname, &winname, NULL, 0, &size_hints,
                   NULL, NULL);
  XFree(winname.value);
  calc->answer = make_window(base, 10, DCURYs / 2, 24 * DCURXs, DCURYs, 0);
  width = (width - 4 * DCURXs) / 2;
  calc->quit =
    make_window(base, width, (int)(2.5 * DCURYs), 4 * DCURXs, DCURYs, 1);
  XSelectInput(display, calc->quit, MYMASK);
  x11_events_listen(g_x11_events, X11_EVENTS_ANY_WINDOW, MYMASK, calc_event, &g_calc);
  calc->use = 1;
}

static void quit_calc(Calc *calc) {
  x11_events_unlisten(g_x11_events, X11_EVENTS_ANY_WINDOW, MYMASK, calc_event, calc);
  XDestroyWindow(display, calc->base);
  clr_command();
  calc->use = 0;
}

void q_calc(void) {
  if (g_calc.use)
    return;

  make_calc(&g_calc);
  ini_calc_string(&g_calc);
}

int do_calc(char *temp, double *z) {
  char val[15];
  int ok;
  int i;
  double newz;
  if (strlen(temp) == 0) {
    *z = 0.0;
    return (1);
  }
  if (has_eq(temp, val, &i)) {

    newz = calculate(&temp[i], &ok); /*  calculate quantity  */

    if (ok == 0)
      return (-1);
    i = find_user_name(PARAMBOX, val);
    if (i > -1) {
      set_val(val, newz); /* a parameter set to value  */
      *z = newz;
      redraw_params();
    } else {
      i = find_user_name(ICBOX, val);
      if (i < 0) {
        err_msg("No such name!");
        return (-1);
      }
      set_val(val, newz);

      last_ic[i] = newz;
      *z = newz;
      redraw_ics();
    }
    return (0);
  }

  newz = calculate(temp, &ok);
  if (ok == 0)
    return (-1);
  *z = newz;
  return (1);
}

static int has_eq(char *z, char *w, int *where) {
  int i;
  for (i = 0; i < strlen(z); i++)
    if (z[i] == ':')
      break;
  if (i == strlen(z))
    return (0);
  strncpy(w, z, i);
  w[i] = 0;
  *where = i + 1;
  return (1);
}

static double calculate(char *expr, int *ok) {
  int com[400], i;
  if (parse_expr(expr, com, &i)) {
    err_msg("Illegal formula ..");
    *ok = 0;
    return 0.0;
  }
  /* fpr_command(com); */
  *ok = 1;
  return evaluate(com);
}
