#ifndef XPPAUT_AUTPP_H
#define XPPAUT_AUTPP_H

#include "f2c.h"

/* --- Types --- */
typedef struct {
  void (*add_diagram)(int ibr, int ntot, int itp, int lab, int npar, double a,
                      const double *uhi, const double *ulo, const double *u0,
                      const double *ubar, const double *par, const double per,
                      int n, int icp1, int icp2, const double *evr,
                      const double *evi);

  int (*check_stop)(void);
} AUTPP_CALLBACKS;

/* --- Functions --- */
void autpp_set_callbacks(const AUTPP_CALLBACKS *cbs);

/* Support functions used by AUTO code. */
doublereal dreal_(doublecomplex *z);
int eigrf_(doublereal *a, integer *n, integer *m, doublecomplex *ecv,
           doublereal *work, integer *ier);
doublereal etime_(real *z);

/* Hook functions used by AUTO code. */
int bcnd_(integer *ndim, doublereal *par, integer *icp, integer *nbc,
          doublereal *u0, doublereal *u1, doublereal *fb, integer *ijac,
          doublereal *dbc);
int fopt_(integer *ndim, doublereal *u, integer *icp, doublereal *par,
          integer *ijac, doublereal *fs, doublereal *dfdu, doublereal *dfdp);
int func_(integer *ndim, doublereal *u, integer *icp, doublereal *par,
          integer *ijac, doublereal *f, doublereal *dfdu, doublereal *dfdp);
int icnd_(integer *ndim, doublereal *par, integer *icp, integer *nint,
          doublereal *u, doublereal *uold, doublereal *udot, doublereal *upold,
          doublereal *fi, integer *ijac, doublereal *dint);
int stpnt_(integer *ndim, doublereal *u, doublereal *par, doublereal *t);
doublereal uszr_(integer *i, integer *nuzr, doublereal *par);

/* --- Custom hook functions used by patched AUTO code. */
void autpp_add_bif(integer *ibr, integer *ntot, integer *itp, integer *lab,
                   integer *npar, doublereal *a, doublereal *uhigh,
                   doublereal *ulow, doublereal *u0, doublereal *ubar,
                   integer *ndim);
void autpp_check_stop(integer *iflag);
void autpp_send_eigen(integer *ibr, integer *ntot, integer *n,
                      doublecomplex *ev);
void autpp_send_mult(integer *ibr, integer *ntot, integer *n,
                     doublecomplex *ev);

#endif /* XPPAUT_AUTPP_H */
