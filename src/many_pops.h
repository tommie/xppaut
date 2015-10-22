#ifndef XPPAUT_MANY_POPS_H
#define XPPAUT_MANY_POPS_H

#include <X11/Xlib.h>

/* --- Macros --- */
#define MAXFRZ 26
#define MAXPERPLOT 10
#define MAXPOP 21

/* --- Types --- */
typedef struct {
  Window w, w_info;
  int Use;
  int Restore;
  int Nullrestore;
  int x0;
  int y0;
  int Width;
  int Height;
  int nvars;
  double rm[3][3];
  double min_scale, color_scale;
  double xmin, ymin, zmin, xmax, ymax, zmax, xorg, yorg, zorg;
  double xbar, ybar, zbar, dx, dy, dz;
  int xv[MAXPERPLOT], yv[MAXPERPLOT], zv[MAXPERPLOT];
  int line[MAXPERPLOT], color[MAXPERPLOT];
  double Theta, Phi;
  double ZPlane, ZView;
  double xlo, ylo, xhi, yhi, oldxlo, oldxhi, oldylo, oldyhi;
  int grtype, ThreeDFlag, TimeFlag, PerspFlag;
  int xshft, yshft, zshft;
  int xorgflag, yorgflag, zorgflag;
  int ColorFlag, ColorValue;
  char xlabel[30], ylabel[30], zlabel[30];
  char gr_info[256];
} GRAPH;

typedef struct {
  Window w;
  char key[20], name[10];
  short use, type;
  float *xv, *yv, *zv;
  int len, color;
} CURVE;

/* --- Data --- */
extern int ActiveWinList[MAXPOP];
extern int current_pop;
extern CURVE frz[MAXFRZ];
extern GRAPH graph[MAXPOP];
extern GRAPH *MyGraph;
extern int num_pops;
extern int SimulPlotFlag;

/* --- Functions --- */
void BaseCol(void);
void GrCol(void);
void SmallBase(void);
void SmallGr(void);
void add_label(char *s, int x, int y, int size, int font);
void canvas_xy(char *buf);
void change_plot_vars(int k);
int check_active_plot(int k);
void check_draw_button(XEvent ev);
void create_a_pop(void);
void do_expose(XEvent ev);
void do_gr_objs_com(int com);
void do_motion_events(XEvent ev);
void do_windows_com(int c);
void draw_label(Window w);
void edit_object_com(int com);
void get_intern_set(void);
void gtitle_text(char *string, Window win);
void hi_lite(Window wi);
void init_grafs(int x, int y, int w, int h);
void make_active(int i);
void ps_restore(void);
void resize_all_pops(int wid, int hgt);
void restore_off(void);
void restore_on(void);
int rotate3dcheck(XEvent ev);
int select_table(void);
void set_active_windows();
void svg_restore(void);
void title_text(char *string);

#endif /* XPPAUT_MANY_POPS_H */
