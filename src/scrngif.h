#ifndef XPPAUT_SCRNGIF_H
#define XPPAUT_SCRNGIF_H

#include <stdio.h>
#include <X11/Xlib.h>

/* --- Types --- */
typedef struct GifTree {
  char typ;         /* terminating, lookup, or search */
  int code;         /* the code to be output */
  unsigned char ix; /* the color map index */
  struct GifTree **node, *nxt, *alt;
} GifTree;

typedef struct { unsigned char r, g, b; } GIFCOL;

/* --- Functions --- */
void add_ani_gif(Window win, FILE *fp, int count);
unsigned char *AddCodeToBuffer(int code, short n, unsigned char *buf);
void end_ani_gif(FILE *fp);
void get_global_colormap(Window win);
void screen_to_gif(Window win, FILE *fp);
void set_global_map(int flag);

#endif /* XPPAUT_SCRNGIF_H */
