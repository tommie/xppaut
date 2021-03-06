/* the menu for XPP commands
   this calls any command
   it also has lots of the direct X Gui stuff
*/
#include "menudrive.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <X11/Xlib.h>

#include "adj2.h"
#include "auto_nox.h"
#include "calc.h"
#include "edit_rhs.h"
#include "extra.h"
#include "ggets.h"
#include "graf_par.h"
#include "init_conds.h"
#include "integrate.h"
#include "kinescope.h"
#include "load_eqn.h"
#include "lunch-new.h"
#include "main.h"
#include "many_pops.h"
#include "markov.h"
#include "menus.h"
#include "nullcline.h"
#include "numerics.h"
#include "pop_list.h"
#include "pp_shoot.h"
#include "tabular.h"
#include "torus.h"
#include "tutor.h"
#include "txtread.h"
#include "base/timeutil.h"
#include "bitmap/alert.bitmap"
#include "ui-x11/window.h"

/* --- Macros --- */
#define XPPAUT_ENTRY(k, l, h) { .key = k, .label = l, .hint = h }

/* --- Types --- */
typedef struct {
  Window w;
  char text[256];
  int here;
} MSGBOXSTRUCT;

/* --- Forward Declarations --- */
static void do_file_com(int com);

/* --- Data --- */
static int status;
static MSGBOXSTRUCT MsgBox;

void do_tutorial(void) {
  printf("Running tutorial!\n");
  int tut = 0;
  while (1) {

    char ans =
        (char)two_choice("Next", "Done", tutorial[tut], "nd", DisplayWidth / 2,
                         DisplayHeight / 2, RootWindow(display, screen),
                         "Did you know you can...");

    if (ans == 'd')
      break;
    tut++;
    tut = tut % N_TUTORIAL;
  }
}

void edit_xpprc(void) {
  pid_t child_pid;

  char rc[256];
  char editor[256];
  int child_status;

  char *ed = getenv("XPPEDITOR");

  if ((ed == NULL) || (strlen(ed) == 0)) {
    err_msg("Environment variable XPPEDITOR needs to be set.");

    return;
  } else {
    sprintf(editor, ed);
  }

  child_pid = fork();

  if (child_pid == 0) {
    sprintf(rc, "%s/.xpprc", getenv("HOME"));

    char *const args[] = {editor, rc, NULL};
    execvp(editor, args);
    wait(&child_status);
    return;
  } else {
    if (child_pid == -1) {
      err_msg("Unable to fork process for editor.");
    }

    return;
  }
}

void xpp_hlp(void) {
  char cmd[256];

  if (getenv("XPPHELP") == NULL) {
    err_msg("Environment variable XPPHELP undefined.");
    return;
  }

  if (getenv("XPPBROWSER") == NULL) {
    err_msg("Environment variable XPPBROWSER undefined.");
    return;
  }

  sprintf(cmd, "file:///%s", getenv("XPPHELP"));

  if (fork() == 0) {

    execlp(getenv("XPPBROWSER"), getenv("XPPHELP"), cmd, (char *)0);
    perror("Unable to open browser. Check your XPPBROWSER and XPPHELP "
           "environement variables.");
    exit(1);
  } else {
    wait(&status);
  }
}

void MessageBox(char *m) {
  int wid = strlen(m) * DCURX + 20;
  int hgt = 4 * DCURY;
  MsgBox.w = make_plain_window(RootWindow(display, screen), DisplayWidth / 2,
                               DisplayHeight / 2, wid, hgt, 4);

  XSetTransientForHint(display, MsgBox.w, main_win);
  make_icon((char *)alert_bits, alert_width, alert_height, MsgBox.w);
  MsgBox.here = 1;
  set_window_title(MsgBox.w, "Yo!");
  strcpy(MsgBox.text, m);
  ping();
}

void RedrawMessageBox(Window w) {
  if (w == MsgBox.w) {
    /*    plintf("%s \n",MsgBox.text); */
    Ftext(10, 2 * DCURY, MsgBox.text, MsgBox.w);
  }
}

void KillMessageBox(void) {
  if (MsgBox.here == 0)
    return;
  MsgBox.here = 0;
  waitasec(ClickTime);
  XDestroyWindow(display, MsgBox.w);
}

int TwoChoice(char *c1, char *c2, char *q, char *key) {
  return two_choice(c1, c2, q, key, DisplayWidth / 2, DisplayHeight / 2,
                    RootWindow(display, screen), NULL);
}

