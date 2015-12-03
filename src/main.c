/*
    Copyright (C) 2002-2011  Bard Ermentrout & Daniel Dougherty

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be contacted at
     bard@pitt.edu
*/
#include "main.h"

#include <stdlib.h>
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>

#include "adj2.h"
#include "aniparse.h"
#include "arrayplot.h"
#include "auto_nox.h"
#include "auto_x11.h"
#include "axes2.h"
#include "browse.h"
#include "calc.h"
#include "color.h"
#include "comline.h"
#include "dae_fun.h"
#include "do_fit.h"
#include "edit_rhs.h"
#include "eig_list.h"
#include "extra.h"
#include "form_ode.h"
#include "ggets.h"
#include "graf_par.h"
#include "graphics.h"
#include "init_conds.h"
#include "integrate.h"
#include "load_eqn.h"
#include "lunch-new.h"
#include "many_pops.h"
#include "menu.h"
#include "menudrive.h"
#include "my_rhs.h"
#include "myfonts.h"
#include "nullcline.h"
#include "numerics.h"
#include "pop_list.h"
#include "read_dir.h"
#include "simplenet.h"
#include "solver.h"
#include "storage.h"
#include "txtread.h"
#include "userbut.h"
#include "bitmap/pp.bitmap"
#include "ui-x11/window.h"

/* --- Macros --- */
#define lowbit(x) ((x) & (~(x) + 1))

#define TOPBUTTONCOLOR 27

#define TOO_SMALL 0
#define BIG_ENOUGH 1

#define cstringmaj MYSTR1
#define cstringmin MYSTR2

/* --- Forward Declarations --- */
static void do_events(unsigned int min_wid, unsigned int min_hgt);
static void getGC(GC *gc);
static int getxcolors(XWindowAttributes *win_info, XColor **colors);
static void init_X(void);
static Window init_win(unsigned int bw, char *icon_name, char *win_name, int x,
                       int y, unsigned int min_wid, unsigned int min_hgt,
                       int argc, char **argv);
static void load_fonts(void);
static void make_pops(void);
static void make_top_buttons(void);
static void set_big_font(void);
static void test_color_info(void);
static void top_button_cross(Window w, int b);
static void top_button_events(XEvent report);
static void top_button_press(Window w);

/* --- Data --- */
int allwinvis = 0;
int use_ani_file = 0;
char anifile[XPP_MAX_NAME];

float xppvermaj, xppvermin;

int Xup, TipsFlag = 1;
Atom deleteWindowAtom = 0;
int XPPBatch = 0, batch_range = 0;
char batchout[256];
char UserOUTFILE[256];

int DisplayHeight, DisplayWidth;
int TrueColorFlag;
char big_font_name[100], small_font_name[100];
char PlotFormat[100];

int PaperWhite = -1;

Window draw_win;
Window main_win;
Window command_pop;
X11StatusBar *main_status_bar;
GC gc, gc_graph, small_gc, font_gc;
char UserBlack[8];
char UserWhite[8];
char UserMainWinColor[8];
char UserDrawWinColor[8];
char UserBGBitmap[XPP_MAX_NAME];

int UserGradients = -1;
int UserMinWidth = 0, UserMinHeight = 0;
unsigned int MyBackColor, MyForeColor, MyMainWinColor, MyDrawWinColor;
unsigned int GrFore, GrBack;
int SCALEX, SCALEY;
Display *display;
int screen;
X11Events *g_x11_events;
int DCURYb, DCURXb, CURY_OFFb;
int DCURYs, DCURXs, CURY_OFFs;
int DCURY, DCURX, CURY_OFF;
FILE *logfile;
int XPPVERBOSE = 1;
int tfBell;

/* Set this to 1 if you want the tutorial to come up at start-up
   as default behavior
 */
int DoTutorial = 0;

XFontStruct *big_font, *small_font;

static Window TopButton[6];
static unsigned int Black, White;
static int ALREADY_SWAPPED = 0;

