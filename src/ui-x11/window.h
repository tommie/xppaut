#ifndef XPPAUT_UI_X11_WINDOW_H
#define XPPAUT_UI_X11_WINDOW_H

#include <X11/Xlib.h>

/* --- Functions --- */
void get_new_size(Window win, unsigned int *wid, unsigned int *hgt);
void make_icon(const char *icon, int wid, int hgt, Window w);
Window make_icon_window(Window root, int x, int y, int width, int height,
                        int bw, int icx, int icy, unsigned char *icdata);
Window make_plain_unmapped_window(Window root, int x, int y, int width,
                                  int height, int bw);
Window make_plain_window(Window root, int x, int y, int width, int height,
                         int bw);
Window make_unmapped_window(Window root, int x, int y, int width, int height,
                            int bw);
Window make_window(Window root, int x, int y, int width, int height, int bw);

#endif /* XPPAUT_UI_X11_WINDOW_H */
