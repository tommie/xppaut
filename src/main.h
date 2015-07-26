#ifndef XPPAUT_MAIN_H
#define XPPAUT_MAIN_H

#include <X11/Xlib.h>
#include "load_eqn.h"

/* --- Macros --- */
/* FixWindowSize(flag) values */
#define FIX_MAX_SIZE 1
#define FIX_MIN_SIZE 2
#define FIX_SIZE 3

/* --- Data --- */
extern int allwinvis;
extern char anifile[XPP_MAX_NAME];
extern int batch_range;
extern char batchout[256];
extern int CURY_OFF;
extern int CURY_OFFb;
extern int CURY_OFFs;
extern int DCURX;
extern int DCURXs;
extern int DCURY;
extern int DCURYb;
extern int DCURYs;
extern int DisplayHeight;
extern int DisplayWidth;
extern int DoTutorial;
extern XFontStruct *big_font;
extern XFontStruct *small_font;
extern char big_font_name[100];
extern char small_font_name[100];
extern unsigned int GrFore;
extern unsigned int GrBack;
extern FILE *logfile;
extern unsigned int MyBackColor;
extern unsigned int MyForeColor;
extern unsigned int MyDrawWinColor;
extern unsigned int MyMainWinColor;
extern OptionsSet notAlreadySet;
extern int OVERRIDE_LOGFILE;
extern int OVERRIDE_QUIET;
extern int PaperWhite;
extern char PlotFormat[100];
extern int SCALEX;
extern int SCALEY;
extern char SLIDER1VAR[20];
extern char SLIDER2VAR[20];
extern char SLIDER3VAR[20];
extern double SLIDER1LO;
extern double SLIDER2LO;
extern double SLIDER3LO;
extern double SLIDER1HI;
extern double SLIDER2HI;
extern double SLIDER3HI;
extern int tfBell;
extern int TipsFlag;
extern int TrueColorFlag;
extern int use_ani_file;
extern int use_intern_sets;
extern char UserBGBitmap[XPP_MAX_NAME];
extern char UserBlack[8];
extern char UserDrawWinColor[8];
extern int UserGradients;
extern char UserMainWinColor[8];
extern int UserMinHeight;
extern int UserMinWidth;
extern char UserOUTFILE[256];
extern char UserWhite[8];
extern int XPPBatch;
extern int XPPVERBOSE;
extern float xppvermaj;
extern float xppvermin;
extern int Xup;

extern Atom deleteWindowAtom;
extern Display *display;
extern GC gc;
extern GC font_gc;
extern GC gc_graph;
extern GC small_gc;
extern int screen;
extern Window command_pop;
extern Window draw_win;
extern Window main_win;
extern Window info_pop;

/* --- Functions --- */
void do_main(int argc, char **argv);
void check_for_quiet(int argc, char **argv);
void do_vis_env(void);
void init_X(void);
void set_big_font(void);
void set_small_font(void);
void xpp_events(XEvent report, int min_wid, int min_hgt);
void do_events(unsigned int min_wid, unsigned int min_hgt);
void bye_bye(void);
void clr_scrn(void);
void redraw_all(void);
void commander(int ch);
Window init_win(unsigned int bw, char *icon_name, char *win_name, int x, int y, unsigned int min_wid, unsigned int min_hgt, int argc, char **argv);
void top_button_draw(Window w);
void top_button_cross(Window w, int b);
void top_button_press(Window w);
void top_button_events(XEvent report);
void make_top_buttons(void);
void getGC(GC *gc);
void load_fonts(void);
void make_pops(void);
void FixWindowSize(Window w, int width, int height, int flag);
int getxcolors(XWindowAttributes *win_info, XColor **colors);
void test_color_info(void);

#endif /* XPPAUT_MAIN_H */