int main(int argc, char **argv) {
  char myfile[XPP_MAX_NAME];
  char pptitle[80];
  unsigned int min_wid = 450, min_hgt = 360;

  get_directory(myfile);

  SCALEX = 640;
  SCALEY = 480;

  Xup = 0;
  sprintf(batchout, "output.dat");
  sprintf(PlotFormat, "ps");

  logfile = stdout;

  loadeqn_init_options();
  loadeqn_load_xpprc();
  do_comline(argc, argv);

  if (!XPPBatch) {
    init_X();
  }

  // Load the ODE file (possibly by showing a file selector).
  form_ode_load();

  // Let command line options override ODE file options.
  do_comline(argc, argv);

  // Set up subsystems based on options.
  init_alloc_info();
  loadeqn_setup_all();
  set_init_guess();
  update_all_ffts();

#ifdef AUTO
  init_auto_win();
#endif

  if (disc(this_file))
    solver_set_method(METHOD_DISCRETE);
  xppvermaj = (float)cstringmaj;
  xppvermin = (float)cstringmin;
  if (strlen(this_file) < 60)
    sprintf(pptitle, "XPP Ver %g.%g >> %s", xppvermaj, xppvermin, this_file);
  else
    sprintf(pptitle, "XPP Version %g.%g", xppvermaj, xppvermin);

  set_delay();
  init_fit_info();
  strip_saveqn();
  create_plot_list();
  auto_load_dll();

  if (XPPBatch) {
    MakeColormap();
    init_browser();
    init_all_graph();
    if_needed_load_set();
    if_needed_load_par();
    if_needed_load_ic();
    if_needed_select_sets();
    set_extra_graphs();
    set_colorization_stuff();

    batch_integrate();
    return 0;
  }

  gtitle_text(pptitle, main_win);
  Xup = 1;
  MakeColormap();

  XMapWindow(display, main_win);

  make_pops();

  make_top_buttons();

  initialize_box();

  init_browser();
  if (allwinvis == 1) {
    make_new_ic_box();
    make_new_bc_box();
    make_new_delay_box();
    make_new_param_box();
    make_new_browser();
    create_eq_list();
  }

  Xup = 1;
  ani_zero();
  set_extra_graphs();
  set_colorization_stuff();

  make_scrbox_lists();

  /*   This is for testing widgets  ---    */

  /*          MAIN LOOP             */
  test_color_info();
  if_needed_load_set();
  if_needed_load_par();
  if_needed_load_ic();

  if (use_ani_file) {
    new_vcr();
    get_ani_file(anifile);
  }

  if (DoTutorial == 1) {
    do_tutorial();
  }

  default_window();

  do_events(min_wid, min_hgt);

  return 0;
}

