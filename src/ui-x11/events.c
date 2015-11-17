#include "events.h"

#include <stdlib.h>
#include "base/vector.h"

/* --- Types --- */
typedef struct {
  Window window;
  long event_mask;
  X11EventsFunc func;
  void *cookie;
} X11EventsCb;

VECTOR_DECLARE(x11_events_cbs, X11EventsCbs, X11EventsCb)
VECTOR_DEFINE(x11_events_cbs, X11EventsCbs, X11EventsCb)

struct X11Events {
  Display *display;
  X11EventsCbs cbs;
  X11EventsCbs cbs_cache;
};

X11Events *x11_events_alloc(Display *d) {
  X11Events *ret = malloc(sizeof(*ret));

  if (!ret)
    return NULL;

  ret->display = d;
  x11_events_cbs_init(&ret->cbs, 0);
  x11_events_cbs_init(&ret->cbs_cache, 0);

  return ret;
}

void x11_events_free(X11Events *evs) {
  x11_events_cbs_clean(&evs->cbs_cache);
  x11_events_cbs_clean(&evs->cbs);
  free(evs);
}

static long mask_for_type(int type) {
  switch (type) {
  case MotionNotify:
    return ButtonMotionMask | PointerMotionMask;
  case ButtonPress:
    return ButtonPressMask;
  case ButtonRelease:
    return ButtonReleaseMask;
  case ColormapNotify:
    return ColormapChangeMask;
  case EnterNotify:
    return EnterWindowMask;
  case LeaveNotify:
    return LeaveWindowMask;
  case Expose:
  case NoExpose:
    return ExposureMask;
  case FocusIn:
  case FocusOut:
    return FocusChangeMask;
  case KeymapNotify:
    return KeymapStateMask;
  case KeyPress:
    return KeyPressMask;
  case KeyRelease:
    return KeyReleaseMask;
  case PropertyNotify:
    return PropertyChangeMask;
  case ResizeRequest:
    return ResizeRedirectMask;
  case CirculateNotify:
  case ConfigureNotify:
  case MapNotify:
    return StructureNotifyMask | SubstructureNotifyMask |
           SubstructureRedirectMask;
  case DestroyNotify:
  case GravityNotify:
  case ReparentNotify:
  case UnmapNotify:
    return StructureNotifyMask | SubstructureNotifyMask;
  case VisibilityNotify:
    return VisibilityChangeMask;
  default:
    return -1;
  }
}

int x11_events_dispatch(X11Events *evs, const XEvent *ev) {
  int ret = 0;
  long mask = mask_for_type(ev->type);

  // Make a copy of cbs to allow callbacks to modify cbs.
  x11_events_cbs_remove(&evs->cbs_cache, 0, evs->cbs_cache.len);

  X11EventsCb *cbs_cache =
      x11_events_cbs_insert(&evs->cbs_cache, 0, evs->cbs.len);

  memcpy(cbs_cache, evs->cbs.elems, sizeof(*cbs_cache) * evs->cbs.len);

  // TODO: Replace with hash map if needed for performance.
  for (size_t i = 0; i < evs->cbs_cache.len; ++i) {
    const X11EventsCb *cb = &evs->cbs_cache.elems[i];

    // TODO: Check that cb still exists in cbs.

    if ((cb->window == X11_EVENTS_ANY_WINDOW ||
         ev->xany.window == cb->window) &&
        (mask & cb->event_mask)) {
      ++ret;
      cb->func(cb->cookie, ev);
    }
  }

  return ret;
}

int x11_events_run(X11Events *evs) {
  while (evs->cbs.len) {
    XEvent ev;

    if (XNextEvent(evs->display, &ev))
      return 1;

    x11_events_dispatch(evs, &ev);
  }

  return 0;
}

int x11_events_listen(X11Events *evs, Window w, long mask, X11EventsFunc func,
                      void *cookie) {
  long sel_mask = mask;

  // Check if we already have something registered.
  for (size_t i = 0; i < evs->cbs.len; ++i) {
    const X11EventsCb *cb = &evs->cbs.elems[i];

    if (w != X11_EVENTS_ANY_WINDOW && cb->window != w)
      continue;

    if (cb->window == w && cb->event_mask == mask && cb->func == func &&
        cb->cookie == cookie)
      return 1;

    if (cb->window == w)
      sel_mask |= cb->event_mask;
  }

  if (w != X11_EVENTS_ANY_WINDOW && XSelectInput(evs->display, w, sel_mask))
    return 1;

  X11EventsCb *cb = x11_events_cbs_append(&evs->cbs);
  if (!cb)
    return 1;

  cb->window = w;
  cb->event_mask = mask;
  cb->func = func;
  cb->cookie = cookie;

  return 0;
}

int x11_events_unlisten(X11Events *evs, Window w, long mask, X11EventsFunc func,
                        void *cookie) {
  for (size_t i = 0; i < evs->cbs.len; ++i) {
    const X11EventsCb *cb = &evs->cbs.elems[i];

    if (cb->event_mask != mask || cb->func != func || cb->cookie != cookie)
      continue;

    x11_events_cbs_remove(&evs->cbs, i, 1);
    // TODO: Do XSelectInput if needed for performance.
    return 0;
  }

  fprintf(stderr,
          "x11_events_unlisten: no match for w=0x%lX, mask=0x%lX, cookie=%p\n",
          w, mask, cookie);
  return 1;
}
