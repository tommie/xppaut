#include "autpp.h"

#include <math.h>

#include "auto_define.h"
#include "auto_nox.h"
#include "derived.h"
#include "gear.h"
#include "load_eqn.h"
#include "numerics.h"
#include "odesol2.h"
#include "parserslow.h"
#include "pp_shoot.h"

/* --- Types --- */
typedef struct {
  int pt, br;
  double evr[NAUTO], evi[NAUTO];
} EIGVAL;

/* --- Data --- */
static AUTPP_CALLBACKS callbacks;
static EIGVAL my_ev;

void autpp_set_callbacks(const AUTPP_CALLBACKS *cbs) { callbacks = *cbs; }

doublereal dreal_(doublecomplex *z) { return (z->r); }

int eigrf_(doublereal *a, integer *n, integer *m, doublecomplex *ecv,
           doublereal *work, integer *ier) {
  double ev[400];
  int i;
  eigen(*n, a, ev, work, ier);
  for (i = 0; i < *n; i++) {
    (ecv + i)->r = ev[2 * i];
    (ecv + i)->i = ev[2 * i + 1];
  }
  return 0;
}

doublereal etime_(real *z) { return (0.0); }

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

void autpp_add_bif(integer *ibr, integer *ntot, integer *itp, integer *lab,
                   integer *npar, doublereal *a, doublereal *uhigh,
                   doublereal *ulow, doublereal *u0, doublereal *ubar,
                   integer *ndim) {
  int icp1 = blbcn_1.icp[0] - 1, icp2 = blbcn_1.icp[1] - 1;
  double per = blbcn_1.par[10];

  if (callbacks.add_diagram)
    callbacks.add_diagram(*ibr, *ntot, *itp, *lab, *npar, *a, uhigh, ulow, u0,
                          ubar, blbcn_1.par, per, *ndim, icp1, icp2, my_ev.evr,
                          my_ev.evi);
}

void autpp_check_stop(integer *iflag) {
  if (!callbacks.check_stop)
    return;

  *iflag = callbacks.check_stop();
}

void autpp_send_eigen(integer *ibr, integer *ntot, integer *n,
                      doublecomplex *ev) {
  int i;
  double er, cs, sn;
  my_ev.pt = abs(*ntot);
  my_ev.br = abs(*ibr);
  for (i = 0; i < *n; i++) {
    er = exp((ev + i)->r);
    cs = cos((ev + i)->i);
    sn = sin((ev + i)->i);
    my_ev.evr[i] = er * cs;
    my_ev.evi[i] = er * sn;
  }
}

void autpp_send_mult(integer *ibr, integer *ntot, integer *n,
                     doublecomplex *ev) {
  int i;
  my_ev.pt = abs(*ntot);
  my_ev.br = abs(*ibr);
  for (i = 0; i < *n; i++) {
    my_ev.evr[i] = (ev + i)->r;
    my_ev.evi[i] = (ev + i)->i;
  }
}