static void init_X(void) {
  char *icon_name = "xpp";
  char *win_name = "XPPAUT";
  unsigned int x = 0, y = 0;
  unsigned int min_wid = 450, min_hgt = 360;
  char *getenv();

  char teststr[] = "The Quick Brown Fox Jumped Over The Lazy Dog?";

  if (UserMinWidth > 0) {
    min_wid = UserMinWidth;
    SCALEX = min_wid;
  }

  if (UserMinHeight > 0) {
    min_hgt = UserMinHeight;
    SCALEY = min_hgt;
  }

  if (PaperWhite == 0) {
    GrFore = White;
    GrBack = Black;
  }

  main_win = init_win(4, icon_name, win_name, x, y, min_wid, min_hgt, 0, NULL);

  /*Set up foreground and background colors*/

  Black = BlackPixel(display, screen);
  White = WhitePixel(display, screen);

  if (strlen(UserBlack) != 0) {
    XColor user_col;

    XParseColor(display, DefaultColormap(display, screen), UserBlack,
                &user_col);
    XAllocColor(display, DefaultColormap(display, screen), &user_col);

    MyForeColor = GrFore = user_col.pixel;
    Black = MyForeColor;
  }

  if (strlen(UserWhite) != 0) {
    XColor user_col;

    XParseColor(display, DefaultColormap(display, screen), UserWhite,
                &user_col);
    XAllocColor(display, DefaultColormap(display, screen), &user_col);

    MyBackColor = GrBack = user_col.pixel;
    White = MyBackColor;
  }

  /*  Switch for reversed video  */
  MyForeColor = GrFore = Black;
  MyBackColor = GrBack = White;
  /*  This is reversed
   MyForeColor=White;
   MyBackColor=Black; */

  if (PaperWhite == 1) /*Respect the swapping implied by the -white option.*/
  {
    printf("Doing swap!\n");
    char swapcol[8];
    strcpy(swapcol, UserWhite);
    strcpy(UserWhite, UserBlack);
    strcpy(UserBlack, swapcol);

    MyForeColor = GrFore = White;
    MyBackColor = GrBack = Black;

    ALREADY_SWAPPED = 1;
  }

  if (strlen(UserMainWinColor) != 0) {
    XColor main_win_col;

    XParseColor(display, DefaultColormap(display, screen), UserMainWinColor,
                &main_win_col);
    XAllocColor(display, DefaultColormap(display, screen), &main_win_col);

    MyMainWinColor = main_win_col.pixel;
  } else {
    MyMainWinColor = MyBackColor;
  }

  XSetWindowBorder(display, main_win, MyForeColor);
  XSetWindowBackground(display, main_win, MyMainWinColor);

  if (strlen(UserDrawWinColor) != 0) {
    XColor draw_win_col;
    XParseColor(display, DefaultColormap(display, screen), UserDrawWinColor,
                &draw_win_col);
    XAllocColor(display, DefaultColormap(display, screen), &draw_win_col);

    MyDrawWinColor = draw_win_col.pixel;
  } else {
    MyDrawWinColor = MyBackColor;
  }

  /* win = main_win; */
  FixWindowSize(main_win, SCALEX, SCALEY, FIX_MIN_SIZE);
  periodic = 1;
  if (DefaultDepth(display, screen) >= 8) {
    COLOR = 1;
  } else {
    COLOR = 0;
  }
  /* if(DefaultDepth(display,screen)==4){
     map16();
     COLOR=1;
     } */

  XSelectInput(display, main_win, ExposureMask | KeyPressMask |
                                      ButtonPressMask | StructureNotifyMask |
                                      ButtonReleaseMask | ButtonMotionMask);

  load_fonts();

  /*BETTER SUPPORT FOR VARIABLE WIDTH FONTS
  Use a statistical average to get average spacing. Some fonts don't
  or are not able to report this accurately so this is reliable way to
  get the information. If person only has variable width font on their
  system they can get by.
  The average spacing will be too small for some short strings having
  capital letters (for example "GO"). Thus, we divide by the string
  length of our test string minus 2 for a little more wiggle room.
  */

  /*   DCURXb = XTextWidth (big_font, "#", 1);
   */
  DCURXb =
      XTextWidth(big_font, teststr, strlen(teststr)) / (strlen(teststr) - 2);

  DCURYb = big_font->ascent + big_font->descent;
  CURY_OFFb = big_font->ascent - 1;

  /*  DCURXs = XTextWidth (small_font, "#", 1);
   */

  DCURXs =
      XTextWidth(small_font, teststr, strlen(teststr)) / (strlen(teststr) - 2);

  DCURYs = small_font->ascent + small_font->descent;
  CURY_OFFs = small_font->ascent - 1;

  getGC(&gc);
  getGC(&gc_graph);
  getGC(&small_gc);
  getGC(&font_gc);

  if (strlen(UserBGBitmap) != 0) /*User supplied */
  {
    /*Pixmap
     * pmap=XCreatePixmapFromBitmapData(display,main_win,lines_bits,lines_width,lines_height,MyForeColor,MyBackColor,DefaultDepth(display,
     * DefaultScreen(display)));
   */
    unsigned int width_return, height_return;
    int x_hot, y_hot;
    unsigned char *pixdata;

    int success = XReadBitmapFileData(UserBGBitmap, &width_return,
                                      &height_return, &pixdata, &x_hot, &y_hot);

    if (success != BitmapSuccess) {
      if (success == BitmapOpenFailed) {
        plintf("Problem reading bitmap file %s -> BitmapOpenFailed\n",
               UserBGBitmap);
      } else if (success == BitmapFileInvalid) {
        plintf("Problem reading bitmap file %s -> BitmapFileInvalid\n",
               UserBGBitmap);
      } else if (success == BitmapNoMemory) {
        plintf("Problem reading bitmap file %s -> BitmapNoMemory\n",
               UserBGBitmap);
      }
    } else {
      Pixmap pmap = XCreatePixmapFromBitmapData(
          display, main_win, (char *)pixdata, width_return, height_return,
          MyForeColor, MyMainWinColor,
          DefaultDepth(display, DefaultScreen(display)));
      XSetWindowBackgroundPixmap(display, main_win, pmap);
      XFreePixmap(display, pmap);
      XFree(pixdata);
    }
  }

  if (COLOR)
    MakeColormap();

  set_big_font();

  XSetFont(display, small_gc, small_font->fid);
  /*make_pops();*/

  /*
  If the user didn't specify specifically heights and widths
  we try to set the initial size to fit everything nicely especially
  if they are using wacky fonts...
  */
  if (UserMinWidth <= 0) {
    SCALEX = 10 + 36 * 2 * DCURXs + 32 * DCURXs;
  }

  if (UserMinHeight <= 0) {
    SCALEY = 25 * DCURYb + 7 * DCURYs;
  }

  XResizeWindow(display, main_win, SCALEX, SCALEY);
  /*FixWindowSize (main_win, SCALEX, SCALEY, FIX_MIN_SIZE);
  */
}

