#ifndef XPPAUT_SOLVER_GEAR_H
#define XPPAUT_SOLVER_GEAR_H

/* --- Functions --- */
int gear(int n, double *t, double tout, double *y, double hmin, double hmax,
         double eps, int mf, double *error, int *kflag, int *jstart,
         double *work, int *iwork);
const char* gear_errmsg(int kflag);

#endif /* XPPAUT_SOLVER_GEAR_H */
