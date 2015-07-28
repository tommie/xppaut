#ifndef XPPAUT_DEL_STAB_H
#define XPPAUT_DEL_STAB_H

/* --- Types --- */
typedef struct { double r, i; } COMPLEX;

/* --- Functions --- */
void do_delay_sing(double *x, double eps, double err, double big, int maxit,
                   int n, int *ierr, float *stabinfo);

#endif /* XPPAUT_DEL_STAB_H */
