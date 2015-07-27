#ifndef XPPAUT_ADJ2_H
#define XPPAUT_ADJ2_H

#include <stdio.h>

/* --- Data --- */
extern int AdjRange;

/* --- Functions --- */
void init_trans(void);
void dump_transpose_info(FILE *fp, int f);
int do_transpose(void);
void alloc_h_stuff(void);
void data_back(void);
void make_adj_com(int com);
void new_h_fun(int silent);
void dump_h_stuff(FILE *fp, int f);
void new_adjoint(void);
void do_liapunov(void);
void alloc_liap(int n);
void do_this_liaprun(int i, double p);

#endif /* XPPAUT_ADJ2_H */
