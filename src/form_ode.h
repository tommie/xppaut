#ifndef XPPAUT_FORM_ODE_H
#define XPPAUT_FORM_ODE_H

#include <stdio.h>
#include "xpplim.h"

/* --- Macros --- */
#define MAXVNAM 33
#define MAXLINES 5000
#define MAXCOMMENTS 500

/* --- Types --- */
typedef struct {
  char *text, *action;
  int aflag;
} ACTION;

typedef struct {
  int *com;
  char *string;
  char *name;
  int side;
} BC_STRUCT;

typedef struct { char *name, *value; } FIXINFO;

/* --- Data --- */
extern ACTION comments[MAXCOMMENTS];
extern FILE *convertf;
extern int ConvertStyle;
extern double default_ic[MAXODE];
extern double default_val[MAXPAR];
extern int EqType[MAXODE];
extern FIXINFO fixinfo[MAXODE];
extern int FIX_VAR;
extern BC_STRUCT my_bc[MAXODE];
extern int *my_ode[MAXODE];
extern int n_comments;
extern int N_plist;
extern int NCON_START;
extern int NEQ_MIN;
extern int NLINES;
extern int NMarkov;
extern int NODE;
extern int NSYM_START;
extern int NUPAR;
extern char *ode_names[MAXODE];
extern int *plotlist;
extern int PrimeStart;
extern char *save_eqn[MAXLINES];
extern char upar_names[MAXPAR][11];
extern char uvar_names[MAXODE][12];

/* --- Functions --- */
void create_plot_list(void);
int disc(char *string);
int find_char(char *s1, char *s2, int i0, int *i1);
int form_ode_format_eqn(char *buf, int size, int eq);
int form_ode_format_lhs(char *buf, int size, int eq);
void form_ode_load(void);
int search_array(char *old, char *new, int *i1, int *i2, int *flag);
void strip_saveqn(void);
void subsk(char *big, char *new, int k, int flag);

#endif /* XPPAUT_FORM_ODE_H */