int GetMouseXY(int *x, int *y) { return get_mouse_xy(x, y, draw_win); }

void FlushDisplay(void) { XFlush(display); }

void clear_draw_window(void) {
  clr_scrn();
  hi_lite(draw_win);
}

void drw_all_scrns(void) {
  int i;
  int ic = current_pop;
  if (SimulPlotFlag == 0) {
    redraw_all();
    return;
  }

  for (i = 0; i < num_pops; i++) {
    make_active(ActiveWinList[i]);
    redraw_all();
  }

  make_active(ic);
  hi_lite(draw_win);
}

void clr_all_scrns(void) {
  int i;
  int ic = current_pop;
  if (SimulPlotFlag == 0) {
    clr_scrn();
    hi_lite(draw_win);
    return;
  }

  for (i = 0; i < num_pops; i++) {
    make_active(ActiveWinList[i]);
    clr_scrn();
  }

  make_active(ic);
  hi_lite(draw_win);
}

void run_the_commands(int com) {
  /* Sorted in reverse numerical order. */
  if (com >= M_UT)
    do_numerics_com(com);
  else if (com >= M_UKE)
    new_lookup_com(com - M_UKE);
  else if (com >= M_UHN)
    do_stochast_com(com - M_UHN);
  else if (com >= M_UPN)
    get_pmap_pars_com(com - M_UPN);
  else if (com >= M_UCN)
    set_col_par_com(com - M_UCN);
  else if (com >= M_UAN)
    make_adj_com(com - M_UAN);
  /* M_FER missing */
  else if (com >= M_FP)
    do_file_com(com);
  else if (com >= M_BR)
    find_bvp_com(com - M_BR);
  else if (com >= M_V2)
    change_view_com(com - M_V2);
  else if (com >= M_TEM)
    edit_object_com(com - M_TEM);
  else if (com >= M_TT)
    do_gr_objs_com(com - M_TT);
  else if (com >= M_MC)
    do_windows_com(com - M_MC);
  else if (com == M_3)
    get_3d_par_com();
  else if (com == M_X)
    xi_vs_t();
  else if (com == M_R)
    drw_all_scrns();
  else if (com == M_EE) {
    clr_all_scrns();
    DF_FLAG = 0;
  } else if (com == M_P)
    new_parameter();
  else if (com >= M_GCN) {
    change_cmap_com(com - M_GCN);
    redraw_dfield();
  } else if (com >= M_GFKK)
    key_frz_com(com - M_GFKN);
  else if (com >= M_GFF)
    freeze_com(com - M_GFF);
  else if (com >= M_GA)
    add_a_curve_com(com - M_GA);
  else if (com >= M_KC)
    do_movie_com(com - M_KC);
  else if (com >= M_AA)
    do_torus_com(com - M_AA);
  else if (com >= M_WW)
    window_zoom_com(com - M_WW);
  else if (com >= M_DD) {
    direct_field_com(com - M_DD);
    if (com == M_DF)
      return;
    create_new_cline();
    redraw_the_graph();
  } else if (com >= M_SG)
    find_equilib_com(com - M_SG);
  else if (com >= M_NFF)
    froz_cline_stuff_com(com - M_NFF);
  else if (com >= M_NN)
    new_clines_com(com - M_NN);
  else if (com == M_C)
    cont_integ();
  else if (com >= M_IR)
    do_init_data(com);
}

static int index_of_key(const X11MenuDescr *descr, int key) {
  for (int i = 0; i < descr->num_entries; ++i) {
    if (key == descr->entries[i].key)
      return i;
  }

  return -1;
}

static void pop_up_and_run(int base, int y, const X11MenuDescr *descr) {
  int key = pop_up_menu(main_win, 10, y * DCURY + 8, descr,
                        main_status_bar);
  int i = index_of_key(descr, key);

  if (i < 0)
    return;

  run_the_commands(base + i);
}

