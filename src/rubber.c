#include "rubber.h"

#include <stdlib.h>
#include <string.h>

#include "ggets.h"
#include "many_pops.h"
#include "load_eqn.h"
#include "main.h"

/* --- Macros --- */
#define EV_MASK                                                                \
  KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask |     \
      ButtonMotionMask

/* --- Types --- */
typedef struct {
  int f; /* RUBBOX, RUBLINE */
  Window w;
  RubberEndFunc end_func;
  void *cookie;
  long event_mask;

  int start[2];
  int end[2];
  int state;
} RubberContext;

/* --- Forward Declarations --- */
static void rubber_draw(const RubberContext *ctx);
static void rubber_stop(RubberContext *ctx);

static void rubber_event(void *cookie, const XEvent *ev) {
  RubberContext *ctx = cookie;

  // TODO(tommie): Remove when using x11_events_listen(w).
  if (ev->xany.window != ctx->w)
    return;

  switch (ev->type) {
  case KeyPress:
    if (ctx->state > 0)
      break;

    ctx->end_func(ctx->cookie, 0, ctx->start, ctx->end);
    rubber_stop(ctx);
    break;

  case ButtonPress:
    if (ctx->state > 0)
      break;

    ctx->state = 1;
    ctx->end[0] = ctx->start[0] = ev->xkey.x;
    ctx->end[1] = ctx->start[1] = ev->xkey.y;
    rubber_draw(ctx);
    break;

  case MotionNotify:
    if (ctx->state == 0)
      break;

    rubber_draw(ctx);
    ctx->end[0] = ev->xmotion.x;
    ctx->end[1] = ev->xmotion.y;
    rubber_draw(ctx);
    break;

  case ButtonRelease:
    if (ctx->state == 0)
      break;

    rubber_draw(ctx);
    ctx->end_func(ctx->cookie, 1, ctx->start, ctx->end);
    rubber_stop(ctx);
    break;
  }
}

void rubber(Window w, int f, RubberEndFunc end_func, void *cookie) {
  // TODO(tommie): Replace XGetWindowAttributes and XSelectInput with
  // x11_events_listen(w) when all callers use it. If used now, it will
  // overwrite the event mask rather than augment it.
  XWindowAttributes wa;

  if (!XGetWindowAttributes(display, w, &wa)) {
    plintf("XGetWindowAttributes failed\n");
    end_func(cookie, 0, NULL, NULL);
    return;
  }

  RubberContext *ctx = malloc(sizeof(*ctx));
  if (!ctx) {
    perror("malloc");
    end_func(cookie, 0, NULL, NULL);
    return;
  }

  memset(ctx, 0, sizeof(*ctx));
  ctx->w = w;
  ctx->f = f;
  ctx->end_func = end_func;
  ctx->cookie = cookie;
  ctx->event_mask = wa.your_event_mask;

  XSelectInput(display, w, ctx->event_mask | EV_MASK);
  x11_events_listen(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK, rubber_event,
                    ctx);
}

static void rubber_stop(RubberContext *ctx) {
  x11_events_unlisten(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK,
                      rubber_event, ctx);

  XSelectInput(display, ctx->w, ctx->event_mask);
  free(ctx);
}

static void rubber_draw(const RubberContext *ctx) {
  int x1 = ctx->start[0], y1 = ctx->start[1];
  int x2 = ctx->end[0], y2 = ctx->end[1];

  XSetFunction(display, gc, GXxor);
  if (xorfix) {
    XSetForeground(display, gc, MyDrawWinColor);
    XSetBackground(display, gc, MyForeColor);
  }

  switch (ctx->f) {
  case RUBLINE:
    XDrawLine(display, ctx->w, gc, ctx->start[0], ctx->start[1], ctx->end[0], ctx->end[1]);
    break;

  case RUBBOX:
    if (x1 > x2) {
      x2 = ctx->start[0];
      x1 = ctx->end[0];
    }
    if (y1 > y2) {
      y2 = ctx->start[1];
      y1 = ctx->end[1];
    }
    rectangle(x1, y1, x2, y2, ctx->w);
    break;

  default:
    plintf("rubber_draw: unknown type: %d\n", ctx->f);
  }

  XSetFunction(display, gc, GXcopy);
  if (xorfix) {
    XSetForeground(display, gc, MyForeColor);
    XSetBackground(display, gc, MyDrawWinColor);
  }
}
