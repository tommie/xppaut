#ifndef XPPAUT_MAIN_H
#define XPPAUT_MAIN_H

#include <X11/Xlib.h>
#include "xpplim.h"

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
void FixWindowSize(Window w, int width, int height, int flag);
void bye_bye(void);
void clr_scrn(void);
void commander(int ch);
void do_main(int argc, char **argv);
void redraw_all(void);
void top_button_draw(Window w);

#endif /* XPPAUT_MAIN_H */
