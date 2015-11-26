#include "menu.h"

#include <stdlib.h>
#include <X11/cursorfont.h>

#include "ggets.h"
#include "main.h"
#include "base/vector.h"
#include "ui-x11/window.h"

/* --- Macros --- */
#define EV_MASK                                                                \
  (ButtonPressMask | ButtonReleaseMask | KeyPressMask | ExposureMask |         \
   StructureNotifyMask | EnterWindowMask | LeaveWindowMask)

/* --- Types --- */
VECTOR_DECLARE(windows, Windows, Window)
VECTOR_DEFINE(windows, Windows, Window)

struct X11Menu {
  X11MenuDescr descr;
  X11StatusBar *sb;
  X11MenuSelectFunc select_func;
  void *cookie;

  int max_len;
  Window title_win;
  Windows entry_wins;
};

static void x11_menu_draw_title(const X11Menu *m) {
  XClearWindow(display, m->title_win);
  set_back();
  Ftext(DCURX * 2, 4, m->descr.title, m->title_win);
  set_fore();
}

static void x11_menu_draw_entry(const X11Menu *m, int i) {
  Ftext(DCURX / 2, 3, m->descr.entries[i].label, m->entry_wins.elems[i]);
  if (m->descr.entries[i].key == m->descr.def_key)
    Ftext(DCURX * (m->max_len + 1), 4, "X", m->entry_wins.elems[i]);
}

static void x11_menu_draw(const X11Menu *m) {
  x11_menu_draw_title(m);

  for (int i = 0; i < m->entry_wins.len; i++)
    x11_menu_draw_entry(m, i);
}

void x11_menu_event(X11Menu *m, const XEvent *ev) {
  int key;

  switch (ev->type) {
  case Expose:
  case MapNotify:
    x11_menu_draw(m);
    break;

  case KeyPress:
    key = get_key_press(ev);
    if (key == '\r')
      key = m->descr.def_key;
    if (key)
      m->select_func(m->cookie, key);
    break;

  case ButtonRelease:
    for (int i = 0; i < m->entry_wins.len; i++) {
      if (ev->xbutton.window == m->entry_wins.elems[i]) {
        m->select_func(m->cookie, m->descr.entries[i].key);
        break;
      }
    }

    break;

  case EnterNotify:
    for (int i = 0; i < m->entry_wins.len; i++) {
      if (ev->xcrossing.window == m->entry_wins.elems[i]) {
        XSetWindowBorderWidth(display, m->entry_wins.elems[i], 1);
        if (TipsFlag)
          x11_status_bar_set_text(m->sb, m->descr.entries[i].hint);
        break;
      }
    }
    break;

  case LeaveNotify:
    for (int i = 0; i < m->entry_wins.len; i++) {
      if (ev->xcrossing.window == m->entry_wins.elems[i]) {
        XSetWindowBorderWidth(display, m->entry_wins.elems[i], 0);
        break;
      }
    }
    break;
  }
}

X11Menu *x11_menu_alloc(const X11MenuDescr *descr, Window parent, int x, int y,
                        unsigned int w, X11StatusBar *sb, X11MenuSelectFunc select_func, void *cookie) {
  X11Menu *ret = malloc(sizeof(*ret));
  if (!ret)
    return NULL;

  ret->descr = *descr;
  ret->sb = sb;
  ret->select_func = select_func;
  ret->cookie = cookie;

  ret->max_len = 0;
  for (int i = 0; i < descr->num_entries; i++) {
    size_t n = strlen(descr->entries[i].label);
    if (n > ret->max_len)
      ret->max_len = n;
  }

  if (!w)
    w = DCURX * (ret->max_len + 3);

  ret->title_win = XCreateSimpleWindow(display, parent, x, y, w, DCURY + 7, 0,
                                       MyBackColor, MyForeColor);
  XSelectInput(display, ret->title_win, ExposureMask);
  XMapWindow(display, ret->title_win);

  Cursor cursor = XCreateFontCursor(display, XC_hand2);

  windows_init(&ret->entry_wins, descr->num_entries);
  if (!windows_insert(&ret->entry_wins, 0, descr->num_entries)) {
    XDestroyWindow(display, ret->title_win);
    free(ret);
    return NULL;
  }
  for (int i = 0; i < descr->num_entries; i++) {
    ret->entry_wins.elems[i] =
        make_window(parent, x, y + DCURY + 7 + i * (DCURY + 5),
                    w, DCURY + 3, 0);
    XDefineCursor(display, ret->entry_wins.elems[i], cursor);
    XSelectInput(display, ret->entry_wins.elems[i], EV_MASK);
  }

  return ret;
}

void x11_menu_free(X11Menu *m) {
  for (int i = 0; i < m->entry_wins.len; i++)
    XDestroyWindow(display, m->entry_wins.elems[i]);
  windows_clean(&m->entry_wins);
  XDestroyWindow(display, m->title_win);
  free(m);
}
