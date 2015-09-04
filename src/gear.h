#ifndef XPPAUT_GEAR_H
#define XPPAUT_GEAR_H

/* --- Functions --- */
int gear(int n, double *t, double tout, double *y, double hmin, double hmax,
         double eps, int mf, double *error, int *kflag, int *jstart,
         double *work, int *iwork);
int ggear(int n, double *t, double tout, double *y, double hmin, double hmax,
          double eps, int mf, double *error, int *kflag, int *jstart,
          double *work, int *iwork);

/**
 * Factors a real matrix by Gaussian elimination.
 *
 * @param a (in/out) the matrix to be factored.
 * @param lda the leading dimension of the array A.
 * @param n the order of the matrix A.
 * @param ipvt (out) an integer vector of pivot indices.
 * @param info (out) = 0  normal value.
 *             = k  if  u(k,k) .eq. 0.0 .  This is not an error
 *                  condition for this subroutine, but it does
 *                  indicate that SGESL or SGEDI will divide by zero
 *                  if called.  Use  RCOND  in SGECO for a reliable
 *                  indication of singularity.
 */
void sgefa(double *a, int lda, int n, int *ipvt, int *info);

/**
 * Solves the real system
 *   A * X = B  or  transpose(A) * X = B
 * using factors computed by sgeco or sgefa.
 *
 * @param a the output from sgeco or sgefa.
 * @param lda the leading dimension of the array A.
 * @param n the order of the matrix A.
 * @param ipvt the pivot vector from sgeco or sgefa.
 * @param b (in/out) the right hand side vector.
 * @param job whether to solve the transpose(A) problem.
 */
void sgesl(double *a, int lda, int n, int *ipvt, double *b, int job);

#endif /* XPPAUT_GEAR_H */
