#ifndef XPPAUT_POP_LIST_H
#define XPPAUT_POP_LIST_H

#include <X11/Xlib.h>

#include "ui-x11/status-bar.h"

/* --- Macros --- */
#define MAX_LEN_SBOX 25

/* --- Functions --- */
void do_hilite_text(char *name, char *value, int flag, Window w, int pos,
                    int col);
int do_string_box(int n, int row, int col, char *title, char **names,
                  char values[][25], int maxchar);
void get_new_size(Window win, unsigned int *wid, unsigned int *hgt);
Window make_fancy_window(Window root, int x, int y, int width, int height,
                         int bw, int fc, int bc);
void make_icon(const char *icon, int wid, int hgt, Window w);
Window make_icon_window(Window root, int x, int y, int width, int height,
                        int bw, int icx, int icy, unsigned char *icdata);
Window make_plain_unmapped_window(Window root, int x, int y, int width,
                                  int height, int bw);
Window make_plain_window(Window root, int x, int y, int width, int height,
                         int bw);
void make_scrbox_lists(void);
Window make_unmapped_window(Window root, int x, int y, int width, int height,
                            int bw);
Window make_window(Window root, int x, int y, int width, int height, int bw);
void message_box(Window *w, int x, int y, char *message);
int pop_up_list(Window root, const char *title, char * const *list, const char *key, int n,
                int max, int def, int x, int y, char * const *hints, X11StatusBar *sb);
void respond_box(const char *button, const char *message);
void set_window_title(Window win, char *string);
int two_choice(char *choice1, char *choice2, char *string, char *key, int x,
               int y, Window w, char *title);
int yes_no_box(void);

#endif /* XPPAUT_POP_LIST_H */
