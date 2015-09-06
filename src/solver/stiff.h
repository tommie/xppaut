#ifndef XPPAUT_SOLVER_STIFF_H
#define XPPAUT_SOLVER_STIFF_H

/* --- Functions --- */
int adaptive(double *ystart, int nvar, double *xs, double x2, double eps,
             double *hguess, double hmin, double *work, int *ier, double epjac,
             int iflag, int *jstart);

#endif /* XPPAUT_SOLVER_STIFF_H */
