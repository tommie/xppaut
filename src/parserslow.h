#ifndef XPPAUT_PARSERSLOW_H
#define XPPAUT_PARSERSLOW_H

#include "xpplim.h"
#include "base/vector.h"

/* --- Macros --- */
#define MAXARG 20

/* --- Types --- */
VECTOR_DECLARE(parser_doubles, ParserDoubles, double)

typedef struct {
  char name[10 + 1];
  int len;
  int com;
  int arg;
  int pri;
} ParserSymbol;

VECTOR_DECLARE(parser_symbols, ParserSymbols, ParserSymbol)

typedef struct {
  char name[12];
  char args[MAXARG][11];
  int narg;

  char *def;
  int *rpn;
} UserFunction;

VECTOR_DECLARE(parser_ufuns, ParserUserFunctions, UserFunction)

/* --- Data --- */
extern ParserDoubles constants;
extern int ERROUT;
extern int NDELAYS;
extern ParserSymbols my_symb;
extern ParserUserFunctions ufuns;
extern ParserDoubles variables;

/* --- Functions --- */
int add_2d_table(char *name, char *file);
int add_con(const char *name, double value);
int add_file_table(int index, char *file);
int add_form_table(int index, int nn, double xlo, double xhi, char *formula);
int add_kernel(const char *name, double mu, const char *expr);
int add_net_name(int index, const char *name);
int add_table_name(int index, const char *name);
int add_ufun(const char *name, const char *expr, int narg);
int add_ufun_name(char *name, int narg);
int add_var(const char *name, double value);
void convert(const char *source, char *dest);
int do_num(const char *source, char *num, double *value, int *ind);
double evaluate(int *equat);
int find_lookup(char *name);
double get_ivar(int i);
int get_param_index(char *name);
int get_val(char *name, double *value);
int get_var_index(char *name);
void init_rpn(void);
int parse_expr(const char *expr, int *command, int *length);
int parse_ufun_expr(const UserFunction *ufun, const char *expr, int *command,
                    int *length);
void set_ivar(int i, double value);
int set_ufun_new(int index, int narg, char *rhs, char args[MAXARG][11]);
int set_val(char *name, double value);

#endif /* XPPAUT_PARSERSLOW_H */
