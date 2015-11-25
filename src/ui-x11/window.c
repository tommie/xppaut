#include "window.h"

#include <X11/Xutil.h>

#include "main.h"

void get_new_size(Window win, unsigned int *wid, unsigned int *hgt) {
  int x, y;
  unsigned int bw, de;
  Window root;
  XGetGeometry(display, win, &root, &x, &y, wid, hgt, &bw, &de);
}

static void draw_gradient(Pixmap pmap, int width, int height) {
  int xx, yy;
  double cosine;
  XColor bcolour, col2, diffcol;
  Colormap cmap = DefaultColormap(display, DefaultScreen(display));
  XParseColor(display, cmap, UserWhite, &bcolour);
  XParseColor(display, cmap, UserBlack, &diffcol);

  for (yy = 0; yy < height; yy += 1) {
    if (yy < 1.0) {
      col2.red = 65535;
      col2.green = 65355;
      col2.blue = 65355;
    } else {
      if (yy < (height / 2.0)) {
        cosine = 1.0;
      } else if ((height - yy) <= 1.0) {
        cosine = 0.1;
      } else {
        cosine = 0.93;
      }
      col2.red = bcolour.red * cosine;
      col2.green = bcolour.green * cosine;
      col2.blue = bcolour.blue * cosine;
    }

    XAllocColor(display, cmap, &col2);
    XSetForeground(display, gc, col2.pixel);

    for (xx = 1; xx < width - 1; xx += 1) {
      XDrawPoint(display, pmap, gc, xx, yy);
    }

    /*Now do xx=0 and xx=width-1*/
    xx = 0;
    col2.red = 65535;
    col2.green = 65355;
    col2.blue = 65355;
    XAllocColor(display, cmap, &col2);
    XSetForeground(display, gc, col2.pixel);
    XDrawPoint(display, pmap, gc, xx, yy);
    xx = width - 1;
    cosine = 0.1;
    col2.red = bcolour.red * cosine;
    col2.green = bcolour.green * cosine;
    col2.blue = bcolour.blue * cosine;

    XAllocColor(display, cmap, &col2);
    XSetForeground(display, gc, col2.pixel);
    XDrawPoint(display, pmap, gc, xx, yy);
  }
}

Window make_plain_unmapped_window(Window root, int x, int y, int width,
                                  int height, int bw) {
  Window win;
  win = XCreateSimpleWindow(display, root, x, y, width, height, bw, MyForeColor,
                            MyBackColor);

  if (root == RootWindow(display, screen))
    XSetWMProtocols(display, win, &deleteWindowAtom, 1);
  XSelectInput(display, win, ExposureMask | KeyPressMask | ButtonPressMask |
                                 StructureNotifyMask | ButtonReleaseMask |
                                 ButtonMotionMask | LeaveWindowMask |
                                 EnterWindowMask);

  return win;
}

Window make_unmapped_window(Window root, int x, int y, int width, int height,
                            int bw) {
  Window win = make_plain_unmapped_window(root, x, y, width, height, bw);

  if (UserGradients == 1) {
    Pixmap pmap = XCreatePixmap(display, root, width, height,
                                DefaultDepth(display, DefaultScreen(display)));

    draw_gradient(pmap, width, height);
    XSetWindowBackgroundPixmap(display, win, pmap);
    XFreePixmap(display, pmap);
  }

  return win;
}

static void bin_prnt_byte(int x, int *arr) {
  int n = 0;
  for (n = 7; n >= 0; n--) {
    if ((x & 0x80) != 0) {
      arr[n] = 1;
    } else {
      arr[n] = 0;
    }

    x = x << 1;
  }

  return;
}

/*Convenience function for making buttons with icons on them*/
Window make_unmapped_icon_window(Window root, int x, int y, int width,
                                 int height, int bw, int icx, int icy,
                                 unsigned char *icdata) {
  Window win = make_plain_unmapped_window(root, x, y, width, height, bw);
  Pixmap pmap = XCreatePixmap(display, root, width, height,
                              DefaultDepth(display, DefaultScreen(display)));
  Colormap cmap = DefaultColormap(display, DefaultScreen(display));

  if (UserGradients == 1) {
    draw_gradient(pmap, width, height);
  } else {
    XColor bcolor;

    XParseColor(display, cmap, UserWhite, &bcolor);
    XAllocColor(display, cmap, &bcolor);
    XSetForeground(display, gc, bcolor.pixel);
    XDrawRectangle(display, pmap, gc, 0, 0, width, height);
  }

  int z = 0, row = 0, col = 0;

  if (icdata != NULL) {
    XColor diffcol;

    XParseColor(display, cmap, UserBlack, &diffcol);
    XAllocColor(display, cmap, &diffcol);
    XSetForeground(display, gc, diffcol.pixel);

    unsigned char *ps = icdata;

    int intstack[8];

    col = 0;
    row = -1;
    z = 0;
    while (row < height) {
      col = 0;
      row++;
      while (1) {
        bin_prnt_byte(*ps, intstack);
        ps++;

        int q = 0;
        for (q = 0; q < 8; q++) /*8 bits per byte*/
        {
          if (col >= width) {
            /*myint[z] = 0;*/
          } else {
            if (intstack[q] == 1) {
              XDrawPoint(display, pmap, gc, col, row);
            }
          }
          z++;
          col++;
        }

        if (col >= width) {

          break;
        }
      }
    }
  }

  XSetWindowBackgroundPixmap(display, win, pmap);
  XFreePixmap(display, pmap);

  return win;
}

Window make_icon_window(Window root, int x, int y, int width, int height,
                        int bw, int icx, int icy, unsigned char *icdata) {
  Window win = make_unmapped_icon_window(root, x, y, width, height, bw, icx,
                                         icy, icdata);
  XMapWindow(display, win);
  return win;
}

Window make_window(Window root, int x, int y, int width, int height, int bw) {
  Window win = make_unmapped_window(root, x, y, width, height, bw);
  XMapWindow(display, win);
  return win;
}

Window make_plain_window(Window root, int x, int y, int width, int height,
                         int bw) {
  Window win = make_plain_unmapped_window(root, x, y, width, height, bw);
  XMapWindow(display, win);
  return win;
}

void make_icon(const char *icon, int wid, int hgt, Window w) {
  Pixmap icon_map;
  XWMHints wm_hints;
  icon_map = XCreateBitmapFromData(display, w, icon, wid, hgt);
  wm_hints.initial_state = NormalState;
  wm_hints.input = True;
  wm_hints.icon_pixmap = icon_map;
  wm_hints.flags = StateHint | IconPixmapHint | InputHint;

  XClassHint class_hints;
  class_hints.res_name = "";
  class_hints.res_class = "";
  XSetWMProperties(display, w, NULL, NULL, NULL, 0, NULL, &wm_hints,
                   &class_hints);
}
