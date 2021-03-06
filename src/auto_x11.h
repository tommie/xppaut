#ifndef XPPAUT_AUTO_X11_H
#define XPPAUT_AUTO_X11_H

#include <X11/Xlib.h>

#include "ui-x11/rubber.h"

/* --- Data --- */
extern int AutoRedrawFlag;
extern int mark_flag;
extern int mark_ibrs;
extern int mark_ipte;
extern int mark_ipts;

/* --- Functions --- */
void ALINE(int a, int b, int c, int d);
void ATEXT(int a, int b, char *c);
void Circle(int x, int y, int r);
void DLINE(double a, double b, double c, double d);
void FillCircle(int x, int y, int r);
void LineWidth(int wid);
void auto_button(XEvent ev);
void auto_enter(Window w, int v);
void auto_get_info(int *n, char *pname);
void auto_keypress(XEvent ev, int *used);
void auto_motion(XEvent ev);
int auto_pop_up_list(char *title, char **list, char *key, int n, int max,
                     int def, int x, int y, char **hints);
void auto_rubber(X11RubberType t, X11RubberEndFunc end_func, void *cookie);
void auto_set_mark(int i);
void auto_stab_line(int x, int y, int xp, int yp);
void autobw(void);
void autocol(int col);
int check_stop_auto(void);
void clear_auto_info(void);
void clear_auto_plot(void);
void clr_stab(void);
void display_auto(Window w);
void do_auto_range();
void draw_auto_info(char *bob, int x, int y);
void make_auto(char *wname, char *iname);
void redraw_auto_menus(void);
void refreshdisplay(void);
void resize_auto_window(XEvent ev);

#endif /* XPPAUT_AUTO_X11_H */
