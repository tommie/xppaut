#ifndef XPPAUT_PP_SHOT_H
#define XPPAUT_PP_SHOT_H

#include <stdio.h>

/* --- Data --- */
extern int Homo_n;
extern int HOMOCLINIC_FLAG;

/* --- Functions --- */
void compile_bvp(void);
void do_bc(double *y__0, double t0, double *y__1, double t1, double *f, int n);
void dump_shoot_range(FILE *fp, int f);
void find_bvp_com(int com);
void init_shoot_range(char *s);
void reset_bvp(void);
int set_up_homoclinic(void);

#endif /* XPPAUT_PP_SHOT_H */
