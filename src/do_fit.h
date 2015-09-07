#ifndef XPPAUT_DO_FIT_H
#define XPPAUT_DO_FIT_H

/* --- Types --- */
typedef struct {
  char file[25];
  char varlist[25], collist[25];
  char parlist1[25], parlist2[25];
  int dim, npars, nvars, npts, maxiter;
  int icols[50], ipar[50], ivar[50];
  double tol, eps;
} FITINFO;

/* --- Functions --- */
void init_fit_info(void);
void test_fit(void);

#endif /* XPPAUT_DO_FIT_H */
