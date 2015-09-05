#include "discrete.h"

#include "../delay_handle.h"
#include "../flags.h"
#include "../ggets.h"
#include "../markov.h"
#include "../odesol2.h"

static void one_step_discrete(double *y, double dt, double *yp, int neq,
                              double *t) {
  int j;
  set_wieners(dt, y, *t);
  rhs(*t, y, yp, neq);
  *t = *t + dt;
  for (j = 0; j < neq; j++) {
    y[j] = yp[j];
  }
}

static int one_flag_step_discrete(double *y, double dt, double *work, int neq,
                                  double *tim, int *istart) {
  double yold[MAXODE], told;
  int i, hit;
  double s, dtt = dt;
  int nstep = 0;
  while (1) {
    for (i = 0; i < neq; i++)
      yold[i] = y[i];
    told = *tim;
    one_step_discrete(y, dtt, work, neq, tim);
    if ((hit = one_flag_step(yold, y, istart, told, tim, neq, &s)) == 0)
      break;
    /* Its a hit !! */
    nstep++;
    dtt = (1 - s) * dt;
    if (nstep > (NFlags + 2)) {
      plintf(" Working too hard?? ");
      plintf("smin=%g\n", s);
      break;
    }
  }
  return (1);
}

int discrete(double *y, double *tim, double dt, int nt, int neq, int *istart,
             double *work) {
  int i;
  if (NFlags == 0) {
    for (i = 0; i < nt; i++) {
      one_step_discrete(y, dt, work, neq, tim);
      stor_delay(y);
    }
    return (0);
  }
  for (i = 0; i < nt; i++) {
    one_flag_step_discrete(y, dt, work, neq, tim, istart);
    stor_delay(y);
  }
  return (0);
}
