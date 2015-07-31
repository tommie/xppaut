#ifndef XPPAUT_MARKOV_H
#define XPPAUT_MARKOV_H

#include <stdio.h>

/* --- Data --- */
extern int NWiener;
extern int STOCH_FLAG;
extern int storind;

/* --- Functions --- */
void add_markov(int nstate, char *name);
void add_wiener(int index);
void  append_stoch(int first, int length);
int build_markov(char **ma, char *name);
void compile_all_markov(void);
void  do_stats(int ierr);
void  do_stochast_com(int i);
void  make_gill_nu(double *nu, int n, int m, double *v);
void  mean_back(void);
double ndrand48(void);
void nsrand48(int seed);
int old_build_markov(FILE *fptr, char *name);
void  one_gill_step(int meth, int nrxn, int *rxn, double *v);
double poidev(double xm);
void set_wieners(double dt, double *x, double t);
void  variance_back(void);

#endif /* XPPAUT_MARKOV_H */
