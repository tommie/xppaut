#ifndef XPPAUT_MANY_POPS_H
#define XPPAUT_MANY_POPS_H

#include <X11/Xlib.h>

/* --- Macros --- */
#define MAXFRZ 26
#define MAXPERPLOT 10
#define MAXPOP 21

/* --- Types --- */
typedef struct {
  Window w,w_info;
  int Use;
  int Restore;
  int Nullrestore;
  int x0;
  int y0;
  int Width;
  int Height;
  int nvars;
  double rm[3][3];
  double min_scale,color_scale;
  double xmin,ymin,zmin,xmax,ymax,zmax,xorg,yorg,zorg;
  double xbar,ybar,zbar,dx,dy,dz;
  int xv[MAXPERPLOT],yv[MAXPERPLOT],zv[MAXPERPLOT];
  int line[MAXPERPLOT],color[MAXPERPLOT];
  double Theta,Phi;
  double ZPlane,ZView;
  double xlo,ylo,xhi,yhi,oldxlo,oldxhi,oldylo,oldyhi;
  int grtype,ThreeDFlag,TimeFlag,PerspFlag;
  int xshft,yshft,zshft;
  int xorgflag,yorgflag,zorgflag;
  int ColorFlag,ColorValue;
  char xlabel[30],ylabel[30],zlabel[30];
  char gr_info[256];
} GRAPH;

typedef struct {
  Window w;
  char key[20],name[10];
  short use,type;
  float *xv,*yv,*zv;
  int len,color;
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
int select_table(void);
void get_intern_set(void);
void make_icon(char *icon, int wid, int hgt, Window w);
void title_text(char *string);
void gtitle_text(char *string, Window win);
void restore_off(void);
void restore_on(void);
void add_label(char *s, int x, int y, int size, int font);
void draw_marker(double x, double y, double size, int type);
void draw_grob(int i);
void arrow_head(double xs, double ys, double xe, double ye, double size);
void destroy_grob(Window w);
void destroy_label(Window w);
void draw_label(Window w);
void add_grob(double xs, double ys, double xe, double ye, double size, int type, int color);
int select_marker_type(int *type);
int man_xy(float *xe, float *ye);
int get_marker_info(void);
int get_markers_info(void);
void add_marker(void);
void add_marker_old(void);
void add_markers(void);
void add_markers_old(void);
void add_pntarr(int type);
void edit_object_com(int com);
void do_gr_objs_com(int com);
void do_windows_com(int c);
void set_restore(int flag);
int is_col_plotted(int nc);
void destroy_a_pop(void);
void init_grafs(int x, int y, int w, int h);
void ps_restore(void);
void svg_restore(void);
int rotate3dcheck(XEvent ev);
void do_motion_events(XEvent ev);
void do_expose(XEvent ev);
void resize_all_pops(int wid, int hgt);
void kill_all_pops(void);
void create_a_pop(void);
void GrCol(void);
void BaseCol(void);
void SmallGr(void);
void SmallBase(void);
void change_plot_vars(int k);
int check_active_plot(int k);
int graph_used(int i);
void make_active(int i);
void select_window(Window w);
void set_gr_fore(void);
void set_gr_back(void);
void hi_lite(Window wi);
void lo_lite(Window wi);
void select_sym(Window w);
void canvas_xy(char *buf);
void check_draw_button(XEvent ev);
void set_active_windows();

#endif /* XPPAUT_MANY_POPS_H */
