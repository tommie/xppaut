#include "odesol2.h"

#include <math.h>

#include "delay_handle.h"
#include "flags.h"
#include "gear.h"
#include "load_eqn.h"
#include "markov.h"
#include "matrixalg.h"
#include "numerics.h"

/* --- Macros --- */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* --- Forward Declarations --- */
static int abmpc(double *y, double *t, double dt, int neq);

/* --- Data --- */
int (*rhs)(double t, double *y, double *ydot, int neq);

static double coefp[] = {6.875 / 3.00, -7.375 / 3.00, 4.625 / 3.00, -.375},
              coefc[] = {.375, 2.375 / 3.00, -.625 / 3.00, 0.125 / 3.00};
static double *y_s[4], *y_p[4], *ypred;

void one_step_rk4(double *y, double dt, double *yval[3], int neq, double *tim) {
  int i;
  double t = *tim, t1, t2;
  set_wieners(dt, y, t);
  rhs(t, y, yval[1], neq);
  for (i = 0; i < neq; i++) {
    yval[0][i] = y[i] + dt * yval[1][i] / 6.00;
    yval[2][i] = y[i] + dt * yval[1][i] * 0.5;
  }
  t1 = t + .5 * dt;
  rhs(t1, yval[2], yval[1], neq);
  for (i = 0; i < neq; i++) {
    yval[0][i] = yval[0][i] + dt * yval[1][i] / 3.00;
    yval[2][i] = y[i] + .5 * dt * yval[1][i];
  }
  rhs(t1, yval[2], yval[1], neq);
  for (i = 0; i < neq; i++) {
    yval[0][i] = yval[0][i] + dt * yval[1][i] / 3.000;
    yval[2][i] = y[i] + dt * yval[1][i];
  }
  t2 = t + dt;
  rhs(t2, yval[2], yval[1], neq);
  for (i = 0; i < neq; i++)
    y[i] = yval[0][i] + dt * yval[1][i] / 6.00;
  *tim = t2;
}

void one_step_heun(double *y, double dt, double *yval[2], int neq,
                   double *tim) {
  int i;
  double t = *tim, t1;
  set_wieners(dt, y, *tim);
  rhs(t, y, yval[0], neq);
  for (i = 0; i < neq; i++)
    yval[0][i] = dt * yval[0][i] + y[i];
  t1 = t + dt;
  rhs(t1, yval[0], yval[1], neq);
  for (i = 0; i < neq; i++)
    y[i] = .5 * (y[i] + yval[0][i] + dt * yval[1][i]);
  *tim = t1;
}

/* Modified Euler  */
int mod_euler(double *y, double *tim, double dt, int nt, int neq, int *istart,
              double *work) {
  double *yval[2];
  int j;

  yval[0] = work;
  yval[1] = work + neq;
  if (NFlags == 0) {
    for (j = 0; j < nt; j++) {
      one_step_heun(y, dt, yval, neq, tim);
      stor_delay(y);
    }
    return (0);
  }
  for (j = 0; j < nt; j++) {
    one_flag_step_heun(y, dt, yval, neq, tim, istart);
    stor_delay(y);
  }
  return (0);
}

/*  Runge Kutta    */
int rung_kut(double *y, double *tim, double dt, int nt, int neq, int *istart,
             double *work) {
  register int j;
  double *yval[3];

  yval[0] = work;
  yval[1] = work + neq;
  yval[2] = work + neq + neq;

  if (NFlags == 0) {
    for (j = 0; j < nt; j++) {
      one_step_rk4(y, dt, yval, neq, tim);
      stor_delay(y);
    }
    return (0);
  }

  for (j = 0; j < nt; j++) {
    one_flag_step_rk4(y, dt, yval, neq, tim, istart);
    stor_delay(y);
  }
  return (0);
}

