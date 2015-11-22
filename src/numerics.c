/* The input is primitive and eventually, I want to make it so
   that it uses nice windows for input.
   For now, I just will let it remain command driven
 */
#include "numerics.h"

#include <math.h>
#include <stdlib.h>
#include <strings.h>

#include "adj2.h"
#include "browse.h"
#include "color.h"
#include "delay_handle.h"
#include "flags.h"
#include "form_ode.h"
#include "ggets.h"
#include "graf_par.h"
#include "integrate.h"
#include "load_eqn.h"
#include "main.h"
#include "many_pops.h"
#include "menu.h"
#include "menudrive.h"
#include "menus.h"
#include "parserslow.h"
#include "pop_list.h"
#include "pp_shoot.h"
#include "solver.h"
#include "storage.h"
#include "solver/volterra2.h"

/* --- Forward Declarations --- */
static void check_pos(int *j);
static Method get_method(void);
static void ruelle(void);

/* --- Data --- */
int cv_bandflag = 0, cv_bandupper = 1, cv_bandlower = 1;

void check_pos(int *j) {
  if (*j <= 0)
    *j = 1;
}

void quick_num(int com) {
  char key[] = "tsrdnviobec";
  if (com >= 0 && com < 11)
    get_num_par(key[com]);
}

void set_total(double total) {
  int n;
  n = (total / fabs(DELTA_T)) + 1;
  TEND = n * fabs(DELTA_T);
}

void get_num_par(int ch) {
  double temp;
  int tmp;
  switch (ch) {
  case 'a':
    make_adj();
    break;

  case 't':
    flash(0);
    /* total */
    new_float("total :", &TEND);
    FOREVER = 0;
    if (TEND < 0) {
      FOREVER = 1;
      TEND = -TEND;
    }

    flash(0);
    break;
  case 's':
    flash(1);
    /* start */
    new_float("start time :", &T0);
    flash(1);
    break;
  case 'r':
    flash(2);
    /* transient */
    new_float("transient :", &TRANS);
    flash(2);
    break;
  case 'd':
    flash(3);
    /* DT */
    temp = DELTA_T;
    new_float("Delta t :", &DELTA_T);
    if (DELTA_T == 0.0)
      DELTA_T = temp;
    if (DELAY > 0.0) {
      free_delay();
      if (alloc_delay(DELAY)) {
        INFLAG = 0; /*  Make sure no last ics allowed */
      }
    } else
      free_delay();
    if (NKernel > 0) {
      INFLAG = 0;
      MyStart = 1;
      alloc_kernels(1);
    }
    /* if(NMemory>0){
       make_kernels();
       reset_memory();
       INFLAG=0;
     } */
    flash(3);
    break;
  case 'n':
    flash(4);
    /* ncline */
    new_int("ncline mesh :", &NMESH);
    /* new_float("Error :",&NULL_ERR); */
    check_pos(&NMESH);

    flash(4);
    break;
  case 'v':
    /*   new_int("Number Left :", &BVP_NL);
       new_int("Number Right :", &BVP_NR); */

    new_int("Maximum iterates :", &BVP_MAXIT);
    check_pos(&BVP_MAXIT);
    new_float("Tolerance :", &BVP_TOL);
    new_float("Epsilon :", &BVP_EPS);
    reset_bvp();
    break;
  case 'i':
    flash(5);
    /* sing pt */
    new_int("Maximum iterates :", &EVEC_ITER);
    check_pos(&EVEC_ITER);
    new_float("Newton tolerance :", &EVEC_ERR);
    new_float("Jacobian epsilon :", &NEWT_ERR);
    if (NFlags > 0)
      new_float("SMIN :", &STOL);

    flash(5);
    break;
  case 'o':
    flash(6);
    /* noutput */
    new_int("n_out :", &NJMP);
    check_pos(&NJMP);

    flash(6);
    break;
  case 'b':
    flash(7);
    /* bounds */
    new_float("Bounds :", &BOUND);
    BOUND = fabs(BOUND);

    flash(7);
    break;
  case 'm':
    flash(8);
    /* method */
    {
      Method m = get_method();

      if (m == METHOD_UNKNOWN)
        break;

      solver_set_method(m);
    }
    if (METHOD == METHOD_GEAR || METHOD == METHOD_RKQS ||
        METHOD == METHOD_STIFF) {
      new_float("Tolerance :", &TOLER);
      new_float("minimum step :", &HMIN);
      new_float("maximum step :", &HMAX);
    }
    if (METHOD == METHOD_CVODE || METHOD == METHOD_DP5 ||
        METHOD == METHOD_DP83 || METHOD == METHOD_RB23) {
      new_float("Relative tol:", &TOLER);
      new_float("Abs. Toler:", &ATOLER);
    }

    if (METHOD == METHOD_BACKEUL || METHOD == METHOD_VOLTERRA) {
      new_float("Tolerance :", &EulTol);
      new_int("MaxIter :", &MaxEulIter);
    }
    if (METHOD == METHOD_VOLTERRA) {
      tmp = MaxPoints;
      new_int("MaxPoints:", &tmp);
      new_int("AutoEval(1=yes) :", &AutoEvaluate);
      allocate_volterra(tmp, 1);
    }

    if (METHOD == METHOD_CVODE || METHOD == METHOD_RB23) {
      new_int("Banded system(0/1)?", &cv_bandflag);
      if (cv_bandflag == 1) {
        new_int("Lower band:", &cv_bandlower);
        new_int("Upper band:", &cv_bandupper);
      }
    }
    flash(8);
    break;
  case 'e':
    flash(9);
    /* delay */
    if (NDELAYS == 0)
      break;
    new_float("Maximal delay :", &DELAY);
    new_float("real guess :", &AlphaMax);
    new_float("imag guess :", &OmegaMax);
    new_int("DelayGrid :", &DelayGrid);
    if (DELAY > 0.0) {
      free_delay();
      if (alloc_delay(DELAY)) {
        INFLAG = 0; /*  Make sure no last ics allowed */
      }
    } else
      free_delay();

    flash(9);
    break;
  case 'c':
    flash(10);
    /* color */
    if (COLOR == 0)
      break;
    set_col_par();
    flash(10);
    break;
  case 'h':
    flash(11);
    do_stochast();
    flash(11);
    break;
  case 'f':
    flash(11);
    /* FFT */
    flash(11);
    break;
  case 'p':
    flash(12);
    /*Poincare map */
    get_pmap_pars();
    flash(12);
    break;
  case 'u':
    flash(13);
    /* ruelle */
    ruelle();
    flash(13);
    break;
  case 'k':
    flash(14);
    /*lookup table */
    new_lookup();
    flash(14);
    break;
  case 27:
    TEND = fabs(TEND);
    alloc_meth();
    help();
    break;

  } /* End num switch */
}