static void set_big_font(void) {
  DCURX = DCURXb;
  DCURY = DCURYb;
  CURY_OFF = CURY_OFFb;
  XSetFont(display, gc, big_font->fid);
}

static void xpp_events(void *cookie, const XEvent *ev) {
  int *minsize = cookie;
  int com;
  int used = 0;

  /*  put_command("Command:");  */

  do_array_plot_events(*ev);
  txt_view_events(*ev);
  do_ani_events(*ev);
  top_button_events(*ev);
  if (ev->type != Expose)
    main_menu_event(ev);
  switch (ev->type) {
  /* case ClientMessage:
           if(report.xclient.data.l[0]==deleteWindowAtom){

           break;
           }
           break; */
  case Expose:
  case MapNotify:

    if (ev->xany.window == command_pop)
      put_command("Command:");
    do_expose(*ev);

    break;
  case ConfigureNotify:
    resize_par_box(ev->xany.window);
    resize_eq_list(ev->xany.window);
    resize_auto_window(*ev);
    if (ev->xconfigure.window == main_win) {

      SCALEX = ev->xconfigure.width;
      SCALEY = ev->xconfigure.height;
      if (SCALEX < minsize[0] || SCALEY < minsize[1]) {
        /*window_size=TOO_SMALL;*/
        SCALEX = minsize[0];
        SCALEY = minsize[0];
      } else {
        /*window_size=BIG_ENOUGH;*/
        XResizeWindow(display, command_pop, SCALEX - 4, DCURY + 1);
        x11_status_bar_set_extents(main_status_bar, 0, SCALEY - DCURY - 4,
                                   SCALEX - 4, DCURY);
        resize_par_slides(SCALEY - 3 * DCURYs - 1 * DCURYb - 13);
        resize_all_pops(SCALEX, SCALEY);
        redraw_all();
      }
    }

    break;

  case KeyPress:
    used = 0;
    box_keypress(*ev, &used);
    if (used)
      break;
    eq_list_keypress(*ev, &used);
    if (used)
      break;
#ifdef AUTO
    auto_keypress(*ev, &used);
    if (used)
      break;
#endif
    break;

  case KeyRelease:
    /* Use KeyRelease to avoid KeyRelease ending up seomewhere else when
     * popping up new windows.
     */
    if (ev->xkey.window == main_win || ev->xkey.window == draw_win)
      commander(get_key_press(ev));

    break;
  case EnterNotify:
    enter_eq_stuff(ev->xcrossing.window, 2);
    enter_slides(ev->xcrossing.window, 1);
    box_enter_events(ev->xcrossing.window, 1);
#ifdef AUTO
    auto_enter(ev->xcrossing.window, 2);
#endif
    break;
  case LeaveNotify:
    enter_eq_stuff(ev->xcrossing.window, 1);
    enter_slides(ev->xcrossing.window, 0);
    box_enter_events(ev->xcrossing.window, 0);
#ifdef AUTO
    auto_enter(ev->xcrossing.window, 1);
#endif
    break;
  case MotionNotify:
    do_motion_events(*ev);
    break;
  case ButtonRelease:
    slide_release(ev->xbutton.window);

    break;
  case ButtonPress:
    /* check_box_cursor(); */
    if (!rotate3dcheck(*ev)) {
      /* box_select_events(ev->xbutton.window,&i1); */
      box_buttons(ev->xbutton.window);

      slide_button_press(ev->xbutton.window);
      eq_list_button(*ev);
#ifdef AUTO
      auto_button(*ev);
#endif

      show_position(*ev, &com);
    }
    break;

  } /* end switch */
}

