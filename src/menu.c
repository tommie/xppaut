#include "menu.h"

#include <stdlib.h>
#include <X11/Xlib.h>

#include "main.h"
#include "menus.h"
#include "ui-x11/menu.h"

/* --- Data --- */
static X11Menu *g_main_menu;

void flash(int num)
{}

static void main_menu_select(void *cookie, int key) {
  if (key > 0)
    commander(key);
}

void main_menu_create(Window base) {
  static const char MAIN_KEYS[] = "icndwakgufpemtsvxr3b";
  X11MenuDescr descr;

  descr.title = "XPP";
  descr.entries = calloc(MAIN_ENTRIES, sizeof(*descr.entries));
  if (!descr.entries)
    exit(1);
  for (int i = 0; i < MAIN_ENTRIES; ++i) {
    descr.entries[i].label = main_menu[i];
    descr.entries[i].hint = main_hint[i];
    descr.entries[i].key = MAIN_KEYS[i];
  }
  descr.num_entries = MAIN_ENTRIES;
  descr.def_key = -1;

  g_main_menu = x11_menu_alloc(&descr, base, 0, DCURYs + DCURYb + 10, 16 * DCURX, main_status_bar, main_menu_select, NULL);
  if (!g_main_menu) exit(1);
}

void main_menu_event(const XEvent *ev) {
  x11_menu_event(g_main_menu, ev);
}
