#include "solver.h"

#include <math.h>

#include "ggets.h"
#include "integrate.h"
#include "load_eqn.h"
#include "numerics.h"
#include "storage.h"
#include "solver/cv2.h"
#include "solver/dormpri.h"
#include "solver/gear.h"
#include "solver/rb23.h"
#include "solver/stiff.h"
#include "solver/volterra2.h"

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
