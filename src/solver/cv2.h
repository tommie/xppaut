#ifndef XPPAUT_SOLVER_CV2_H
#define XPPAUT_SOLVER_CV2_H

/* --- Functions --- */
int cvode(int *command, double *y, double *t, int n, double tout, int *kflag,
          double *atol, double *rtol);
void cvode_err_msg(int kflag);
void end_cv(void);

#endif /* XPPAUT_SOLVER_CV2_H */
