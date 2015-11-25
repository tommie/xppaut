#include "menu.h"

#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>

#include "ggets.h"
#include "main.h"
#include "many_pops.h"
#include "menus.h"
#include "ui-x11/window.h"

/* --- Types --- */
typedef struct {
  Window base;
  Window title;
  Window w[25];
  char key[25];
  char **names;
  char **hints;
  int n;
  int visible;
} MENUDEF;

/* --- Forward Declarations --- */
static void add_menu(Window base, int j, int n, char **names, char *key,
                     char **hint);

/* --- Data --- */
static MENUDEF my_menus[1];

void flash(int num)
{}

static void add_menu(Window base, int j, int n, char **names, char *key, char **hint)
{
  Window w;
  int i;
  Cursor cursor;
  cursor = XCreateFontCursor(display, XC_hand2);
  w = make_plain_unmapped_window(base, 0, DCURYs + DCURYb + 10, 16 * DCURX,
                                 21 * (DCURY + 2) - 3, 1);
  my_menus[j].base = w;
  XDefineCursor(display, w, cursor);
  my_menus[j].names = names;
  my_menus[j].n = n;
  my_menus[j].hints = hint;
  strcpy(my_menus[j].key, key);
  my_menus[j].title = make_unmapped_window(w, 0, 0, 16 * DCURX, DCURY, 1);
  for (i = 0; i < n; i++) {
    my_menus[j].w[i] =
        make_unmapped_window(w, 0, (i + 1) * (DCURY + 2), 16 * DCURX, DCURY, 0);
  }
  my_menus[j].visible = 0;
  XMapRaised(display, my_menus[j].base);
  XMapSubwindows(display, my_menus[j].base);
}

void create_the_menus(Window base)
{
  char key[30];
  strcpy(key, "icndwakgufpemtsvxr3b");
  add_menu(base, MAIN_MENU, MAIN_ENTRIES, main_menu, key, main_hint);
  my_menus[0].visible = 1;
}

void menu_crossing(Window win, int yn)
{
  int i, n, j = 0;
  char **z;
  if (j < 0)
    return;
  if (my_menus[j].visible == 0)
    return;
  n = my_menus[j].n;
  z = my_menus[j].hints;
  for (i = 0; i < n; i++) {
    if (win == my_menus[j].w[i]) {
      XSetWindowBorderWidth(display, win, yn);
      if (yn && TipsFlag)
        x11_status_bar_set_text(main_status_bar, z[i]);
      return;
    }
  }
}

void menu_expose(Window win)
{
  int i, n, j = 0;
  char **z;
  if (j < 0)
    return;
  if (my_menus[j].visible == 0)
    return;
  n = my_menus[j].n;
  z = my_menus[j].names;
  if (win == my_menus[j].title) {
    set_fore();
    bar(0, 0, 16 * DCURX, DCURY, win);
    set_back();
    XDrawString(display, win, gc, DCURX / 2 + 5, CURY_OFF, z[0], strlen(z[0]));
    set_fore();
    /* BaseCol();
    XDrawString(display,win,gc,0,CURY_OFF,z[0],strlen(z[0]));
    */
    return;
  }
  for (i = 0; i < n; i++) {
    if (win == my_menus[j].w[i]) {
      BaseCol();
      XDrawString(display, win, gc, 5, CURY_OFF, z[i + 1], strlen(z[i + 1]));
      return;
    }
  }
}

void menu_button(Window win)
{
  int i, n, j = 0;
  if (j < 0)
    return;
  if (my_menus[j].visible == 0)
    return;
  n = my_menus[j].n;
  for (i = 0; i < n; i++) {
    if (win == my_menus[j].w[i]) {
      XSetWindowBorderWidth(display, win, 0);
      commander(my_menus[j].key[i]);
      return;
    }
  }
}

void draw_help(void) {
  int i, j = 0, n;
  /*char **z;
  */
  if (j < 0)
    return;
  if (my_menus[j].visible == 0)
    return;
  n = my_menus[j].n;
  /*z=my_menus[j].names;
  */
  menu_expose(my_menus[j].title);
  for (i = 0; i < n; i++)
    menu_expose(my_menus[j].w[i]);
}
