#ifndef XPPAUT_MENU_H
#define XPPAUT_MENU_H

#include <X11/Xlib.h>

/* --- Functions --- */
void flash(int num);
void main_menu_create(Window base);
void main_menu_event(const XEvent *ev);

#endif /* XPPAUT_MENU_H */
