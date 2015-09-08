#include "solver.h"

#include <math.h>

#include "form_ode.h"
#include "ggets.h"
#include "integrate.h"
#include "load_eqn.h"
#include "numerics.h"
#include "parserslow.h"
#include "storage.h"
#include "solver/adams.h"
#include "solver/backeuler.h"
#include "solver/cv2.h"
#include "solver/discrete.h"
#include "solver/dormpri.h"
#include "solver/euler.h"
#include "solver/gear.h"
#include "solver/modeuler.h"
#include "solver/rb23.h"
#include "solver/rk4.h"
#include "solver/stiff.h"
#include "solver/symplect.h"
#include "solver/volterra2.h"

/* --- Data --- */
Method METHOD;
static int (*solver)(double *y, double *tim, double dt, int nstep, int neq,
                     int *ist, double *work);

void solver_end(void) {
  switch (METHOD) {
  case METHOD_CVODE:
#ifdef CVODE_YES
    end_cv();
#endif
    break;

  default:
    break;
  }
}

void solver_errmsg(int kflag) {
  if (!kflag) {
    err_msg("success");
    return;
  }

  switch (METHOD) {
  case METHOD_CVODE:
#ifdef CVODE_YES
    cvode_err_msg(kflag);
#else
    err_msg("cvode not supported");
#endif
    break;

  case METHOD_DP5:
  case METHOD_DP83:
    dp_err(kflag + 1); /* Adjust so 1 means success. */
    break;

  case METHOD_RB23:
    err_msg("step size too small");
    break;

  case METHOD_RKQS:
  case METHOD_STIFF:
    err_msg(adaptive_errmsg(kflag));
    break;

  case METHOD_GEAR:
    err_msg(gear_errmsg(kflag));
    break;

  default:
    switch (kflag) {
    case -1:
      err_msg("singular Jacobian");
      break;
    case -2:
      err_msg("too many iterates");
      break;

    default:
      err_msg("unknown error");
    }
  }
}

int solver_integrate(double *y, double *t, int node, double tend, int *istart) {
  int nit;
  int kflag;
  double dt = DELTA_T;
  double error[MAXODE];

  switch (METHOD) {
  case METHOD_CVODE:
#ifdef CVODE_YES
    /* cvode(command,y,t,n,tout,kflag,atol,rtol)
     * command =0 continue, 1 is start 2 finish
     */
    cvode(istart, y, t, node, tend, &kflag, &TOLER, &ATOLER);
    return kflag;
#else
    return 911;
#endif

  case METHOD_DP5:
  case METHOD_DP83:
    dp(istart, y, t, node, tend, &TOLER, &ATOLER, METHOD - METHOD_DP5, &kflag);
    return kflag - 1; /* Adjust so 0 means success. */

  case METHOD_RB23:
    rb23(y, t, tend, istart, node, WORK, &kflag);
    return kflag < 0 ? kflag : 0;

  case METHOD_RKQS:
  case METHOD_STIFF:
    adaptive(y, node, t, tend, TOLER, &dt, HMIN, WORK, &kflag, NEWT_ERR, METHOD,
             istart);
    return kflag;

  case METHOD_GEAR:
    if (*istart)
      *istart = 0;
    gear(node, t, tend, y, HMIN, HMAX, TOLER, 2, error, &kflag, istart, WORK);
    return kflag < 0 ? kflag : 0;

  case METHOD_DISCRETE:
    nit = fabs(*t - tend);
    dt = dt / fabs(dt);
    return solver(y, t, dt, nit, node, istart, WORK);

  default:
    nit = (int)((tend - *t) / dt);
    kflag = solver(y, t, dt, nit, node, istart, WORK);

    if (kflag < 0)
      return kflag;

    if ((dt < 0 && *t > tend) || (dt > 0 && *t < tend)) {
      dt = tend - *t;
      kflag = solver(y, t, dt, 1, node, istart, WORK);
      if (kflag < 0)
        return kflag;
    }
  }

  return 0;
}

void solver_set_method(Method m) {
  if (m == METHOD_VOLTERRA && NKernel == 0) {
    err_msg("Volterra only for integral eqns");
    m = METHOD_ADAMS;
  }
  if (NKernel > 0)
    m = METHOD_VOLTERRA;
  if (m == METHOD_SYMPLECT) {
    if ((NODE % 2) != 0) {
      err_msg("Symplectic is only for even dimensions");
      m = METHOD_ADAMS;
    }
  }

  switch (m) {
  case METHOD_DISCRETE:
    solver = discrete;
    DELTA_T = 1;
    break;
  case METHOD_EULER:
    solver = euler;
    break;
  case METHOD_MODEULER:
    solver = mod_euler;
    break;
  case METHOD_RK4:
    solver = rung_kut;
    break;
  case METHOD_ADAMS:
    solver = adams;
    break;
  case METHOD_GEAR:
    NJMP = 1;
    break;
  case METHOD_VOLTERRA:
    solver = volterra;
    break;
  case METHOD_SYMPLECT:
    solver = symplect3;
    break;
  case METHOD_BACKEUL:
    solver = back_euler;
    break;
  case METHOD_RKQS:
  case METHOD_STIFF:
  case METHOD_CVODE:
  case METHOD_DP5:
  case METHOD_DP83:
  case METHOD_RB23:
    NJMP = 1;
    break;
  default:
    solver = rung_kut;
  }

  METHOD = m;
}