static void do_events(unsigned int min_wid, unsigned int min_hgt) {
#define EV_MASK                                                                \
  ExposureMask | StructureNotifyMask | SubstructureNotifyMask | KeyPressMask | \
      EnterWindowMask | LeaveWindowMask | ButtonMotionMask |                   \
      PointerMotionMask | ButtonReleaseMask | ButtonPressMask

  blank_screen(main_win);
  if (RunImmediately == 1) {
    run_the_commands(M_IG);
    RunImmediately = 0;
  }

  int minsize[2];

  minsize[0] = min_wid;
  minsize[1] = min_hgt;
  x11_events_listen(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK, xpp_events,
                    minsize);

  x11_events_run(g_x11_events);

  x11_events_unlisten(g_x11_events, X11_EVENTS_ANY_WINDOW, EV_MASK, xpp_events,
                      minsize);
#undef EV_MASK
}

void bye_bye(void) {
  int i;
  yes_reset_auto();
  XUnloadFont(display, big_font->fid);
  XUnloadFont(display, small_font->fid);
  for (i = 0; i < 5; i++) {
    if (avsymfonts[i])
      XUnloadFont(display, symfonts[i]->fid);
    if (avromfonts[i])
      XUnloadFont(display, romfonts[i]->fid);
  }
  XFreeGC(display, gc);
  XCloseDisplay(display);
  exit(1);
}

void clr_scrn(void) {
  blank_screen(draw_win);
  restore_off();
  do_axes();
}

void redraw_all(void) {
  redraw_dfield();
  restore(0, my_browser.maxrow);
  draw_label(draw_win);
  draw_freeze(draw_win);
  restore_on();
}

void commander(int ch) {
  switch (ch) {
  case 'i':
    /*  initial data  */
    ini_data_menu();
    break;
  case 'c':
    /* continue */
    cont_integ();
    break;
  case 'n':
    /*nullclines */
    /* test_color_info(); */
    new_clines();
    break;
  case 'd':
    /*dir fields */
    direct_field();
    break;
  case 'w':
    /* window */
    window_zoom();
    break;
  case 'a':
    /*phase-space */
    do_torus();
    break;
  case 'k':
    /*kinescope */
    do_movie();
    break;
  case 'g':
    add_a_curve();
    break;
  case 'u':
    do_numerics_pop_up();
    break;
  case 'f':
    do_file_pop_up();
    break;
  case 'p':
    /*parameters */
    /* change_par(-1); */
    new_param();
    break;
  case 'e':
    /*erase */
    clear_screens();
    break;
  case 'h':
  case 'm':
    do_windows();
    /*half windows */
    break;
  case 't':
    /*text */
    do_gr_objs();
    break;
  case 's':
    /*sing pts */
    find_equilibrium();
    break;
  case 'v':
    /*view_axes */
    change_view();
    break;
  case 'b':
    find_bvp();
    break;

  case 'x':
    /*x vs t */
    x_vs_t();
    break;
  case 'r':
    /*restore*/
    redraw_them_all();
    break;
  case '3':
    get_3d_par();
    break;
  case 'y':
    /*  test_test(); */
    break;
  }
}