void do_stochast(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('n', "(N)ew seed", "Seed random number generator"),
    XPPAUT_ENTRY('c', "(C)ompute", "Run many simulations to get average trajectory"),
    XPPAUT_ENTRY('d', "(D)ata", "Get data from last simulation"),
    XPPAUT_ENTRY('m', "(M)ean", "Load average trajectory from many runs"),
    XPPAUT_ENTRY('v', "(V)ariance", "Load variance of from many runs"),
    XPPAUT_ENTRY('h', "(H)istogram", "Compute histogram"),
    XPPAUT_ENTRY('o', "(O)ld hist", "Reload last histogram"),
    XPPAUT_ENTRY('f', "(F)ourier", "Fourier series of trajectory"),
    XPPAUT_ENTRY('p', "(P)ower", "Power spectrum/phase"),
    XPPAUT_ENTRY('i', "F(i)t data", "Fit data from file to trajectory"),
    XPPAUT_ENTRY('s', "(S)tat", "Get mean/variance of single trajectory"),
    XPPAUT_ENTRY('l', "(L)iapunov", "Compute maximal Liapunov exponent"),
    XPPAUT_ENTRY('a', "ST (A)utocor", "Compute spike-time autocorrel"),
    XPPAUT_ENTRY('x', "(X)correl etc", "Compute correlations - subtracting mean"),
    XPPAUT_ENTRY('e', "Sp(e)c.dns", "Compute windowed spectral density"),
    XPPAUT_ENTRY('2', "(2)D-hist", "Compute two-variable histograms"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Stochastic",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_UHN, 2, &MENU_DESCR);
}

void get_pmap_pars(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('n', "(N)one", "Turn off Poincare map"),
    XPPAUT_ENTRY('s', "(S)ection", "Define section for Poincare map"),
    XPPAUT_ENTRY('m', "(M)ax/min", "Compute Poincare map on maximum/minimum of variable"),
    XPPAUT_ENTRY('p', "(P)eriod", "Compute map based on period between events"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Poincare map",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_UPN, 6, &MENU_DESCR);
}

void set_col_par(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('n', "(N)o color", ""),
    XPPAUT_ENTRY('v', "(V)elocity", "Color according to magnitude of derivative"),
    XPPAUT_ENTRY('a', "(A)nother quantity", "Color according to height of Z-axis"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Color code",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_UCN, 12, &MENU_DESCR);
}

void make_adj(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('n', "(N)ew adj", "Compute a new adjoint function"),
    XPPAUT_ENTRY('m', "(M)ake H", "Compute averaging interaction function"),
    XPPAUT_ENTRY('a', "(A)djoint", "Load computed adjoint"),
    XPPAUT_ENTRY('o', "(O)rbit", "Load computed orbit"),
    XPPAUT_ENTRY('h', "(H)fun", "Load computed interaction function"),
    XPPAUT_ENTRY('p', "(P)arameters", "Adjoint numerical parameters"),
    XPPAUT_ENTRY('r', "(R)ange", "Range over stuff to computte many adjoints"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Adjoint",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_UAN, 11, &MENU_DESCR);
}

void do_file_pop_up(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('p', "(P)rt src", "Display source and active comments"),
    XPPAUT_ENTRY('w', "(W)rite set", "Save information for restart"),
    XPPAUT_ENTRY('r', "(R)ead set", "Read information for restart"),
    XPPAUT_ENTRY('a', "(A)uto", "Run AUTO, the bifurcation package"),
    XPPAUT_ENTRY('c', "(C)alculator", "A little calculator -- press ESC to exit"),
    XPPAUT_ENTRY('e', "(E)dit", "Edit right-hand sides or functions or auxiliaries"),
    XPPAUT_ENTRY('s', "(S)ave info", "Save info about simulation in human readable format"),
    XPPAUT_ENTRY('b', "(B)ell toggle", "Turn bell on/off"),
    XPPAUT_ENTRY('h', "(H)elp", "Browser help"),
    XPPAUT_ENTRY('q', "(Q)uit", "Duh!"),
    XPPAUT_ENTRY('t', "(T)ranspose", "Transpose storage"),
    XPPAUT_ENTRY('i', "T(i)ps toggle", "Turn off these silly tips"),
    XPPAUT_ENTRY('g', "(G)et par set", "Set predefined parameters"),
    XPPAUT_ENTRY('l', "C(l)one", "Clone the ode file"),
    XPPAUT_ENTRY('x', ".(X)pprc", "Edit your .xpprc preferences file"),
    XPPAUT_ENTRY('u', "T(u)torial", "Run a quick tutorial on XPPAUT"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "File",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_FP, 10, &MENU_DESCR);
}

static void do_file_com(int com) {
  switch (com) {
  case M_FT:
    do_transpose();
    break;
  case M_FG:
    get_intern_set();
    break;
  case M_FI:
    TipsFlag = 1 - TipsFlag;
    break;
  case M_FP:
    make_txtview();
    break;
  case M_FW:
    do_lunch(WRITEM);
    break;
  case M_FS:
    file_inf();
    break;
  case M_FA:
#ifdef AUTO
    do_auto_win();
#endif
    break;
  case M_FC:
    q_calc();
    break;
  case M_FE:
    edit_menu();
    break;
  case M_FR:
    do_lunch(READEM);
    break;
  case M_FB:
    tfBell = 1 - tfBell;
    break;
  case M_FH:
    xpp_hlp();
    break;
  case M_FX:
    edit_xpprc();
    break;
  case M_FU:
    do_tutorial();
    break;
  case M_FQ:
    if (yes_no_box())
      bye_bye();
    break;
  case M_FL:
    clone_ode();
    break;
  }
}

void do_numerics_pop_up(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('t', "(T)otal", "Total time to integrate eqns"),
    XPPAUT_ENTRY('s', "(S)tart time", "Starting time -- T0"),
    XPPAUT_ENTRY('r', "T(r)ansient", "Time to integrate before storing"),
    XPPAUT_ENTRY('d', "(D)t", "Time step to use"),
    XPPAUT_ENTRY('n', "(N)cline ctrl", "Mesh for nullclines"),
    XPPAUT_ENTRY('i', "S(i)ng pt ctrl", "Numerical parameters for fixed points"),
    XPPAUT_ENTRY('o', "# (O)utput", "Number of steps per plotted point"),
    XPPAUT_ENTRY('b', "(B)ounds", "Maximum allowed size of any variable"),
    XPPAUT_ENTRY('m', "(M)ethod", "Integration method"),
    XPPAUT_ENTRY('e', "D(e)lay", "Maximum delay and delay related stuff"),
    XPPAUT_ENTRY('c', "(C)olor code", "Color trajectories according to velocity,etc"),
    XPPAUT_ENTRY('h', "Stoc(h)astic", "Curve fitting, FFT, mean, variance, seed, etc"),
    XPPAUT_ENTRY('p', "(P)oincare ma", "Define Poincare map parameters"),
    XPPAUT_ENTRY('u', "R(u)elle plot", "Define shifted plots"),
    XPPAUT_ENTRY('k', "Loo(k)up", "Modify lookup tables"),
    XPPAUT_ENTRY('v', "Bnd(v)al", "Numerical setup for boundary value solver"),
    XPPAUT_ENTRY('a', "(A)veraging", "Compute adjoint and averaged functions"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Numerics",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_UT, 9, &MENU_DESCR);
}

void do_gr_objs(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('t', "(T)ext", "Create text labels in different fonts "),
    XPPAUT_ENTRY('a', "(A)rrow", "Add arrows to trajectories"),
    XPPAUT_ENTRY('p', "(P)ointer", "Create lines with arrowheads"),
    XPPAUT_ENTRY('m', "(M)arker", "Add squares, circles, etc to plot"),
    XPPAUT_ENTRY('e', "(E)dit", "Change properties, delete, or move existing add-on"),
    XPPAUT_ENTRY('d', "(D)elete all", "Delete all text, markers, arrows"),
    XPPAUT_ENTRY('s', "Marker(s)", "Create many markers based on browser data"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Text,etc",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_TT, 10, &MENU_DESCR);
}

void edit_object(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('m', "(M)ove", "Move the selected item"),
    XPPAUT_ENTRY('c', "(C)hange", "Change properties of selected item"),
    XPPAUT_ENTRY('d', "(D)elete", "Delete selected item"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Edit",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_TEM, 10, &MENU_DESCR);
}

void new_lookup(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('e', "(E)dit", "Edit the lookup tables"),
    XPPAUT_ENTRY('v', "(V)iew", "View a table in the data browser"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Tables",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_UKE, 11, &MENU_DESCR);
}

void find_bvp(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('r', "(R)ange", "Solve BVP over range of parameters"),
    XPPAUT_ENTRY('n', "(N)o show", "Don't show any but final step"),
    XPPAUT_ENTRY('s', "(S)how", "Show each step of iteration"),
    XPPAUT_ENTRY('p', "(P)eriodic", "Solve BVP with periodic conditions"),
    XPPAUT_ENTRY('h', "(H)omoclinic", "Set up special homoclinic stuff"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Bndry Value Prob",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_BR, 6, &MENU_DESCR);
}

void change_view(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('2', "(2)D", "Two-dimensional view settings"),
    XPPAUT_ENTRY('3', "(3)D", "Three-dimensional view settings"),
    XPPAUT_ENTRY('a', "(A)rray", "Plot array"),
    XPPAUT_ENTRY('t', "(T)oon", "Animation window"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Axes",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_V2, 13, &MENU_DESCR);
}

void do_windows(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('c', "(C)reate", "Create new window"),
    XPPAUT_ENTRY('k', "(K)ill all", "Delete all but main window"),
    XPPAUT_ENTRY('d', "(D)estroy", "Delete last window"),
    XPPAUT_ENTRY('b', "(B)ottom", "Place current window at bottom"),
    XPPAUT_ENTRY('a', "(A)uto", "Automatically redraw"),
    XPPAUT_ENTRY('m', "(M)anual", "Redraw only when requested"),
    XPPAUT_ENTRY('s', "(S)imPlot toggle", "Plot all graphs simultaneously -- slows you down"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Make window",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_MC, 14, &MENU_DESCR);
}

void add_a_curve(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('a', "(A)dd curve", "Add another curve to the current plot"),
    XPPAUT_ENTRY('d', "(D)elete last", "Delete last added plot"),
    XPPAUT_ENTRY('r', "(R)emove all", "Remove all the added plots except the main one"),
    XPPAUT_ENTRY('e', "(E)dit curve", "Edit parameters for a plot"),
    XPPAUT_ENTRY('p', "(P)ostscript", "Create a postscript file of current plot"),
    XPPAUT_ENTRY('v', "S(V)G", "Create a styleable svg file of current plot"),
    XPPAUT_ENTRY('f', "(F)reeze", "Options for permanently saving curve"),
    XPPAUT_ENTRY('x', "A(x)es opts", "Axes label sizes and zero axes for postscript"),
    XPPAUT_ENTRY('o', "Exp(o)rt data", "Export the numbers used in the graphs on the screen"),
    XPPAUT_ENTRY('c', "(C)olormap", "Change colormap"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Curves",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_GA, 8, &MENU_DESCR);
}

void freeze(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('f', "(F)reeze", "Permanently keep main curve -- even after reintegrating"),
    XPPAUT_ENTRY('d', "(D)elete", "Delete specified frozen curve"),
    XPPAUT_ENTRY('e', "(E)dit", "Edit specified frozen curve"),
    XPPAUT_ENTRY('r', "(R)emove all", "Remove all frozen curves"),
    XPPAUT_ENTRY('k', "(K)ey", "Toggle key on/off"),
    XPPAUT_ENTRY('b', "(B)if.Diag", "Import bifurcation data"),
    XPPAUT_ENTRY('c', "(C)lr. BD", "Clear imported bifurcation curve"),
    XPPAUT_ENTRY('o', "Freeze t(o)ggle", "Automatically freeze after each integration"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Freeze",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_GFF, 8, &MENU_DESCR);
}

void key_frz(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('n', "(N)o key", ""),
    XPPAUT_ENTRY('k', "(K)ey", ""),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Key",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_GFKN, 8, &MENU_DESCR);
}

void change_cmap(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('n', "(N)ormal", " blue-green-red"),
    XPPAUT_ENTRY('p', "(P)eriodic", "red-...-violet-red"),
    XPPAUT_ENTRY('h', "(H)ot", "black-red-yellow-white"),
    XPPAUT_ENTRY('c', "(C)ool", "pale cyan->pale yellow"),
    XPPAUT_ENTRY('b', "(B)lue-red", "blue-violet-red"),
    XPPAUT_ENTRY('g', "(G)ray", "black-white"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Colormap",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_GCN, 8, &MENU_DESCR);
}

void do_movie(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('c', "(C)apture", "Grab a screen shot"),
    XPPAUT_ENTRY('r', "(R)eset", "Clear all screen shots"),
    XPPAUT_ENTRY('p', "(P)layback", "Manually cycle thru screenshots"),
    XPPAUT_ENTRY('a', "(A)utoplay", "Continuously cycle through screen shots"),
    XPPAUT_ENTRY('s', "(S)ave", "Dump the screen shots to disk"),
    XPPAUT_ENTRY('m', "(M)ake AniGif", "Make animated gif file from screenshots"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Kinescope",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_KC, 8, &MENU_DESCR);
}

void do_torus(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('a', "(A)ll", "Each variable is on a circle"),
    XPPAUT_ENTRY('n', "(N)one", "No variable on circle"),
    XPPAUT_ENTRY('c', "(C)hoose", "Choose circle variables"),
  };
  X11MenuDescr menu_descr = {
    .title = "Torus",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };
  menu_descr.def_key = (TORUS ? 'a' : 'n');

  pop_up_and_run(M_AA, 4, &menu_descr);
}

void window_zoom(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('w', "(W)indow", "Manually choose 2D view"),
    XPPAUT_ENTRY('z', "(Z)oom In", "Zoom into with mouse"),
    XPPAUT_ENTRY('o', "Zoom (O)ut", "Zoom out with mouse"),
    XPPAUT_ENTRY('f', "(F)it", "Let XPP automatically choose window"),
    XPPAUT_ENTRY('d', "(D)efault", "Reset to default view"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Window",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_WW, 13, &MENU_DESCR);
}

void direct_field(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('d', "(D)irect Field", "Draw vector field for 2D section"),
    XPPAUT_ENTRY('f', "(F)low", "Draw regular series of trajectories"),
    XPPAUT_ENTRY('n', "(N)o dir. fld.", ""),
    XPPAUT_ENTRY('c', "(C)olorize", "Color the PP on a grid"),
    XPPAUT_ENTRY('s', "(S)caled Dir.Fld", "Draw only directions"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Two-D Fun",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_DD, 6, &MENU_DESCR);
}

void new_clines(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('n', "(N)ew", "Compute new nullclines"),
    XPPAUT_ENTRY('r', "(R)estore", "Redraw last nullclines"),
    XPPAUT_ENTRY('a', "(A)uto", "Set automatic redraw -- X redraws when needed"),
    XPPAUT_ENTRY('m', "(M)anual", "Only redraw when asked (Redraw)"),
    XPPAUT_ENTRY('f', "(F)reeze", "Freeze multiple nullclines"),
    XPPAUT_ENTRY('s', "(S)ave", "Save nullcline values to a file"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Nullclines",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_NN, 6, &MENU_DESCR);
}

void froz_cline_stuff(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('f', "(F)reeze", "Freeze current clines"),
    XPPAUT_ENTRY('d', "(D)elete all", "Delete all frozen clines"),
    XPPAUT_ENTRY('r', "(R)ange", "Range freeze a bunch of clines"),
    XPPAUT_ENTRY('a', "(A)nimate", "Animate nullclines"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Freeze cline",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_NFF, 6, &MENU_DESCR);
}

void find_equilibrium(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('g', "(G)o", "Find fixed points over range of parameter"),
    XPPAUT_ENTRY('m', "(M)ouse", ""),
    XPPAUT_ENTRY('r', "(R)ange", "Use mouse to guess fixed point"),
    XPPAUT_ENTRY('c', "Monte (C)ar", "Monte carlo search for fixed points"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Equilibria",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_SG, 6, &MENU_DESCR);
}

void ini_data_menu(void) {
  static const X11MenuEntry ENTRIES[] = {
    XPPAUT_ENTRY('r', "(R)ange", "Integrate over a range of parameters, init data, etc"),
    XPPAUT_ENTRY('2', "(2)par range", "Integrate over range of 2 parameters,init data, etc"),
    XPPAUT_ENTRY('l', "(L)ast", "Pick up from last step of previous solution"),
    XPPAUT_ENTRY('o', "(O)ld", "Use current initial data"),
    XPPAUT_ENTRY('g', "(G)o", "Use current initial data"),
    XPPAUT_ENTRY('m', "(M)ouse", "Specify initial data with mouse"),
    XPPAUT_ENTRY('s', "(S)hift", "Pick up from last step and shift T to latest time"),
    XPPAUT_ENTRY('n', "(N)ew", "Input new initial data"),
    XPPAUT_ENTRY('h', "S(h)hoot", "Use the initial data from last shooting from fixed pt"),
    XPPAUT_ENTRY('f', "(F)ile", "Read init data from file"),
    XPPAUT_ENTRY('u', "Form(u)la", "Type in function of 't' for t^th equation"),
    XPPAUT_ENTRY('i', "M(i)ce", "Repeated ICs using mouse "),
    XPPAUT_ENTRY('d', "(D)AE guess", "Guess new values for DAE"),
    XPPAUT_ENTRY('b', "(B)ackward", "Integrate backwards"),
  };
  static const X11MenuDescr MENU_DESCR = {
    .title = "Integrate",
    .entries = (X11MenuEntry *)ENTRIES,
    .num_entries = sizeof(ENTRIES) / sizeof(*ENTRIES),
    .def_key = -1,
  };

  pop_up_and_run(M_IR, 3, &MENU_DESCR);
}

void new_param(void) { run_the_commands(M_P); }

void clear_screens(void) { run_the_commands(M_EE); }

void x_vs_t(void) { run_the_commands(M_X); }

void redraw_them_all(void) { run_the_commands(M_R); }

void get_3d_par(void) { run_the_commands(M_3); }
