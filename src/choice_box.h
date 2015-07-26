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
void destroy_choice(CHOICE_BOX p);
void display_choice(Window w, CHOICE_BOX p);
void do_checks(CHOICE_BOX p);
void base_choice(char *wname, int n, int mcc, char **names, int *check, int type);
int do_choice_box(Window root, char *wname, int n, int mcc, char **names, int *check, int type);
int choice_box_event_loop(CHOICE_BOX p);

#endif /* XPPAUT_CHOICE_BOX_H */
