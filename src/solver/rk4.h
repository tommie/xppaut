#ifndef XPPAUT_SOLVER_RK4_H
#define XPPAUT_SOLVER_RK4_H

/* --- Functions --- */
int rung_kut(double *y, double *tim, double dt, int nt, int neq, int *istart,
             double *work);

#endif /* XPPAUT_SOLVER_RK4_H */