static Window init_win(unsigned int bw, char *icon_name, char *win_name, int x,
                       int y, unsigned int min_wid, unsigned int min_hgt,
                       int argc, char **argv) {
  /*  XSetWindowAttributes xswa;
   XWindowAttributes xwa;
    */
  Window wine;
  int count;
  unsigned dp_h, dp_w;
  Pixmap icon_map;
  XIconSize *size_list;
  XSizeHints size_hints;
  char *display_name = NULL;
  if ((display = XOpenDisplay(display_name)) == NULL) {
    plintf(" Failed to open X-Display \n");
    exit(-1);
  }
  g_x11_events = x11_events_alloc(display);
  /*   Remove after debugging is done */
  /* XSynchronize(display,1); */
  screen = DefaultScreen(display);
  if (!deleteWindowAtom) {
    deleteWindowAtom = XInternAtom(display, "WM_DELETE_WINDOW", 0);
  }
  dp_w = DisplayWidth(display, screen);
  dp_h = DisplayHeight(display, screen);
  DisplayWidth = dp_w;
  DisplayHeight = dp_h;
  if (SCALEX > dp_w)
    SCALEX = dp_w;
  if (SCALEY > dp_h)
    SCALEY = dp_h;
  wine = XCreateSimpleWindow(display, RootWindow(display, screen), x, y, SCALEX,
                             SCALEY, bw, MyForeColor, MyBackColor);
  /*  xswa.override_redirect=1;
   XChangeWindowAttributes(display,wine,CWOverrideRedirect,&xswa); */
  XGetIconSizes(display, RootWindow(display, screen), &size_list, &count);
  icon_map = XCreateBitmapFromData(display, wine, (char *)pp_bits, pp_width,
                                   pp_height);

  memset(&size_hints, 0, sizeof(size_hints));
#ifdef X11R3
  size_hints.flags = PPosition | PSize | PMinsize;
  size_hints.x = x;
  size_hints.y = y;
  size_hints.width = width;
  size_hints.height = height;
  size_hints.min_width = min_wid;
  size_hints.min_height = min_hgt;
#else

  size_hints.flags = PPosition | PSize | PMinSize;
  size_hints.min_width = min_wid;
  size_hints.min_height = min_hgt;
#endif

#ifdef X11R3
  XSetStandardProperties(display, wine, win_name, icon_name, icon_map, argv,
                         argc, &size_hints);
#else
  {
    XWMHints wm_hints;
    XClassHint class_hints;
    XTextProperty winname, iconname;
    if (XStringListToTextProperty(&icon_name, 1, &iconname) == 0) {
      plintf("X error: failure for iconname\n");
      exit(-1);
    }
    if (XStringListToTextProperty(&win_name, 1, &winname) == 0) {
      plintf("X error: failure for winname\n");
      exit(-1);
    }

    wm_hints.initial_state = NormalState;
    wm_hints.input = True;
    wm_hints.icon_pixmap = icon_map;
    wm_hints.flags = StateHint | IconPixmapHint | InputHint;
    class_hints.res_name = "base";
    class_hints.res_class = win_name;

    XSetWMProperties(display, wine, &winname, &iconname, argv, argc,
                     &size_hints, &wm_hints, &class_hints);
    XSetWMProtocols(display, wine, &deleteWindowAtom, 1);

    XFree(winname.value);
    XFree(iconname.value);
  }
#endif
  return (wine);
}