void chk_delay(void) {
  if (DELAY > 0.0) {
    free_delay();
    if (alloc_delay(DELAY)) {
      INFLAG = 0; /*  Make sure no last ics allowed */
    }
  } else
    free_delay();
}

void set_delay(void) {
  if (NDELAYS == 0)
    return;
  if (DELAY > 0.0) {
    free_delay();
    if (alloc_delay(DELAY)) {
      INFLAG = 0;
    }
  }
}

void ruelle(void) {
  new_int("x-axis shift ", &(MyGraph->xshft));
  new_int("y-axis shift ", &(MyGraph->yshft));
  new_int("z-axis shift", &(MyGraph->zshft));
  if (MyGraph->xshft < 0)
    MyGraph->xshft = 0;
  if (MyGraph->yshft < 0)
    MyGraph->yshft = 0;
  if (MyGraph->zshft < 0)
    MyGraph->zshft = 0;
}

void compute_one_period(double period, double *x, char *name) {
  int opm = POIMAP;
  char filename[256];
  double ot = TRANS, ote = TEND;
  FILE *fp;
  TRANS = 0;
  T0 = 0;
  MyTime = 0;
  TEND = period;
  POIMAP = 0; /* turn off poincare map */
  reset_browser();

  usual_integrate_stuff(x);
  sprintf(filename, "orbit.%s.dat", name);
  fp = fopen(filename, "w");
  if (fp != NULL) {
    write_mybrowser_data(fp);
    fclose(fp);
  } else {
    TRANS = ot;
    POIMAP = opm;
    TEND = ote;

    return;
  }
  new_adjoint();
  sprintf(filename, "adjoint.%s.dat", name);
  fp = fopen(filename, "w");
  if (fp != NULL) {
    write_mybrowser_data(fp);
    fclose(fp);
    data_back();
  }
  new_h_fun(1);
  sprintf(filename, "hfun.%s.dat", name);
  fp = fopen(filename, "w");
  if (fp != NULL) {
    write_mybrowser_data(fp);
    fclose(fp);
    data_back();
  }

  reset_browser();

  TRANS = ot;
  POIMAP = opm;
  TEND = ote;
}

void get_pmap_pars_com(int l) {
  static char mkey[] = "nsmp";
  char ch;
  static char *n[] = {"*0Variable", "Section", "Direction (+1,-1,0)",
                      "Stop on sect(y/n)"};
  char values[4][MAX_LEN_SBOX];
  static char *yn[] = {"N", "Y"};
  int status;
  char n1[15];
  int i1 = POIVAR;

  ch = mkey[l];

  POIMAP = 0;
  if (ch == 's')
    POIMAP = 1;
  if (ch == 'm')
    POIMAP = 2;
  if (ch == 'p')
    POIMAP = 3;

  if (POIMAP == 0)
    return;

  ind_to_sym(i1, n1);
  sprintf(values[0], "%s", n1);
  sprintf(values[1], "%.16g", POIPLN);
  sprintf(values[2], "%d", POISGN);
  sprintf(values[3], "%s", yn[SOS]);
  status = do_string_box(4, 4, 1, "Poincare map", n, values, 45);
  if (status != 0) {
    find_variable(values[0], &i1);
    if (i1 < 0) {
      POIMAP = 0;
      err_msg("No such section");
      return;
    }
    POIVAR = i1;
    POISGN = atoi(values[2]);
    if (values[3][0] == 'Y' || values[3][0] == 'y')
      SOS = 1;
    else
      SOS = 0;
    POIPLN = atof(values[1]);
  }
}

