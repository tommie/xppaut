/* Modified Euler  */
#include "modeuler.h"

#include "../delay_handle.h"
#include "../flags.h"
#include "../ggets.h"
#include "../markov.h"
#include "../odesol2.h"

static void one_step_heun(double *y, double dt, double *yval[2], int neq,
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

static int one_flag_step_heun(double *y, double dt, double *yval[2], int neq,
                              double *tim, int *istart) {
  double yold[MAXODE], told;
  int i, hit;
  double s, dtt = dt;
  int nstep = 0;
  while (1) {
    for (i = 0; i < neq; i++)
      yold[i] = y[i];
    told = *tim;
    one_step_heun(y, dtt, yval, neq, tim);
    if ((hit = one_flag_step(yold, y, istart, told, tim, neq, &s)) == 0)
      break;
    /* Its a hit !! */
    nstep++;
    dtt = (1 - s) * dt;
    if (nstep > (NFlags + 2)) {
      plintf(" Working too hard? ");
      plintf(" smin=%g\n", s);
      break;
    }
  }
  return (1);
}

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
