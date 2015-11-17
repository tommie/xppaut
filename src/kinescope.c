/* Kinescope for X windows */
#include "kinescope.h"

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>

#include "aniparse.h"
#include "ggets.h"
#include "main.h"
#include "mykeydef.h"
#include "pop_list.h"
#include "scrngif.h"
#include "base/timeutil.h"

/* --- Macros --- */
#define EV_MASK ButtonPressMask | ButtonReleaseMask | KeyPressMask
#define MAXFILM 250

/* --- Types --- */
typedef struct {
  unsigned int h, w;
  Pixmap xi;
} MOVIE;

typedef struct {
  unsigned int w;
  unsigned int h;
  int frame;

  int ncycles;
  int ival;
  int cycle;
  int curr_ival;
} Player;

/* --- Forward Declarations --- */
static void auto_play(void);
static void auto_play_stop(Player *p);
static void make_anigif(void);
static void play_back(void);
static void play_back_stop(Player *p);
static void save_kine(void);
static void save_movie(char *basename, int fmat);
static void too_small(void);

/* --- Data --- */
int mov_ind;

static MOVIE movie[MAXFILM];

void do_movie_com(int c) {
  /*  XDestroyWindow(display,temp);
    draw_help();
    XFlush(display); */
  switch (c) {

  case 0:
    if (film_clip() == 0)
      respond_box("Okay", "Out of film!");
    break;
  case 1:
    reset_film();
    break;
  case 2:
    play_back();
    break;
  case 3:
    auto_play();
    break;
  case 4:
    save_kine();
    break;
  case 5:
    make_anigif();
    break;
  case 6: /* test_keys(); */
    break;
  }
}

void reset_film(void) {
  int i;
  if (mov_ind == 0)
    return;
  for (i = 0; i < mov_ind; i++)
    XFreePixmap(display, movie[i].xi);
  mov_ind = 0;
}

int film_clip(void) {
  int x, y;
  unsigned int h, w, bw, d;
  Window root;
  if (mov_ind >= MAXFILM)
    return (0);
  XGetGeometry(display, draw_win, &root, &x, &y, &w, &h, &bw, &d);
  movie[mov_ind].h = h;
  movie[mov_ind].w = w;
  movie[mov_ind].xi = XCreatePixmap(display, RootWindow(display, screen), w, h,
                                    DefaultDepth(display, screen));
  XCopyArea(display, draw_win, movie[mov_ind].xi, gc_graph, 0, 0, w, h, 0, 0);
  mov_ind++;
  return 1;
}

static int show_frame(const Player *p) {
  if (p->h < movie[p->frame].h || p->w < movie[p->frame].w) {
    too_small();
    return 1;
  }
  XCopyArea(display, movie[p->frame].xi, draw_win, gc_graph, 0, 0, p->w, p->h,
            0, 0);
  XFlush(display);

  return 0;
}

static void play_back_event(void *cookie, const XEvent *ev) {
  Player *p = cookie;

  switch (ev->type) {
  case ButtonRelease:
    p->frame++;
    if (p->frame >= mov_ind)
      p->frame = 0;
    if (show_frame(p))
      play_back_stop(p);
    break;

  case KeyPress:
    switch (get_key_press(ev)) {
    case ESC:
      play_back_stop(p);
      break;

    case RIGHT:
      p->frame++;
      if (p->frame >= mov_ind)
        p->frame = 0;
      if (show_frame(p))
        play_back_stop(p);
      break;

    case LEFT:
      p->frame--;
      if (p->frame < 0)
        p->frame = mov_ind - 1;
      if (show_frame(p))
        play_back_stop(p);
      break;
    case HOME:
      p->frame = 0;
      if (show_frame(p))
        play_back_stop(p);
      break;
    case END:
      p->frame = mov_ind - 1;
      if (show_frame(p))
        play_back_stop(p);
      break;
    }
  }
}

static void play_back(void) {
  int x, y;
  unsigned int bw, d;
  Window root;
  Player *p = malloc(sizeof(*p));
  if (!p)
    return;

  p->frame = 0;
  XGetGeometry(display, draw_win, &root, &x, &y, &p->w, &p->h, &bw, &d);
  if (mov_ind == 0)
    return;

  if (show_frame(p))
    return;

  x11_events_listen(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK,
                    play_back_event, p);
}

static void play_back_stop(Player *p) {
  x11_events_unlisten(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK,
                      play_back_event, p);
  free(p);
}

static void save_kine(void) {
  char base[128];
  int fmat = 2;
  sprintf(base, "frame");
  /* #ifdef NOGIF
 #else
 new_int("format:1-ppm,2-gif",&fmat);
 #endif
  */
  new_string("Base file name", base);
  if (strlen(base) > 0)
    save_movie(base, fmat);
}