static Method get_method(void) {
  char ch;
  int i;
  int nmeth;

  /* This must match enum Method. */
  static char *n[] = {"(D)iscrete",    "(E)uler",   "(M)od. Euler",
                      "(R)unge-Kutta", "(A)dams",   "(G)ear",
                      "(V)olterra",    "(B)ackEul", "(Q)ualst.RK4",
                      "(S)tiff",       "(C)Vode",   "DoPri(5)",
                      "DoPri(8)3",     "Rosen(2)3", "sYmplectic"};
  static char key[] = "demragvbqsc582y";

  nmeth = sizeof(n) / sizeof(*n);
  ch = (char)pop_up_list(main_win, "Method", n, key, nmeth, nmeth, METHOD, 10,
                         DCURY + 8, meth_hint, main_status_bar);
  for (i = 0; i < nmeth; i++)
    if (ch == key[i])
      return (Method)i;

  return METHOD_UNKNOWN;
}

void user_set_color_par(int flag, char *via, double lo, double hi) {
  int ivar;
  MyGraph->min_scale = lo;
  if (hi > lo)
    MyGraph->color_scale = (hi - lo);
  else
    MyGraph->color_scale = 1;

  if (strncasecmp("speed", via, 5) == 0) {
    MyGraph->ColorFlag = 1;
  } else {
    find_variable(via, &ivar);
    if (ivar >= 0) {
      MyGraph->ColorValue = ivar;
      MyGraph->ColorFlag = 2;
    } else {
      MyGraph->ColorFlag = 0; /* no valid colorizing */
    }
  }
  if (flag == 0) { /* force overwrite  */
    MyGraph->ColorFlag = 0;
  }
}

void set_col_par_com(int i) {
  int j, ivar;
  double temp[2];
  float maxder = 0.0, minder = 0.0, sum = 0.0;
  char ch, name[20];
  MyGraph->ColorFlag = i;
  if (MyGraph->ColorFlag == 0) {
    /* set color to black/white */
    return;
  }
  if (MyGraph->ColorFlag == 2) {
    ind_to_sym(MyGraph->ColorValue, name);
    new_string("Color via:", name);
    find_variable(name, &ivar);

    if (ivar >= 0)
      MyGraph->ColorValue = ivar;
    else {

      err_msg("No such quantity!");
      MyGraph->ColorFlag = 0;
      return;
    }
  }

  /*   This will be uncommented    ..... */
  ch = TwoChoice("(O)ptimize", "(C)hoose", "Color", "oc");

  if (ch == 'c') {
    temp[0] = MyGraph->min_scale;
    temp[1] = MyGraph->min_scale + MyGraph->color_scale;
    new_float("Min :", &temp[0]);
    new_float("Max :", &temp[1]);
    if (temp[1] > temp[0] && ((MyGraph->ColorFlag == 2) ||
                              (MyGraph->ColorFlag == 1 && temp[0] >= 0.0))) {
      MyGraph->min_scale = temp[0];
      MyGraph->color_scale = (temp[1] - temp[0]);
    } else {
      err_msg("Min>=Max or Min<0 error");
    }
    return;
  }
  if (MyGraph->ColorFlag == 1) {
    if (storind < 2)
      return;
    maxder = 0.0;
    minder = 1.e20;
    for (i = 1; i < my_browser.maxrow; i++) {
      sum = 0.0;
      for (j = 0; j < NODE; j++)
        sum += (float)fabs((double)(my_browser.data[1 + j][i] -
                                    my_browser.data[1 + j][i - 1]));
      if (sum < minder)
        minder = sum;
      if (sum > maxder)
        maxder = sum;
    }
    if (minder >= 0.0 && maxder > minder) {
      MyGraph->color_scale = (maxder - minder) / (fabs(DELTA_T * NJMP));
      MyGraph->min_scale = minder / (fabs(DELTA_T * NJMP));
    }
  } else {
    get_max(MyGraph->ColorValue, &temp[0], &temp[1]);
    MyGraph->min_scale = temp[0];
    MyGraph->color_scale = (temp[1] - temp[0]);
    if (MyGraph->color_scale == 0.0)
      MyGraph->color_scale = 1.0;
  }
}