void top_button_draw(Window w) {
  if (w == TopButton[0])
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "ICs  ", 5);
  if (w == TopButton[1])
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "BCs  ", 5);
  if (w == TopButton[2])
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "Delay", 5);
  if (w == TopButton[3])
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "Param", 5);
  if (w == TopButton[4])
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "Eqns ", 5);
  if (w == TopButton[5])
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "Data ", 5);
}

static void top_button_cross(Window w, int b) {
  int i;
  for (i = 0; i < 6; i++)
    if (w == TopButton[i]) {
      XSetWindowBorderWidth(display, w, b);
      return;
    }
}

static void top_button_press(Window w) {
  if (w == TopButton[0]) {
    make_new_ic_box();
  }
  if (w == TopButton[1]) {
    make_new_bc_box();
  }
  if (w == TopButton[2]) {
    make_new_delay_box();
  }
  if (w == TopButton[3]) {
    make_new_param_box();
  }
  if (w == TopButton[4]) {
    create_eq_list();
  }
  if (w == TopButton[5]) {
    make_new_browser();
  }
}

static void top_button_events(XEvent report) {
  switch (report.type) {
  case Expose:
  case MapNotify:
    top_button_draw(report.xany.window);
    break;
  case EnterNotify:
    top_button_cross(report.xcrossing.window, 2);
    break;
  case LeaveNotify:
    top_button_cross(report.xcrossing.window, 1);
    break;
  case ButtonPress:
    top_button_press(report.xbutton.window);
    break;
  }
  user_button_events(report);
}

static void make_top_buttons(void) {
  int x1 = 2, x2 = 6 * DCURXs + 5, dx = DCURXs;
  TopButton[0] = make_window(main_win, x1, 1, x2, DCURYs, 1);
  x1 = x1 + x2 + dx;
  TopButton[1] = make_window(main_win, x1, 1, x2, DCURYs, 1);
  x1 = x1 + x2 + dx;

  TopButton[2] = make_window(main_win, x1, 1, x2, DCURYs, 1);
  x1 = x1 + x2 + dx;

  TopButton[3] = make_window(main_win, x1, 1, x2, DCURYs, 1);
  x1 = x1 + x2 + dx;

  TopButton[4] = make_window(main_win, x1, 1, x2, DCURYs, 1);
  x1 = x1 + x2 + dx;

  TopButton[5] = make_window(main_win, x1, 1, x2, DCURYs, 1);
  x1 = x1 + x2 + dx;
  create_user_buttons(x1, 1, main_win);
}

void getGC(GC *gc) {
  unsigned int valuemask = 0;
  XGCValues values;
  *gc = XCreateGC(display, main_win, valuemask, &values);
  XSetForeground(display, *gc, MyForeColor);
}

void load_fonts(void) {
  int i;
  /*printf("\n\nFONTS %s %s \n",big_font_name,small_font_name);
   */
  if ((big_font = XLoadQueryFont(display, big_font_name)) == NULL) {
    plintf("X Error: Failed to load big font: %s\n", big_font_name);
    exit(-1);
  }

  if ((small_font = XLoadQueryFont(display, small_font_name)) == NULL) {
    plintf("X Error: Failed to load small font: %s\n", small_font_name);
    exit(-1);
  }

  for (i = 0; i < 5; i++) {
    if ((symfonts[i] = XLoadQueryFont(display, symbolfonts[i])) == NULL) {
      if (i == 0 || i == 1)
        symfonts[i] = small_font;
      else
        symfonts[i] = big_font;
      avsymfonts[i] = 1;
    } else {
      avsymfonts[i] = 1;
      plintf(" sym %d loaded ..", i);
    }

    if ((romfonts[i] = XLoadQueryFont(display, timesfonts[i])) == NULL) {
      if (i == 0 || i == 1)
        romfonts[i] = small_font;
      else
        romfonts[i] = big_font;
      avromfonts[i] = 1;
    } else {
      avromfonts[i] = 1;
      plintf(" times %d loaded ..", i);
    }
  }
  plintf("\n");
}

