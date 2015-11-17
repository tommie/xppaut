#ifndef XPPAUT_UI_X11_EVENTS_H
#define XPPAUT_UI_X11_EVENTS_H

#include <sys/time.h>
#include <X11/Xlib.h>

/* --- Macros --- */
#define X11_EVENTS_ANY_WINDOW 0
#define X11_EVENTS_T_REPEAT 1

/* --- Types --- */
typedef struct X11Events X11Events;
typedef void (*X11EventsFunc)(void *cookie, const XEvent *ev);
typedef void (*X11EventsTimeoutFunc)(void *cookie);

/**
 * Allocate and initialize an events context.
 *
 * @param d the X11 display to operate on.
 * @return a newly allocated context, to be freed by x11_events_free.
 */
X11Events *x11_events_alloc(Display *d);

/**
 * Free an allocated events context.
 */
void x11_events_free(X11Events *evs);

/**
 * Dispatch a single event to callbacks.
 *
 * @param ev the X11 event to dispatch.
 * @return the number of callbacks invoked.
 */
int x11_events_dispatch(X11Events *evs, const XEvent *ev);

/**
 * Loop over events until there are no more listeners.
 *
 * @return zero on normal termination, non-zero if an error occurred.
 */
int x11_events_run(X11Events *evs);

/**
 * Register a listener.
 *
 * If this combination of arguments is already registered, this function fails.
 *
 * @param evs the events context to register on.
 * @param w the window to listen on. Can be X11_EVENTS_ANY_WINDOW.
 * @param mask the mask for events to listen on.
 * @param func the function to call.
 * @param cookie the data to pass as first argument to func.
 * @return zero on success.
 */
int x11_events_listen(X11Events *evs, Window w, long mask, X11EventsFunc func,
                      void *cookie);
int x11_events_unlisten(X11Events *evs, Window w, long mask, X11EventsFunc func,
                        void *cookie);

/**
 * Register a timeout.
 *
 * If this combination of function and cookie is already registered, this
 * function fails.
 *
 * @param evs the events context to register on.
 * @param rtv a time in the future, relative now, to call the callback.
 * @param flags a bitmask of X11_EVENTS_T_* masks.
 * @param func the function to call.
 * @param cookie the data to pass as first argument to func.
 * @return zero on success.
 */
int x11_events_timeout(X11Events *evs, const struct timeval *rtv,
                       unsigned int flags, X11EventsTimeoutFunc func,
                       void *cookie);
int x11_events_timeout_cancel(X11Events *evs, X11EventsTimeoutFunc func,
                              void *cookie);

#endif /* XPPAUT_UI_X11_EVENTS_H */
