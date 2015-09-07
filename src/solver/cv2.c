#include "cv2.h"

#include <string.h>

#define HMAX CV_HMAX
#define HMIN CV_HMIN
#include "band.h"
#include "cvband.h"
#include "cvdense.h"  /* prototype for CVDense, constant DENSE_NJE         */
#include "cvode.h"    /* prototypes for CVodeMalloc, CVode, and CVodeFree, */
                      /* constants OPT_SIZE, BDF, NEWTON, SV, SUCCESS,     */
                      /* NST, NFE, NSETUPS, NNI, NCFN, NETF                */
#include "dense.h"    /* definitions of type DenseMat, macro DENSE_ELEM    */
#include "llnltyps.h" /* definitions of types real (set to double) and     */
                      /* integer (set to int), and the constant FALSE      */
#include "vector.h"   /* definitions of type N_Vector and macro N_VIth,    */
                      /* prototypes for N_VNew, N_VFree                    */
#undef HMIN
#undef HMAX

#include "../delay_handle.h"
#include "../flags.h"
#include "../ggets.h"
#include "../load_eqn.h"
#include "../my_rhs.h"
#include "../numerics.h"

/* --- Data --- */
static double cv_ropt[OPT_SIZE];
static int cv_iopt[OPT_SIZE];
static void *cvode_mem;
static N_Vector ycv;

static void cvf(int n, double t, N_Vector y, N_Vector ydot, void *fdata) {
  my_rhs(t, y->data, ydot->data, n);
}

static void start_cv(double *y, double t, int n, double tout, double *atol,
                     double *rtol) {
  int i;

  ycv = N_VNew(n, NULL);
  for (i = 0; i < n; i++)
    ycv->data[i] = y[i];
  cvode_mem = CVodeMalloc(n, cvf, t, ycv, BDF, NEWTON, SS, rtol, atol, NULL,
                          NULL, FALSE, cv_iopt, cv_ropt, NULL);
  if (cv_bandflag == 1)
    CVBand(cvode_mem, cv_bandupper, cv_bandlower, NULL, NULL);
  else
    CVDense(cvode_mem, NULL, NULL);
}

void end_cv(void) {
  N_VFree(ycv);
  CVodeFree(cvode_mem);
}

void cvode_err_msg(int kflag) {
  char s[256];
  strcpy(s, "");
  switch (kflag) {
  case 0:
    strcpy(s, "");
    break;
  case -1:
    strcpy(s, "No memory allocated");
    break;
  case -2:
    strcpy(s, "Bad input to CVode");
    break;
  case -3:
    strcpy(s, "Too much work -- try smaller DT");
    break;
  case -4:
    sprintf(s, "Tolerance too low-- try TOL=%g ATOL=%g", TOLER * cv_ropt[TOLSF],
            ATOLER * cv_ropt[TOLSF]);
    break;
  case -5:
    strcpy(s, "Error test failure too frequent ??");
    break;
  case -6:
    strcpy(s, "Converg. failure -- oh well!");
    break;
  case -7:
    strcpy(s, "Setup failed for linsolver in CVODE ???");
    break;
  case -8:
    strcpy(s, "Singular matrix encountered. Hmmm?");
    break;
  case -9:
    strcpy(s, "Flags error...");
    break;
  }
  if (strlen(s) > 0)
    err_msg(s);
}

/* rtol is like our TOLER and atol is something else ?? */
/* command =0 continue, 1 is start 2 finish */
static int ccvode(int *command, double *y, double *t, int n, double tout,
                  int *kflag, double *atol, double *rtol) {
  int i, flag;
  *kflag = 0;
  if (*command == 2) {
    end_cv();
    return (1);
  }
  if (*command == 1) {
    start_cv(y, *t, n, tout, atol, rtol);
    flag = CVode(cvode_mem, tout, ycv, t, NORMAL);
    if (flag != SUCCESS) {

      *kflag = flag;
      end_cv();
      *command = 1;
      return (-1);
    }
    *command = 0;
    for (i = 0; i < n; i++)
      y[i] = ycv->data[i];
    return (0);
  }
  flag = CVode(cvode_mem, tout, ycv, t, NORMAL);
  if (flag != SUCCESS) {
    *kflag = flag;
    end_cv();
    *command = 1;

    return (-1);
  }
  for (i = 0; i < n; i++)
    y[i] = ycv->data[i];
  return (0);
}

/* command =0 continue, 1 is start 2 finish */
static int one_flag_step_cvode(int *command, double *y, double *t, int n,
                               double tout, int *kflag, double *atol,
                               double *rtol) {
  double yold[MAXODE], told;
  int i, hit, neq = n;
  double s;
  int nstep = 0;
  while (1) {
    for (i = 0; i < neq; i++)
      yold[i] = y[i];
    told = *t;
    ccvode(command, y, t, n, tout, kflag, atol, rtol);
    if (*kflag < 0)
      break;
    if ((hit = one_flag_step(yold, y, command, told, t, neq, &s)) == 0)
      break;
    /* Its a hit !! */
    nstep++;
    end_cv();
    *command = 1; /* for cvode always reset  */
    if (*t == tout)
      break;
    if (nstep > (NFlags + 2)) {
      plintf(" Working too hard? ");
      plintf("smin=%g\n", s);
      return 1;
    }
  }
  return 0;
}

/* command =0 continue, 1 is start 2 finish */
int cvode(int *command, double *y, double *t, int n, double tout, int *kflag,
          double *atol, double *rtol) {
  int err = 0;
  if (NFlags == 0)
    err = ccvode(command, y, t, n, tout, kflag, atol, rtol);
  else
    err = one_flag_step_cvode(command, y, t, n, tout, kflag, atol, rtol);
  if (err == 1)
    *kflag = -9;
  if (!*kflag)
    stor_delay(y);
  return err;
}