static void make_pops(void) {
  int x, y;
  unsigned int h, w, bw, d;
  Window wn;
  int var_ind[3] = {
      !strlen(SLIDERVAR[0]) ? -1 : 0, !strlen(SLIDERVAR[1]) ? -1 : 1,
      !strlen(SLIDERVAR[2]) ? -1 : 2,
  };

  XGetGeometry(display, main_win, &wn, &x, &y, &w, &h, &bw, &d);
  main_status_bar =
      x11_status_bar_alloc(main_win, 0, h - DCURY - 4, w - 2, DCURY);
  main_menu_create(main_win);
  command_pop = XCreateSimpleWindow(display, main_win, 0, DCURYs + 4, w - 2,
                                    DCURY + 4, 2, MyForeColor, MyBackColor);
  if (!main_status_bar)
    exit(1);
  XCreateFontCursor(display, XC_hand2);
  XSelectInput(display, command_pop,
               KeyPressMask | ButtonPressMask | ExposureMask);
  XMapWindow(display, command_pop);
  init_grafs(16 * DCURX + 6, DCURYs + DCURYb + 6, w - 16 - 16 * DCURX,
             h - 6 * DCURY - 16);
  create_par_sliders(main_win, 0, h - 5 * DCURY + 8, var_ind);
  get_draw_area();
}

void FixWindowSize(Window w, int width, int height, int flag) {
  XSizeHints size_hints;
  switch (flag) {
  case FIX_SIZE:
    size_hints.flags = PSize | PMinSize | PMaxSize;
    size_hints.width = width;
    size_hints.min_width = width;
    size_hints.max_width = width;
    size_hints.height = height;
    size_hints.min_height = height;
    size_hints.max_height = height;
    break;
  case FIX_MIN_SIZE:
    size_hints.flags = PMinSize;
    size_hints.min_width = width;

    size_hints.min_height = height;

    break;
  case FIX_MAX_SIZE:
    size_hints.flags = PMaxSize;
    size_hints.max_width = width;
    size_hints.max_height = height;
    break;
  }
  XSetWMProperties(display, w, NULL, NULL, NULL, 0, &size_hints, NULL, NULL);
}

static int getxcolors(XWindowAttributes *win_info, XColor **colors) {
  int i, ncolors;

  *colors = (XColor *)NULL;
  TrueColorFlag = 0;
  if (win_info->visual->class == TrueColor) {
    TrueColorFlag = 1;
    plintf("TrueColor visual:  no colormap needed\n");
    return 0;
  }

  else if (!win_info->colormap) {
    plintf("no colormap associated with window\n");
    return 0;
  }

  ncolors = win_info->visual->map_entries;
  plintf("%d entries in colormap\n", ncolors);

  *colors = (XColor *)malloc(sizeof(XColor) * ncolors);

  if (win_info->visual->class == DirectColor) {
    int red, green, blue, red1, green1, blue1;

    plintf("DirectColor visual\n");

    red = green = blue = 0;
    red1 = lowbit(win_info->visual->red_mask);
    green1 = lowbit(win_info->visual->green_mask);
    blue1 = lowbit(win_info->visual->blue_mask);
    for (i = 0; i < ncolors; i++) {
      (*colors)[i].pixel = red | green | blue;
      (*colors)[i].pad = 0;
      red += red1;
      if (red > win_info->visual->red_mask)
        red = 0;
      green += green1;
      if (green > win_info->visual->green_mask)
        green = 0;
      blue += blue1;
      if (blue > win_info->visual->blue_mask)
        blue = 0;
    }
  } else {
    for (i = 0; i < ncolors; i++) {
      (*colors)[i].pixel = i;
      (*colors)[i].pad = 0;
    }
  }

  XQueryColors(display, win_info->colormap, *colors, ncolors);

  return (ncolors);
}

static void test_color_info(void) {
  XColor *colors;
  XWindowAttributes xwa;
  /*int n;
  */
  TrueColorFlag = 0;

  XGetWindowAttributes(display, main_win, &xwa);
  /*n=getxcolors(&xwa,&colors);
  */
  getxcolors(&xwa, &colors);

  if (colors)
    free((char *)colors);
}
