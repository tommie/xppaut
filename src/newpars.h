#ifndef XPPAUT_NEWPARS_H
#define XPPAUT_NEWPARS_H

/* --- Macros --- */
/* VAR_INFO.type values */
#define COMMAND -1
#define FIXED 0
#define FUNCTION 1
#define IC 2
#define MAP 3
#define ODE 4
#define VEQ 5
#define MARKOV_VAR 6
#define AUX_VAR 7
#define TABLE 8
#define SPEC_FUN 11
#define DAE 12
#define DERIVE_PAR 13
#define SOL_VAR 14
#define EXPORT 15
#define PAR_AM 16
#define ONLY 26
#define GROUP 27

#define NAMLEN 10
#define MAXARG 20
#define MAXEXPLEN 1024

/* --- Types --- */
typedef struct var_info {
  char lhs[MAXEXPLEN],rhs[MAXEXPLEN],args[MAXARG][NAMLEN+1];
  int type,nargs;
  double value;
  struct var_info *next,*prev;
} VAR_INFO;

#endif /* XPPAUT_NEWPARS_H */
