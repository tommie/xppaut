#ifndef XPPAUT_UI_X11_MENU_H
#define XPPAUT_UI_X11_MENU_H

#include <stdint.h>
#include <X11/Xlib.h>

#include "status-bar.h"

/* --- Types --- */
typedef struct {
  char *label;
  char *hint;
  int key;
} X11MenuEntry;

typedef struct {
  char *title;
  X11MenuEntry *entries;
  size_t num_entries;
  int def_key;
} X11MenuDescr;
typedef void (*X11MenuSelectFunc)(void *cookie, int key);
typedef struct X11Menu X11Menu;

/* --- Functions --- */
X11Menu *x11_menu_alloc(const X11MenuDescr *descr, Window parent, int x, int y,
                        unsigned int w, X11StatusBar *sb,
                        X11MenuSelectFunc select_func, void *cookie);
void x11_menu_free(X11Menu *m);
void x11_menu_event(X11Menu *m, const XEvent *ev);

#endif /* XPPAUT_UI_X11_MENU_H */
