#ifndef XPPAUT_SOLVER_MODEULER_H
#define XPPAUT_SOLVER_MODEULER_H

/* --- Functions --- */
int mod_euler(double *y, double *tim, double dt, int nt, int neq, int *istart,
              double *work);

#endif /* XPPAUT_SOLVER_MODEULER_H */
