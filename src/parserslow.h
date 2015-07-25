#ifndef XPPAUT_PARSERSLOW_H
#define XPPAUT_PARSERSLOW_H

#include "volterra.h"
#include "xpplim.h"

/* --- Macros --- */
#define MAXARG 20

/* --- Types --- */
typedef struct {
  int narg;
  char args[MAXARG][11];
} UFUN_ARG;

/* --- Data --- */
extern UFUN_ARG ufun_arg[MAXUFUN];

/* --- Functions --- */
void init_rpn(void);
int get_var_index(char *name);
int add_con(char *name, double value);
int add_kernel(char *name, double mu, char *expr);
int add_var(char *junk, double value);
int add_expr(char *expr, int *command, int *length);
int add_net_name(int index, char *name);
int add_vect_name(int index, char *name);
int add_2d_table(char *name, char *file);
int add_file_table(int index, char *file);
int add_table_name(int index, char *name);
int add_form_table(int index, int nn, double xlo, double xhi, char *formula);
void set_old_arg_names(int narg);
void set_new_arg_names(int narg, char args[10][11]);
int add_ufun_name(char *name, int index, int narg);
void fixup_endfun(int *u, int l, int narg);
int add_ufun_new(int index, int narg, char *rhs, char args[MAXARG][11]);
int add_ufun(char *junk, char *expr, int narg);
int find_lookup(char *name);
int get_param_index(char *name);
int get_val(char *name, double *value);
int set_val(char *name, double value);
void set_ivar(int i, double value);
double get_ivar(int i);
int do_num(char *source, char *num, double *value, int *ind);
void convert(char *source, char *dest);

#endif /* XPPAUT_PARSERSLOW_H */
