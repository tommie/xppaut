#ifndef XPPAUT_CHOICE_BOX_H
#define XPPAUT_CHOICE_BOX_H

#include <X11/Xlib.h>
#include "xpplim.h"

/* --- Types --- */
typedef struct {
  char title[MAXCHAR];
  int n;
  Window base;
  Window ok;
  Window cancel;
  short type;
  int mc;
  Window cw[MAXENTRY];
  char **name;
  int *flag;
} CHOICE_BOX;

/* --- Functions --- */
void display_choice(Window w, CHOICE_BOX p);

#endif /* XPPAUT_CHOICE_BOX_H */
