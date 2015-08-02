#include "editutil.h"

#include "../main.h"

void put_edit_cursor(Window w, int pos) {
  int x1 = pos;
  int x2 = x1 + 1;
  XDrawLine(display, w, small_gc, x1, 1, x1, DCURYs - 1);
  XDrawLine(display, w, small_gc, x2, 1, x2, DCURYs - 1);
}
