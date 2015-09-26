#ifndef XPPAUT_PARSERSLOW_H
#define XPPAUT_PARSERSLOW_H

#include "xpplim.h"

/* --- Macros --- */
#define MAXARG 20
#define SETVAR(i, x)                                                           \
  do {                                                                         \
    if ((i) < NVAR)                                                            \
      variables[i] = (x);                                                      \
  } while (0)
#define GETVAR(i) ((i) < NVAR ? variables[i] : 0.0)

/* --- Types --- */
typedef struct {
  char name[12];
  char args[MAXARG][11];
  int narg;

  char *def;
  int *rpn;
} UserFunction;

/* --- Data --- */
extern double constants[MAXPAR];
extern int ERROUT;
extern int NCON;
extern int NDELAYS;
extern int NFUN;
extern int NSYM;
extern int NVAR;
extern int RandSeed;
extern UserFunction ufuns[MAXUFUN];
extern double variables[MAXODE1];

/* --- Functions --- */
int add_2d_table(char *name, char *file);
int add_con(const char *name, double value);
int add_expr(const char *expr, int *command, int *length);
int add_file_table(int index, char *file);
int add_form_table(int index, int nn, double xlo, double xhi, char *formula);
int add_kernel(const char *name, double mu, const char *expr);
int add_net_name(int index, const char *name);
int add_table_name(int index, const char *name);
int add_ufun(const char *name, const char *expr, int narg);
int add_ufun_name(char *name, int index, int narg);
int add_ufun_new(int index, int narg, char *rhs, char args[MAXARG][11]);
int add_var(const char *name, double value);
int add_vect_name(int index, char *name);
void convert(const char *source, char *dest);
int do_num(const char *source, char *num, double *value, int *ind);
double evaluate(int *equat);
int find_lookup(char *name);
void fixup_endfun(int *u, int l, int narg);
double get_ivar(int i);
int get_param_index(char *name);
int get_val(char *name, double *value);
int get_var_index(char *name);
void init_rpn(void);
void set_ivar(int i, double value);
void set_new_arg_names(int narg, char args[10][11]);
void set_old_arg_names(int narg);
int set_val(char *name, double value);

#endif /* XPPAUT_PARSERSLOW_H */