/*   ABM   */
int adams(double *y, double *tim, double dt, int nstep, int neq, int *ist,
          double *work) {
  int istart = *ist, i, istpst, k, ik, n;
  int irk;
  double *work1;
  double x0 = *tim, xst = *tim;
  work1 = work;
  if (istart == 1) {
    for (i = 0; i < 4; i++) {
      y_p[i] = work + (4 + i) * neq;
      y_s[i] = work + (8 + i) * neq;
    }
    ypred = work + 3 * neq;
    goto n20;
  }
  if (istart > 1)
    goto n350;
  istpst = 0;
  goto n400;

n20:

  x0 = xst;
  rhs(x0, y, y_p[3], neq);
  for (k = 1; k < 4; k++) {
    rung_kut(y, &x0, dt, 1, neq, &irk, work1);
    stor_delay(y);
    for (i = 0; i < neq; i++)
      y_s[3 - k][i] = y[i];
    rhs(x0, y, y_p[3 - k], neq);
  }
  istpst = 3;
  if (istpst <= nstep)
    goto n400;
  ik = 4 - nstep;
  for (i = 0; i < neq; i++)
    y[i] = y_s[ik - 1][i];
  xst = xst + nstep * dt;
  istart = ik;
  goto n1000;

n350:

  ik = istart - nstep;
  if (ik <= 1)
    goto n370;
  for (i = 0; i < neq; i++)
    y[i] = y_s[ik - 1][i];
  xst = xst + nstep * dt;
  istart = ik;
  goto n1000;

n370:
  for (i = 0; i < neq; i++)
    y[i] = y_s[0][i];
  if (ik == 1) {
    x0 = xst + dt * nstep;
    goto n450;
  }

  istpst = istart - 1;

n400:

  if (istpst == nstep)
    goto n450;
  for (n = istpst + 1; n < nstep + 1; n++) {
    set_wieners(dt, y, x0);
    abmpc(y, &x0, dt, neq);
    stor_delay(y);
  }

n450:
  istart = 0;
  xst = x0;

n1000:

  *tim = *tim + nstep *dt;
  *ist = istart;
  return (0);
}

static int abmpc(double *y, double *t, double dt, int neq) {
  double x1, x0 = *t;
  int i, k;
  for (i = 0; i < neq; i++) {
    ypred[i] = 0;
    for (k = 0; k < 4; k++)
      ypred[i] = ypred[i] + coefp[k] * y_p[k][i];
    ypred[i] = y[i] + dt * ypred[i];
  }

  for (i = 0; i < neq; i++)
    for (k = 3; k > 0; k--)
      y_p[k][i] = y_p[k - 1][i];
  x1 = x0 + dt;
  rhs(x1, ypred, y_p[0], neq);

  for (i = 0; i < neq; i++) {
    ypred[i] = 0;
    for (k = 0; k < 4; k++)
      ypred[i] = ypred[i] + coefc[k] * y_p[k][i];
    y[i] = y[i] + dt * ypred[i];
  }
  *t = x1;
  rhs(x1, y, y_p[0], neq);

  return (1);
}

/* this is rosen  - rosenbock step
   This uses banded routines as well */
int rb23(double *y, double *tstart, double tfinal, int *istart, int n,
         double *work, int *ierr) {
  int out = -1;
  if (NFlags == 0) {
    out = rosen(y, tstart, tfinal, istart, n, work, ierr);
  } else {
    out = one_flag_step_rosen(y, tstart, tfinal, istart, n, work, ierr);
  }
  return (out);
}

