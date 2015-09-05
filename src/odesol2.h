#ifndef XPPAUT_ODESOL2_H
#define XPPAUT_ODESOL2_H

/* --- Data --- */
extern int (*rhs)(double t, double *y, double *ydot, int neq);

/* --- Functions --- */
int rb23(double *y, double *tstart, double tfinal, int *istart, int n,
         double *work, int *ierr);
int rosen(double *y, double *tstart, double tfinal, int *istart, int n,
          double *work, int *ierr);

#endif /* XPPAUT_ODESOL2_H */
