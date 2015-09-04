#ifndef XPPAUT_GEAR_H
#define XPPAUT_GEAR_H

/* --- Functions --- */
int gear(int n, double *t, double tout, double *y, double hmin, double hmax,
         double eps, int mf, double *error, int *kflag, int *jstart,
         double *work, int *iwork);
int ggear(int n, double *t, double tout, double *y, double hmin, double hmax,
          double eps, int mf, double *error, int *kflag, int *jstart,
          double *work, int *iwork);
void sgefa(double *a, int lda, int n, int *ipvt, int *info);
void sgesl(double *a, int lda, int n, int *ipvt, double *b, int job);

#endif /* XPPAUT_GEAR_H */
