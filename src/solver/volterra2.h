#ifndef XPPAUT_SOLVER_VOLTERRA2_H
#define XPPAUT_SOLVER_VOLTERRA2_H

#include "xpplim.h"

/* --- Macros --- */
/* KERNEL.flag values */
#define CONV 2

/* --- Types --- */
typedef struct {
  double k_n1, k_n, sum, betnn, mu, *al, *cnv;
  int *formula, flag, *kerform;
  char name[20], *expr, *kerexpr;
} KERNEL;

/* --- Data --- */
extern int AutoEvaluate;
extern KERNEL kernel[MAXKER];
extern int MaxPoints;
extern int NKernel;

/* --- Functions --- */
void alloc_kernels(int flag);
void alloc_v_memory(void);
void allocate_volterra(int npts, int flag);
double ker_val(int in);
void re_evaluate_kernels(void);
int volterra(double *y, double *t, double dt, int nt, int neq, int *istart,
             double *work);
int volterra2_add_kernel(const char *expr, double mu);

#endif /* XPPAUT_SOLVER_VOLTERRA2_H */
