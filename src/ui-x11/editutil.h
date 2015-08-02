#ifndef XPPAUT_UI_X11_EDITUTIL_H
#define XPPAUT_UI_X11_EDITUTIL_H

#include <X11/Xlib.h>

/* --- Macros --- */
#define EDIT_WAIT 0
#define EDIT_NEXT 1
#define EDIT_ESC 2
#define EDIT_DONE 3

/* --- Functions --- */
void put_edit_cursor(Window w, int pos);

#endif /* XPPAUT_UI_X11_EDITUTIL_H */
