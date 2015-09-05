#ifndef XPPAUT_GEAR_H
#define XPPAUT_GEAR_H

/* --- Functions --- */
int gear(int n, double *t, double tout, double *y, double hmin, double hmax,
         double eps, int mf, double *error, int *kflag, int *jstart,
         double *work, int *iwork);
int ggear(int n, double *t, double tout, double *y, double hmin, double hmax,
          double eps, int mf, double *error, int *kflag, int *jstart,
          double *work, int *iwork);

#endif /* XPPAUT_GEAR_H */
