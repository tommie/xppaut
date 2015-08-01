#ifndef _userbut_h_
#define _userbut_h_

#include <X11/Xlib.h>

typedef struct {
  Window w;
  char bname[10];
  int com;
} USERBUT;

void add_user_button(char *s);
void create_user_buttons(int x0, int y0, Window base);
void user_button_events(XEvent report);



#endif
