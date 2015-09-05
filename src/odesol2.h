#ifndef XPPAUT_ODESOL2_H
#define XPPAUT_ODESOL2_H

/* --- Data --- */
extern int (*rhs)(double t, double *y, double *ydot, int neq);

/* --- Functions --- */
int adams(double *y, double *tim, double dt, int nstep, int neq, int *ist,
          double *work);
int mod_euler(double *y, double *tim, double dt, int nt, int neq, int *istart,
              double *work);
void one_step_heun(double *y, double dt, double *yval[2], int neq, double *tim);
int rb23(double *y, double *tstart, double tfinal, int *istart, int n,
         double *work, int *ierr);
int rosen(double *y, double *tstart, double tfinal, int *istart, int n,
          double *work, int *ierr);

#endif /* XPPAUT_ODESOL2_H */
