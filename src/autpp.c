#include "f2c.h"
#include "auto_nox.h"
#include "derived.h"
#include "load_eqn.h"
#include "numerics.h"
#include "odesol2.h"
#include "parserslow.h"
#include "pp_shoot.h"

int func_(integer *ndim, doublereal *u, integer *icp, doublereal *par,
          integer *ijac, doublereal *f, doublereal *dfdu, doublereal *dfdp) {
  int i, j;
  double zz[NAUTO];
  for (i = 0; i < NAutoPar; i++) {
    constants[Auto_index_to_array[i]] = par[i];
  }
  evaluate_derived();
  rhs(0.0, u, f, *ndim);
  if (METHOD > 0 || NJMP == 1)
    return 0;
  for (i = 1; i < NJMP; i++) {
    for (j = 0; j < *ndim; j++)
      zz[j] = f[j];
    rhs(0.0, zz, f, *ndim);
  }
  return 0;
}

int stpnt_(integer *ndim, doublereal *u, doublereal *par, doublereal *t) {
  int i;

  double p;
  for (i = 0; i < NAutoPar; i++)
    par[i] = constants[Auto_index_to_array[i]];
  if (NewPeriodFlag == 0) {
    for (i = 0; i < *ndim; i++)
      u[i] = last_ic[i];
    return 0;
  }
  get_start_period(&p);
  par[10] = p;
  get_start_orbit(u, *t, p, *ndim);
  return 0;
}

doublereal uszr_(integer *i, integer *nuzr, doublereal *par) {
  int i0 = *i - 1;

  if (i0 < 0 || i0 >= NAutoUzr)
    return (1.0);
  return (par[UzrPar[i0]] - outperiod[i0]);
}

int bcnd_(integer *ndim, doublereal *par, integer *icp, integer *nbc,
          doublereal *u0, doublereal *u1, doublereal *fb, integer *ijac,
          doublereal *dbc) {
  int i;
  /* Hooks to the XPP bc parser!! */
  for (i = 0; i < NAutoPar; i++) {
    constants[Auto_index_to_array[i]] = par[i];
  }

  evaluate_derived();
  do_bc(u0, 0.0, u1, 1.0, fb, *nbc);
  return 0;
}

int icnd_(integer *ndim, doublereal *par, integer *icp, integer *nint,
          doublereal *u, doublereal *uold, doublereal *udot, doublereal *upold,
          doublereal *fi, integer *ijac, doublereal *dint) {
  int i;
  double dum = 0.0;

  for (i = 0; i < Homo_n; i++)
    dum += upold[i] * (u[i] - uold[i]);
  fi[0] = dum;

  return 0;
}

int fopt_(integer *ndim, doublereal *u, integer *icp, doublereal *par,
          integer *ijac, doublereal *fs, doublereal *dfdu, doublereal *dfdp) {
  return 0;
}
