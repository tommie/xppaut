#ifndef XPPAUT_UI_X11_STATUS_BAR_H
#define XPPAUT_UI_X11_STATUS_BAR_H

#include <X11/Xlib.h>

/* --- Types --- */
typedef struct X11StatusBar X11StatusBar;

/* --- Functions --- */
/**
 * Allocate and map a new status bar widget.
 *
 * The status bar must be freed with x11_status_bar_free.
 *
 * @param parent the window to create the status bar in.
 * @param x x-coordinate of status bar.
 * @param y y-coordinate of status bar.
 * @param w width of status bar.
 * @param h height of status bar.
 * @return a new object, or NULL on error.
 */
X11StatusBar *x11_status_bar_alloc(Window parent, int x, int y, unsigned int w,
                                   unsigned int h);
void x11_status_bar_free(X11StatusBar *sb);

/**
 * Set new position and size.
 *
 * @param sb the status bar.
 * @param x x-coordinate of status bar.
 * @param y y-coordinate of status bar.
 * @param w width of status bar.
 * @param h height of status bar.
 */
void x11_status_bar_set_extents(X11StatusBar *sb, int x, int y, unsigned int w,
                                unsigned int h);

/**
 * Set the text.
 *
 * @param sb the status bar.
 * @param text the new text.
 */
void x11_status_bar_set_text(X11StatusBar *sb, const char *text);

#endif /* XPPAUT_UI_X11_STATUS_BAR_H */
