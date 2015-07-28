#ifndef XPPAUT_CV2_H
#define XPPAUT_CV2_H

/* --- Functions --- */
int ccvode(int *command, double *y, double *t, int n, double tout, int *kflag,
           double *atol, double *rtol);
int cvode(int *command, double *y, double *t, int n, double tout, int *kflag,
          double *atol, double *rtol);
void cvode_err_msg(int kflag);
void end_cv(void);

#endif /* XPPAUT_CV2_H */
