#ifndef XPPAUT_FLAGS_H
#define XPPAUT_FLAGS_H

/* --- Data --- */
extern int NFlags;
extern double STOL;

/* --- Functions --- */
int add_global(char *cond, int sign, char *rest);
void show_flags(void);
int compile_flags(void);
int one_flag_step(double *yold, double *ynew, int *istart, double told,
                  double *tnew, int neq, double *s);
int one_flag_step_gear(int neq, double *t, double tout, double *y, double hmin,
                       double hmax, double eps, int mf, double *error,
                       int *kflag, int *jstart, double *work, int *iwork);
int one_flag_step_dp(int *istart, double *y, double *t, int n, double tout,
                     double *tol, double *atol, int flag, int *kflag);
int one_flag_step_adap(double *y, int neq, double *t, double tout, double eps,
                       double *hguess, double hmin, double *work, int *ier,
                       double epjac, int iflag, int *jstart);
int one_flag_step_cvode(int *command, double *y, double *t, int n, double tout,
                        int *kflag, double *atol, double *rtol);

#endif /* XPPAUT_FLAGS_H */