int rosen(double *y, double *tstart, double tfinal, int *istart, int n,
          double *work, int *ierr) {
  static double htry;
  double epsjac = NEWT_ERR;
  double eps = 1e-15, hmin, hmax;
  double tdir = 1, t0 = *tstart, t = t0;
  double atol = ATOLER, rtol = TOLER;
  double sqrteps = sqrt(eps);
  double thresh = atol / rtol, absh, h;
  double d = 1 / (2. + sqrt(2.)), e32 = 6. + sqrt(2.), tnew;
  /*double ninf;  Is this needed?*/
  int i, n2 = n * n, done = 0, info, ml = cv_bandlower, mr = cv_bandupper,
         mt = ml + mr + 1;
  int ipivot[MAXODE1], nofailed;
  double temp, err, tdel;
  double *ypnew, *k1, *k2, *k3, *f0, *f1, *f2, *dfdt, *ynew, *dfdy;
  *ierr = 1;
  ypnew = work;
  k1 = ypnew + n;
  k2 = k1 + n;
  k3 = k2 + n;
  f0 = k3 + n;
  f1 = f0 + n;
  f2 = f1 + n;
  dfdt = f2 + n;
  ynew = dfdt + n;
  dfdy = ynew + n;

  if (t0 > tfinal)
    tdir = -1;
  hmax = fabs(tfinal - t);
  if (*istart == 1)
    htry = hmax;
  rhs(t0, y, f0, n);
  hmin = 16 * eps * fabs(t);
  absh = MIN(hmax, MAX(hmin, htry));
  while (!done) {
    nofailed = 1;
    hmin = 16 * eps * fabs(t);
    absh = MIN(hmax, MAX(hmin, absh));
    h = tdir * absh;
    if (1.1 * absh >= fabs(tfinal - t)) {
      h = tfinal - t;
      absh = fabs(h);
      done = 1;
    }
    get_the_jac(t, y, f0, ypnew, dfdy, n, epsjac, 1.0);
    tdel = (t + tdir * MIN(sqrteps * MAX(fabs(t), fabs(t + h)), absh)) - t;
    rhs(t + tdel, y, f1, n);
    for (i = 0; i < n; i++)
      dfdt[i] = (f1[i] - f0[i]) / tdel;
    while (1) { /* advance a step  */
      for (i = 0; i < n2; i++)
        dfdy[i] = -h * d * dfdy[i];
      for (i = 0; i < n; i++)
        k1[i] = f0[i] + (h * d) * dfdt[i];
      if (cv_bandflag) {
        for (i = 0; i < n; i++)
          dfdy[i * mt + ml] += 1;

        bandfac(dfdy, ml, mr, n);
        bandsol(dfdy, k1, ml, mr, n);
      } else {
        for (i = 0; i < n; i++)
          dfdy[i * n + i] += 1;

        sgefa(dfdy, n, n, ipivot, &info);
        sgesl(dfdy, n, n, ipivot, k1, 0);
      }
      for (i = 0; i < n; i++)
        ynew[i] = y[i] + .5 * h * k1[i];
      rhs(t + .5 * h, ynew, f1, n);
      for (i = 0; i < n; i++)
        k2[i] = f1[i] - k1[i];
      if (cv_bandflag)
        bandsol(dfdy, k2, ml, mr, n);
      else
        sgesl(dfdy, n, n, ipivot, k2, 0);
      for (i = 0; i < n; i++) {
        k2[i] = k2[i] + k1[i];
        ynew[i] = y[i] + h * k2[i];
      }
      tnew = t + h;
      rhs(tnew, ynew, f2, n);
      for (i = 0; i < n; i++)
        k3[i] = f2[i] - e32 * (k2[i] - f1[i]) - 2 * (k1[i] - f0[i]) +
                (h * d) * dfdt[i];
      if (cv_bandflag)
        bandsol(dfdy, k3, ml, mr, n);
      else
        sgesl(dfdy, n, n, ipivot, k3, 0);
      /*ninf=0;  This is not used anywhere?
      */
      err = 0.0;
      for (i = 0; i < n; i++) {
        temp = MAX(MAX(fabs(y[i]), fabs(ynew[i])), thresh);
        temp = fabs(k1[i] - 2 * k2[i] + k3[i]) / temp;
        if (err < temp)
          err = temp;
      }
      err = err * (absh / 6);
      /* plintf(" err=%g hmin=%g absh=%g \n",err,hmin,absh);
         wait_for_key(); */
      if (err > rtol) {
        if (absh < hmin) {
          /* plintf("rosen failed at t=%g. Step size too small \n",t);*/
          *ierr = -1;
          return (-1);
        }
        absh = MAX(hmin, absh * MAX(0.1, pow(0.8 * (rtol / err), 1. / 3.)));
        /* plintf(" absh=%g  %g  \n",absh,0.8*(rtol/err)); */
        h = tdir * absh;
        nofailed = 0;
        done = 0;
      } else {
        /* plintf(" successful step -- nofail=%d absh=%g \n",nofailed,absh); */
        break;
      }
    }
    if (nofailed == 1) {
      /* plintf(" I didn't fail! \n"); */
      temp = 1.25 * pow(err / rtol, 1. / 3.);
      if (temp > 0.2)
        absh = absh / temp;
      else
        absh = 5 * absh;
    }
    /* plintf("  absh=%g \n",absh); */
    t = tnew;
    for (i = 0; i < n; i++) {
      y[i] = ynew[i];
      f0[i] = f2[i];
    }
  }
  *tstart = t;
  htry = h;
  *istart = 0;
  return (0);
}
