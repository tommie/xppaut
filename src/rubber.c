#include "rubber.h"

#include <string.h>

#include "ggets.h"
#include "many_pops.h"
#include "load_eqn.h"
#include "main.h"

/* --- Types --- */
typedef struct {
  int f; /* RUBBOX, RUBLINE */
  Window w;
  RubberEndFunc end_func;
  void *cookie;

  int start[2];
  int end[2];
  int state;
} RubberContext;

/* --- Forward Declarations --- */
static void rbox(int i1, int j1, int i2, int j2, Window w, int f);
static void rubber_stop(RubberContext *ctx);

static int rubber_event(RubberContext *ctx, const XEvent *ev) {
  switch (ev->type) {
  case Expose:
    XSetFunction(display, gc, GXxor);
    if (xorfix) {
      XSetForeground(display, gc, MyDrawWinColor);
      XSetBackground(display, gc, MyForeColor);
    }
    break;

  case KeyPress:
    if (ctx->state > 0)
      break;

    ctx->end_func(ctx->cookie, 0, ctx->start, ctx->end);
    return 1;

  case ButtonPress:
    if (ctx->state > 0)
      break;

    ctx->state = 1;
    ctx->end[0] = ctx->start[0] = ev->xkey.x;
    ctx->end[1] = ctx->start[1] = ev->xkey.y;
    rbox(ctx->start[0], ctx->start[1], ctx->end[0], ctx->end[1], ctx->w, ctx->f);
    break;

  case MotionNotify:
    if (ctx->state == 0)
      break;

    rbox(ctx->start[0], ctx->start[1], ctx->end[0], ctx->end[1], ctx->w, ctx->f);
    ctx->end[0] = ev->xmotion.x;
    ctx->end[1] = ev->xmotion.y;
    rbox(ctx->start[0], ctx->start[1], ctx->end[0], ctx->end[1], ctx->w, ctx->f);
    break;
  case ButtonRelease:
    if (ctx->state == 0)
      break;

    rbox(ctx->start[0], ctx->start[1], ctx->end[0], ctx->end[1], ctx->w, ctx->f);
    ctx->end_func(ctx->cookie, 1, ctx->start, ctx->end);
    return 1;
  }

  return 0;
}

void rubber(Window w, int f, RubberEndFunc end_func, void *cookie) {
  RubberContext ctx;

  memset(&ctx, 0, sizeof(ctx));
  ctx.w = w;
  ctx.f = f;
  ctx.end_func = end_func;
  ctx.cookie = cookie;

  XFlush(display);
  XSetFunction(display, gc, GXxor);
  if (xorfix) {
    XSetForeground(display, gc, MyDrawWinColor);
    XSetBackground(display, gc, MyForeColor);
  }
  XSelectInput(display, w, KeyPressMask | ButtonPressMask | ButtonReleaseMask |
                               PointerMotionMask | ButtonMotionMask |
                               ExposureMask);

  for (;;) {
    XEvent ev;

    XNextEvent(display, &ev);
    if (ev.type == Expose)
      do_expose(ev);
    if (rubber_event(&ctx, &ev))
      break;
  }

  rubber_stop(&ctx);
}

static void rubber_stop(RubberContext *ctx) {
  XSetFunction(display, gc, GXcopy);
  if (xorfix) {
    XSetForeground(display, gc, MyForeColor);
    XSetBackground(display, gc, MyDrawWinColor);
  }
  XSelectInput(display, ctx->w, KeyPressMask | ButtonPressMask | ExposureMask |
                                    ButtonReleaseMask | ButtonMotionMask);
}

static void rbox(int i1, int j1, int i2, int j2, Window w, int f) {
  int x1 = i1, x2 = i2, y1 = j1, y2 = j2;
  if (f == RUBLINE) {
    XDrawLine(display, w, gc, i1, j1, i2, j2);
    return;
  }
  if (x1 > x2) {
    x2 = i1;
    x1 = i2;
  }
  if (y1 > y2) {
    y1 = j2;
    y2 = j1;
  }
  rectangle(x1, y1, x2, y2, w);
}
