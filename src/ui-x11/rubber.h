#ifndef XPPAUT_UI_X11_RUBBER_H
#define XPPAUT_UI_X11_RUBBER_H

#include <X11/Xlib.h>

/* --- Types --- */
typedef enum {
  X11_RUBBER_BOX,
  X11_RUBBER_LINE
} X11RubberType;

/**
 * A function called when a rubber ends.
 *
 * @param cookie some user-defined data.
 * @param commit non-zero if the data should be committed. Zero if the user
 *               aborted.
 * @param start a 2-element array of the start point, in window coordinates.
 * @param end a 2-element array of the end point, in window coordinates.
 */
typedef void (*X11RubberEndFunc)(void *cookie, int commit, const int *start/*[2]*/, const int *end/*[2]*/);

/* --- Functions --- */
/**
 * Handles input of a 2D range by capturing mouse button press, move and
 * release.
 *
 * @param w the window to capture and draw in.
 * @param t the type of graphics.
 * @param end_func the function to call when the rubber ends.
 * @param cookie data to pass to the callback.
 */
void x11_rubber(Window w, X11RubberType t, X11RubberEndFunc end_func, void *cookie);

#endif /* XPPAUT_UI_X11_RUBBER_H */
