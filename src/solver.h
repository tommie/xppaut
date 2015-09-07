#ifndef XPPAUT_SOLVER_H
#define XPPAUT_SOLVER_H

/* --- Functions --- */
void solver_end(void);
void solver_errmsg(int kflag);
int solver_integrate(double *y, double *t, int node, double tend, int *istart);

#endif /* XPPAUT_SOLVER_H */