static void make_anigif(void) {
  int i = 0;
  int x, y;
  FILE *fp;
  Window root;
  unsigned int h, w, bw, d;
  XGetGeometry(display, draw_win, &root, &x, &y, &w, &h, &bw, &d);
  if (mov_ind == 0)
    return;
  if (h < movie[i].h || w < movie[i].w) {
    too_small();
    return;
  }
  h = movie[0].h;
  w = movie[0].w;
  for (i = 0; i < mov_ind; i++) {
    if ((movie[i].h != h) || (movie[i].w != w)) {
      err_msg("All clips must be same size");
      return;
    }
  }
  fp = fopen("anim.gif", "wb");
  set_global_map(1);
  for (i = 0; i < mov_ind; i++) {
    XCopyArea(display, movie[i].xi, draw_win, gc_graph, 0, 0, w, h, 0, 0);
    XFlush(display);
    /* add_ani_gif(draw_win,fp,i); */
    add_ani_gif(movie[i].xi, fp, i);
  }

  end_ani_gif(fp);
  fclose(fp);
  set_global_map(0);
}

static void save_movie(char *basename, int fmat) {
  /*char file[256];
  */
  char file[XPP_MAX_NAME];
  int i = 0;
  int x, y;
  FILE *fp;
  Window root;
  Pixmap xi;
  unsigned int h, w, bw, d;
  XGetGeometry(display, draw_win, &root, &x, &y, &w, &h, &bw, &d);
  if (mov_ind == 0)
    return;
  if (h < movie[i].h || w < movie[i].w) {
    too_small();
    return;
  }

  for (i = 0; i < mov_ind; i++) {
    if (fmat == 1)
      sprintf(file, "%s_%d.ppm", basename, i);
    else
      sprintf(file, "%s_%d.gif", basename, i);
    XCopyArea(display, movie[i].xi, draw_win, gc_graph, 0, 0, w, h, 0, 0);
    XFlush(display);
    if (fmat == 1)
      writeframe(file, draw_win, w, h);
#ifndef NOGIF
    else {
      XGetGeometry(display, draw_win, &root, &x, &y, &w, &h, &bw, &d);
      xi = XCreatePixmap(display, RootWindow(display, screen), w, h,
                         DefaultDepth(display, screen));
      XCopyArea(display, draw_win, xi, gc_graph, 0, 0, w, h, 0, 0);

      fp = fopen(file, "wb");
      screen_to_gif(xi, fp);
      fclose(fp);
    }
#endif
  }
}

static void auto_play_event(void *cookie, const XEvent *ev) {
  const int dt = 20;
  const int smax = 500;
  Player *p = cookie;
  unsigned int key;

  switch (ev->type) {
  case ButtonRelease:
    auto_play_stop(p);
    break;

  case KeyPress:
    key = get_key_press(ev);
    switch (key) {
    case ESC:
      auto_play_stop(p);
      break;

    case ',':
      p->ival -= dt;
      if (p->ival < dt)
        p->ival = dt;
      break;

    case '.':
      p->ival += dt;
      if (p->ival > smax)
        p->ival = smax;
      break;
    }
    break;
  }
}

static void auto_play_timeout(void *cookie) {
  Player *p = cookie;

  p->frame++;
  if (p->frame >= mov_ind) {
    p->cycle++;
    p->frame = 0;
    if (p->cycle >= p->ncycles) {
      auto_play_stop(p);
      return;
    }
  }

  if (show_frame(p)) {
    auto_play_stop(p);
    return;
  }

  if (p->ival != p->curr_ival) {
    struct timeval rtv = {.tv_sec = 0, .tv_usec = p->ival * 1000};

    x11_events_timeout_cancel(g_x11_events, auto_play_timeout, p);
    x11_events_timeout(g_x11_events, &rtv, X11_EVENTS_T_REPEAT,
                       auto_play_timeout, p);
    p->curr_ival = p->ival;
  }
}

static void auto_play(void) {
  int x, y;
  unsigned int bw, d;
  Window root;
  Player *p = malloc(sizeof(*p));
  if (!p)
    return;

  if (mov_ind == 0)
    return;

  p->frame = 0;
  p->ncycles = 1;
  p->ival = 50;
  p->cycle = 0;
  new_int("Number of cycles", &p->ncycles);
  new_int("Msec between frames", &p->ival);
  if (p->ival < 0)
    p->ival = 0;
  if (p->ncycles <= 0)
    return;
  XGetGeometry(display, draw_win, &root, &x, &y, &p->w, &p->h, &bw, &d);

  if (show_frame(p))
    return;

  struct timeval rtv = {.tv_sec = 0, .tv_usec = p->ival * 1000};
  p->curr_ival = p->ival;
  x11_events_timeout(g_x11_events, &rtv, X11_EVENTS_T_REPEAT, auto_play_timeout,
                     p);
  x11_events_listen(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK,
                    auto_play_event, p);
}

static void auto_play_stop(Player *p) {
  x11_events_unlisten(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK,
                      auto_play_event, p);
  x11_events_timeout_cancel(g_x11_events, auto_play_timeout, p);
  free(p);
}

static void too_small(void) {
  respond_box("Okay", "Window too small for film!");
}
