#ifndef XPPAUT_BROWSE_H
#define XPPAUT_BROWSE_H

#include <stdio.h>
#include <X11/Xlib.h>

/* --- Macros --- */
#define BMAXCOL 20

/* --- Types --- */
typedef struct {
  Window base, upper;
  Window find, up, down, pgup, pgdn, home, end, left, right;
  Window first, last, restore, write, get, close;
  Window load, repl, unrepl, table, addcol, delcol;
  Window main;
  Window label[BMAXCOL];
  Window time;
  Window hint;
  char hinttxt[256];
  int xflag;
  int col0, row0, ncol, nrow;
  int maxrow, maxcol;
  float **data;
  int istart, iend;
} BROWSER;

/* --- Data --- */
extern BROWSER my_browser;

/* --- Functions --- */
float **get_browser_data(void);
float *get_data_col(int c);
Window br_button(Window root, int row, int col, char *name, int iflag);
void data_get_mybrowser(int row);
void find_variable(char *s, int *col);
void get_data_xyz(float *x, float *y, float *z, int i1, int i2, int i3,
                  int off);
int get_maxrow_browser(void);
void init_browser(void);
void make_new_browser(void);
void open_write_file(FILE **fp, char *fil, int *ok);
void reset_browser(void);
void set_browser_data(float **data, int maxrow, int maxcol);
void wipe_rep(void);
void write_mybrowser_data(FILE *fp);

#endif /* XPPAUT_BROWSE_H */
