
#ifndef XPPAUT_DIALOG_BOX_H
#define XPPAUT_DIALOG_BOX_H

#include <X11/Xlib.h>
#include "xpplim.h"

/* --- Types --- */
typedef struct {
  Window mes;
  Window ok;
  Window cancel;
  Window input;
  Window base;
  char mes_s[MAXCHAR];
  char input_s[MAXCHAR];
  char ok_s[MAXCHAR];
  char cancel_s[MAXCHAR];
} DIALOG;

/* --- Functions --- */
int get_dialog(char *wname, char *name, char *value, char *ok, char *cancel, int max);
int dialog_event_loop(DIALOG *d, int max, int *pos, int *col);
void display_dialog(Window w, DIALOG d, int pos, int col);


#endif /* XPPAUT_DIALOG_BOX_H */
