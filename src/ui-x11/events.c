#include "events.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base/timeutil.h"
#include "base/vector.h"

/* --- Types --- */
typedef struct {
  Window window;
  long event_mask;
  X11EventsFunc func;
  void *cookie;
} X11EventsCb;
typedef struct {
  struct timeval ival;
  struct timeval next;
  unsigned int flags;
  X11EventsTimeoutFunc func;
  void *cookie;
} X11EventsTimeoutCb;

VECTOR_DECLARE(x11_events_cbs, X11EventsCbs, X11EventsCb)
VECTOR_DEFINE(x11_events_cbs, X11EventsCbs, X11EventsCb)
VECTOR_DECLARE(x11_events_timeout_cbs, X11EventsTimeoutCbs, X11EventsTimeoutCb)
VECTOR_DEFINE(x11_events_timeout_cbs, X11EventsTimeoutCbs, X11EventsTimeoutCb)

struct X11Events {
  Display *display;
  X11EventsCbs cbs;
  X11EventsCbs cbs_cache;
  X11EventsTimeoutCbs timeout_cbs;
  X11EventsTimeoutCbs timeout_cbs_cache;
};

X11Events *x11_events_alloc(Display *d) {
  X11Events *ret = malloc(sizeof(*ret));

  if (!ret)
    return NULL;

  ret->display = d;
  x11_events_cbs_init(&ret->cbs, 0);
  x11_events_cbs_init(&ret->cbs_cache, 0);
  x11_events_timeout_cbs_init(&ret->timeout_cbs, 0);
  x11_events_timeout_cbs_init(&ret->timeout_cbs_cache, 0);

  return ret;
}

void x11_events_free(X11Events *evs) {
  x11_events_timeout_cbs_clean(&evs->timeout_cbs_cache);
  x11_events_timeout_cbs_clean(&evs->timeout_cbs);
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

/**
 * Dispatch timeouts that have already happened
 *
 * @params now the current time, in normalized absolute time.
 * @return the number of timeout callbacks called.
 */
static int x11_events_dispatch_timeouts(X11Events *evs,
                                        const struct timeval *now) {
  int ret = 0;

  // Make a copy of cbs to allow callbacks to modify cbs.
  x11_events_timeout_cbs_remove(&evs->timeout_cbs_cache, 0,
                                evs->timeout_cbs_cache.len);

  X11EventsTimeoutCb *cbs_cache = x11_events_timeout_cbs_insert(
      &evs->timeout_cbs_cache, 0, evs->timeout_cbs.len);

  memcpy(cbs_cache, evs->timeout_cbs.elems,
         sizeof(*cbs_cache) * evs->timeout_cbs.len);

  // TODO: Replace with hash map if needed for performance.
  for (size_t i = 0; i < evs->timeout_cbs_cache.len; ++i) {
    const X11EventsTimeoutCb *cb = &evs->timeout_cbs_cache.elems[i];

    // TODO: Check that cb still exists in timeout_cbs.

    // if (cb->next >= now)
    if (!timeval_less(now, &cb->next)) {
      ++ret;
      cb->func(cb->cookie);
    }
  }

  return ret;
}

/**
 * Dispatch timeouts that have already happened, and removes some from the
 * context.
 *
 * @params now the current time, in normalized absolute time.
 * @return the number of timeout callbacks called.
 */
static int x11_events_expire_timeouts(X11Events *evs,
                                      const struct timeval *now) {
  int ret = x11_events_dispatch_timeouts(evs, now);

  if (!ret)
    return 0;

  size_t n = evs->timeout_cbs.len;

  for (size_t i = 0; i < n; ++i) {
    X11EventsTimeoutCb *cb = &evs->timeout_cbs.elems[n - i - 1];

    if (cb->flags & X11_EVENTS_T_REPEAT) {
      // while (cb->next <= now)
      while (!timeval_less(now, &cb->next))
        timeval_add(&cb->next, &cb->ival);
      continue;
    }

    // if (cb->next >= now)
    if (!timeval_less(now, &cb->next))
      x11_events_timeout_cbs_remove(&evs->timeout_cbs, n - i - 1, 1);
  }

  return ret;
}

/**
 * Returns the next point in time a callback should be called.
 *
 * @params buf the output buffer. Output only valid if zero is returned.
 * @return zero on success. Non-zero if there are no timeout callbacks.
 */
static int x11_events_next_timeout(X11Events *evs, struct timeval *buf) {
  for (size_t i = 0; i < evs->timeout_cbs.len; ++i) {
    const X11EventsTimeoutCb *cb = &evs->timeout_cbs.elems[i];

    if (i == 0 || timeval_less(&cb->next, buf))
      *buf = cb->next;
  }

  return evs->timeout_cbs.len == 0;
}

static int x11_events_wait(X11Events *evs, struct timeval *tv) {
  struct timespec now;

  if (clock_gettime(CLOCK_MONOTONIC, &now)) {
    perror("clock_gettime");
    return 1;
  }

  now.tv_sec = -now.tv_sec;
  now.tv_nsec = -now.tv_nsec;
  timeval_add_ts(tv, &now);

  int expired = 1;

  if (tv->tv_sec >= 0) {
    int fd = ConnectionNumber(evs->display);
    fd_set readset;

    FD_ZERO(&readset);
    FD_SET(fd, &readset);
    switch (select(fd + 1, &readset, NULL, NULL, tv)) {
    case 0:
      break;

    case 1:
      expired = 0;
      break;

    default:
      perror("select");
      return 1;
    }
  }

  if (expired) {
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
      perror("clock_gettime");
      return 1;
    }

    struct timeval now_tv = {0, 0};

    timeval_add_ts(&now_tv, &now);
    x11_events_expire_timeouts(evs, &now_tv);
  }

  return 0;
}

int x11_events_run(X11Events *evs) {
  while (evs->cbs.len || evs->timeout_cbs.len) {
    struct timeval tv;
    XEvent ev;

    // Ignore timeouts if there are events. We prioritize interactivity.
    if (!x11_events_next_timeout(evs, &tv) && XPending(evs->display) == 0) {
      if (x11_events_wait(evs, &tv))
        return 1;

      continue;
    }

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

int x11_events_timeout(X11Events *evs, const struct timeval *rtv,
                       unsigned int flags, X11EventsTimeoutFunc func,
                       void *cookie) {
  struct timespec ts;

  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    return 1;

  for (size_t i = 0; i < evs->timeout_cbs.len; ++i) {
    const X11EventsTimeoutCb *cb = &evs->timeout_cbs.elems[i];

    if (cb->func == func && cb->cookie == cookie)
      return 1;
  }

  X11EventsTimeoutCb *cb = x11_events_timeout_cbs_append(&evs->timeout_cbs);
  if (!cb)
    return 1;

  cb->ival = *rtv;
  cb->next = *rtv;
  timeval_add_ts(&cb->next, &ts);
  cb->flags = flags;
  cb->func = func;
  cb->cookie = cookie;

  return 0;
}

int x11_events_timeout_cancel(X11Events *evs, X11EventsTimeoutFunc func,
                              void *cookie) {
  for (size_t i = 0; i < evs->timeout_cbs.len; ++i) {
    const X11EventsTimeoutCb *cb = &evs->timeout_cbs.elems[i];

    if (cb->func != func || cb->cookie != cookie)
      continue;

    x11_events_timeout_cbs_remove(&evs->timeout_cbs, i, 1);
    return 0;
  }

  fprintf(stderr, "x11_events_timeout_cancel: no match for cookie=%p\n",
          cookie);
  return 1;
}
