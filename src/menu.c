#include "menu.h"

#include <stdlib.h>
#include <X11/Xlib.h>

#include "main.h"
#include "menus.h"
#include "ui-x11/menu.h"

/* --- Data --- */
#define ME(k, l, h)                                                            \
  { .key = k, .label = l, .hint = h }
static const X11MenuEntry MAIN_MENU_ENTRIES[] = {
    ME('i', "Initialconds", "Integrate the equations"),
    ME('c', "Continue", "Continue integration for specified time"),
    ME('n', "Nullcline", "Draw nullclines"),
    ME('d', "Dir.field/flow", "Direction fields and flows of the phaseplane"),
    ME('w', "Window/zoom", "Change the size of two-dimensional view"),
    ME('a', "phAsespace", "Set up periodic/torus phase space"),
    ME('k', "Kinescope", "Take snapshots of the screen"),
    ME('g', "Graphic stuff", "Adding graphs,hard copy, etc"),
    ME('u', "nUmerics", "Numerics options"),
    ME('f', "File", "Quit, save stuff, etc"),
    ME('p', "Parameters", "Change problem parameters"),
    ME('e', "Erase", "Clear screen"),
    ME('m', "Makewindow", "Create other windows"),
    ME('t', "Text,etc", "Add fancy text and lines,arrows"),
    ME('s', "Sing pts", "Find fixed points and stability"),
    ME('v', "Viewaxes", "Change 2 or 3d views"),
    ME('x', "Xi vs t", "Plot variable vs time"),
    ME('r', "Restore", "Redraw the graph"),
    ME('3', "3d-params", "Set parameters for 3D view"),
    ME('b', "Bndryval", "Run boundary value solver"),
};
#undef ME
static const X11MenuDescr MAIN_MENU_DESCR = {
    .title = "XPP",
    .entries = (X11MenuEntry *)MAIN_MENU_ENTRIES,
    .num_entries = sizeof(MAIN_MENU_ENTRIES) / sizeof(*MAIN_MENU_ENTRIES),
    .def_key = -1,
};
static X11Menu *g_main_menu;

static void main_menu_select(void *cookie, int key) {
  if (key > 0)
    commander(key);
}

void main_menu_create(Window base) {
  g_main_menu =
      x11_menu_alloc(&MAIN_MENU_DESCR, base, 0, DCURYs + DCURYb + 10,
                     16 * DCURX, main_status_bar, main_menu_select, NULL);
  if (!g_main_menu)
    exit(1);
}

void main_menu_event(const XEvent *ev) { x11_menu_event(g_main_menu, ev); }
