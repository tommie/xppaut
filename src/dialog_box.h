
#ifndef XPPAUT_DIALOG_BOX_H
#define XPPAUT_DIALOG_BOX_H

#include <X11/Xlib.h>
#include "xpplim.h"

/* --- Macros --- */
/* get_dialog return values */
#define FORGET_ALL 0
#define DONE_WITH_THIS 1
#define ALL_DONE 2

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
int get_dialog(char *wname, char *name, char *value, char *ok, char *cancel,
               int max);

#endif /* XPPAUT_DIALOG_BOX_H */
