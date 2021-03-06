/*****************************
*      PARSER.C              *
*
*
*     parses any algebraic expression
*     and converts to an integer array
*     to be interpreted by the rpe_val
*     function.
*
*     the main data structure is a contiguous
*     list of symbols with their priorities
*     and their symbol value
*
*     on the first pass, the expression is converted to
*     a list of integers without any checking except for
*     valid symbols and for numbers
*
*     next this list of integers is converted to an RPN expression
*     for evaluation.
*
*
*  6/95  stuff added to add names to namelist without compilation
*************************************************************/

#include "config.h"
#include "parserslow.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "comline.h"
#include "delay_handle.h"
#include "expr_builtins.h"
#include "ggets.h"
#include "homsup.h"
#include "markov.h"
#include "simplenet.h"
#include "strutil.h"
#include "tabular.h"
#include "base/ndrand.h"
#include "solver/volterra2.h"

/* --- Macros --- */
/* Command types */
#define FUN2TYPE 1
#define CONTYPE 2
#define VARTYPE 3
#define SVARTYPE 4
#define NETTYPE 6
#define TABTYPE 7
#define USTACKTYPE 8
#define FUN1TYPE 9
#define KERTYPE 10
#define UFUNTYPE 24
#define SCONTYPE 32
#define MAXTYPE 20000000
#define COM(a, b) ((a)*MAXTYPE + (b))

/* Simple commands */
#define ENDEXP 999
#define ENDFUN 998
#define ENDDELAY 996
#define MYIF 995
#define MYELSE 993
#define MYTHEN 994
#define SUMSYM 990
#define ENDSUM 991
#define ENDSHIFT 988
#define NUMSYM 987
#define ENDDELSHFT 986
#define ENDISHIFT 985
#define ENDSET 981
#define INDXCOM 922

/* Tokens */
#define LPAREN 0
#define RPAREN 1
#define COMMA 2
#define MINUS 4
#define NEGATE 9
#define STARTTOK 10
#define ENDTOK 11
#define DELSYM 42
#define NUMTOK 59
#define SHIFTSYM 64
#define DELSHFTSYM 65
#define ISHIFTSYM 67
#define INDX 68
#define FIRST_ARG 73

#define MAXEXPLEN 1024
#define DOUB_EPS 2.23E-15
#define POP stack[--stack_pointer]
#define PUSH(a) do {                            \
    double zippy = (a);                         \
    stack[stack_pointer++] = zippy;             \
  } while (0);

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

/* --- Types --- */
VECTOR_DEFINE(parser_doubles, ParserDoubles, double)
VECTOR_DEFINE(parser_symbols, ParserSymbols, ParserSymbol)
VECTOR_DEFINE(parser_ufuns, ParserUserFunctions, UserFunction)

/* --- Forward Declarations --- */
static int is_uvar(int x);

static void find_name(const char *string, int *index);
static int alg_to_rpn(int *toklist, int *command);
static int unary_sym(int token);
static int binary_sym(int token);
static int make_toks(const char *source, int *my_token);
static void tokeninfo(int tok);
static int find_tok(const ParserSymbols *syms, const char *source);

static double eval_rpn(int *equat);

/* --- Data --- */
int NDELAYS = 0;
ParserDoubles constants = VECTOR_INIT;
ParserDoubles variables = VECTOR_INIT;
ParserSymbols my_symb = VECTOR_INIT;
ParserUserFunctions ufuns = VECTOR_INIT;

static int SumIndex = 1;
static int shift_index = -1;

/* FIXXX */
static int stack_pointer, uptr;
static double stack[200], ustack[200];

static const ParserSymbol BUILTIN_SYMBOLS[] = {
    {"(", 1, ENDEXP, 0, 1}, /*  0   */
    {")", 1, ENDEXP, 0, 2},
    {",", 1, ENDEXP, 0, 3},
    {"+", 1, COM(FUN2TYPE, 0), 0, 4},
    {"-", 1, COM(FUN2TYPE, 1), 0, 4},
    {"*", 1, COM(FUN2TYPE, 2), 0, 6},
    {"/", 1, COM(FUN2TYPE, 3), 0, 6},
    {"^", 1, COM(FUN2TYPE, 5), 0, 7},
    {"**", 2, COM(FUN2TYPE, 5), 0, 7},
    {"~", 1, COM(FUN1TYPE, 14), 0, 6},
    {"START", 5, -1, 0, 0}, /* 10  */
    {"END", 3, ENDEXP, 0, -1},
    {"ATAN2", 5, COM(FUN2TYPE, 4), 2, 10},
    {"MAX", 3, COM(FUN2TYPE, 6), 2, 10},
    {"MIN", 3, COM(FUN2TYPE, 7), 2, 10},
    {"SIN", 3, COM(FUN1TYPE, 0), 0, 10},
    {"COS", 3, COM(FUN1TYPE, 1), 0, 10},
    {"TAN", 3, COM(FUN1TYPE, 2), 0, 10},
    {"ASIN", 4, COM(FUN1TYPE, 3), 0, 10},
    {"ACOS", 4, COM(FUN1TYPE, 4), 0, 10},
    {"ATAN", 4, COM(FUN1TYPE, 5), 0, 10}, /* 20  */
    {"SINH", 4, COM(FUN1TYPE, 6), 0, 10},
    {"TANH", 4, COM(FUN1TYPE, 7), 0, 10},
    {"COSH", 4, COM(FUN1TYPE, 8), 0, 10},
    {"ABS", 3, COM(FUN1TYPE, 9), 0, 10},
    {"EXP", 3, COM(FUN1TYPE, 10), 0, 10},
    {"LN", 2, COM(FUN1TYPE, 11), 0, 10},
    {"LOG", 3, COM(FUN1TYPE, 11), 0, 10},
    {"LOG10", 5, COM(FUN1TYPE, 12), 0, 10},
    {"SQRT", 4, COM(FUN1TYPE, 13), 0, 10},
    {"HEAV", 4, COM(FUN1TYPE, 16), 0, 10}, /*  30 */
    {"SIGN", 4, COM(FUN1TYPE, 17), 0, 10},
    {"#$%1", 4, COM(USTACKTYPE, 0), 0, 10},
    {"#$%2", 4, COM(USTACKTYPE, 1), 0, 10},
    {"#$%3", 4, COM(USTACKTYPE, 2), 0, 10},
    {"#$%4", 4, COM(USTACKTYPE, 3), 0, 10},
    {"#$%5", 4, COM(USTACKTYPE, 4), 0, 10},
    {"#$%6", 4, COM(USTACKTYPE, 5), 0, 10},
    {"#$%7", 4, COM(USTACKTYPE, 6), 0, 10},
    {"#$%8", 4, COM(USTACKTYPE, 7), 0, 10},
    {"FLR", 3, COM(FUN1TYPE, 18), 0, 10}, /*  40 */
    {"MOD", 3, COM(FUN2TYPE, 8), 2, 10},  /*  41 */
    {"DELAY", 5, ENDDELAY, 2, 10},
    /*  42 */                             /*  Delay symbol */
    {"RAN", 3, COM(FUN1TYPE, 19), 1, 10}, /* 43 */
    {"&", 1, COM(FUN2TYPE, 9), 0, 6},     /* logical stuff  */
    {"|", 1, COM(FUN2TYPE, 10), 0, 4},
    {">", 1, COM(FUN2TYPE, 11), 0, 7},
    {"<", 1, COM(FUN2TYPE, 12), 0, 7},
    {"==", 2, COM(FUN2TYPE, 13), 0, 7},
    {">=", 2, COM(FUN2TYPE, 14), 0, 7},
    {"<=", 2, COM(FUN2TYPE, 15), 0, 7}, /*50 */
    {"IF", 2, MYIF, 1, 10},
    {"THEN", 4, MYTHEN, 1, 10},
    {"ELSE", 4, MYELSE, 1, 10},
    {"!=", 2, COM(FUN2TYPE, 16), 0, 7},
    {"NOT", 3, COM(FUN1TYPE, 20), 0, 6},
    {"NORMAL", 6, COM(FUN2TYPE, 17), 2, 10},  /* returns normally dist number */
    {"BESSELJ", 7, COM(FUN2TYPE, 18), 2, 10}, /* Bessel J   */
    {"BESSELY", 7, COM(FUN2TYPE, 19), 2, 10}, /* Bessel Y */
    {"NXXQQ", 5, NUMSYM, 0, 10},
    {"ERF", 3, COM(FUN1TYPE, 21), 0, 10}, /* 60 */
    {"ERFC", 4, COM(FUN1TYPE, 22), 0, 10},
    {"SUM", 3, SUMSYM, 2, 10},
    {"OF", 2, ENDSUM, 0, 10},
    {"SHIFT", 5, ENDSHIFT, 2, 10},
    {"DEL_SHFT", 8, ENDDELSHFT, 3, 10}, /* 65 */
    {"HOM_BCS", 7, COM(FUN1TYPE, 23), 0, 10},
    {"ISHIFT", 6, ENDISHIFT, 2, 10}, /* 67 */
    {"@", 1, INDXCOM, 0, 10},        /*68 */
    {"]", 1, ENDSHIFT, 0, 10},
    {"[", 1, ENDSHIFT, 0, 10},                /*70 */
    {"POISSON", 7, COM(FUN1TYPE, 24), 0, 10}, /* 71 */
    {"SET", 3, ENDSET, 3, 10},                /* 72 */
    {"ARG1", 4, COM(USTACKTYPE, 0), 0, 10},   /*  FIXXX ????  */
    {"ARG2", 4, COM(USTACKTYPE, 1), 0, 10},
    {"ARG3", 4, COM(USTACKTYPE, 2), 0, 10},
    {"ARG4", 4, COM(USTACKTYPE, 3), 0, 10},
    {"ARG5", 4, COM(USTACKTYPE, 4), 0, 10},
    {"ARG6", 4, COM(USTACKTYPE, 5), 0, 10},
    {"ARG7", 4, COM(USTACKTYPE, 6), 0, 10},
    {"ARG8", 4, COM(USTACKTYPE, 7), 0, 10},
    {"ARG9", 4, COM(USTACKTYPE, 8), 0, 10},
    {"ARG10", 5, COM(USTACKTYPE, 9), 0, 10},
    {"ARG11", 5, COM(USTACKTYPE, 10), 0, 10},
    {"ARG12", 5, COM(USTACKTYPE, 11), 0, 10},
    {"ARG13", 5, COM(USTACKTYPE, 12), 0, 10},
    {"ARG14", 5, COM(USTACKTYPE, 13), 0, 10},
    {"ARG15", 5, COM(USTACKTYPE, 14), 0, 10},
    {"ARG16", 5, COM(USTACKTYPE, 15), 0, 10},
    {"ARG17", 5, COM(USTACKTYPE, 16), 0, 10},
    {"ARG18", 5, COM(USTACKTYPE, 17), 0, 10},
    {"ARG19", 5, COM(USTACKTYPE, 18), 0, 10},
    {"ARG20", 5, COM(USTACKTYPE, 19), 0, 10},
    {"BESSELI", 7, COM(FUN2TYPE, 20), 2, 10}, /* Bessel I  # 93 */
    {"LGAMMA", 6, COM(FUN1TYPE, 25), 1, 10}   /* Log Gamma  #94 */
};
static const size_t NUM_BUILTIN_SYMBOLS = sizeof(BUILTIN_SYMBOLS) / sizeof(*BUILTIN_SYMBOLS);

/*************************
  RPN COMPILER           *
**************************/

void init_rpn(void) {
  parser_doubles_init(&constants, 0);
  parser_ufuns_init(&ufuns, 0);
  parser_doubles_init(&variables, 0);
  parser_symbols_init(&my_symb, NUM_BUILTIN_SYMBOLS);
  memcpy(parser_symbols_insert(&my_symb, 0, NUM_BUILTIN_SYMBOLS),
         BUILTIN_SYMBOLS, NUM_BUILTIN_SYMBOLS * sizeof(*BUILTIN_SYMBOLS));

  add_con("PI", M_PI);

  add_con("I'", 0.0);
  SumIndex = constants.len - 1;

  add_con("", 0.0);
  shift_index = constants.len - 1;

  /*   This is going to be for interacting with the
       animator */
  add_con("mouse_x", 0.0);
  add_con("mouse_y", 0.0);
  add_con("mouse_vx", 0.0);
  add_con("mouse_vy", 0.0);

  /* end animator stuff */
  /*  add_con("c___1",0.0);
      add_con("c___2",0.0);
      add_con("c___3",0.0); */

  init_table();
}

int duplicate_name(const char *name) {
  int i;
  find_name(name, &i);
  if (i >= 0) {
    plintf("%s is a duplicate name\n", name);
    return (1);
  }
  return (0);
}

/**
 * Add a symbol to the lookup table.
 *
 * @param name is the symbol name.
 * @param pri the priority of the symbol.
 * @param arg the number of arguments.
 * @param com the RPN command.
 * @return the symbol index, and negative on error.
 */
static int add_symbol(const char *name, int pri, int arg, int com) {
  int len;
  char cname[100];

  if (duplicate_name(name))
    return -1;

  convert(name, cname);
  len = strlen(cname);
  if (len < 1) {
    plintf("Empty parameter - remove spaces\n");
    return -1;
  }

  ParserSymbol *p = parser_symbols_append(&my_symb);

  if (!p)
    return -1;

  if (len > sizeof(p->name) - 1)
    len = sizeof(p->name) - 1;

  strncpy(p->name, cname, len);
  p->name[len] = '\0';
  p->len = len;
  p->pri = pri;
  p->arg = arg;
  p->com = com;

  return my_symb.len - 1;
}

int add_con(const char *name, double value) {
  double *p = parser_doubles_append(&constants);

  if (!p) return 1;

  *p = value;

  if (add_symbol(name, 10, 0, COM(CONTYPE, constants.len - 1)) < 0) {
    parser_doubles_remove(&constants, constants.len - 1, 1);
    return 1;
  }

  return 0;
}

int get_var_index(char *name) {
  int type, com;
  find_name(name, &type);
  if (type < 0)
    return -1;
  com = my_symb.elems[type].com;
  if (is_uvar(com)) {
    return (com % MAXTYPE);
  }
  return (-1);
}

int get_type(int index) { return (my_symb.elems[index].com); }

int add_kernel(const char *name, double mu, const char *expr) {
  int ki = volterra2_add_kernel(expr, mu);
  if (ki < 0)
    return 1;

  if (add_symbol(name, 10, 0, COM(KERTYPE, ki)) < 0) {
    volterra2_remove_kernel(ki);
    return 1;
  }

  return 0;
}

int add_var(const char *name, double value) {
  double *p = parser_doubles_append(&variables);

  if (!p) return 1;

  *p = value;

  if (add_symbol(name, 10, 0, COM(VARTYPE, variables.len - 1)) < 0) {
    parser_doubles_remove(&variables, variables.len - 1, 1);
    return 1;
  }

  return 0;
}

int parse_expr(const char *expr, int *command, int *length) {
  char dest[1024];
  int my_token[1024];
  int err, i;
  convert(expr, dest);
  /* plintf(" Making token ...\n");  */
  err = make_toks(dest, my_token);

  /*  i=0;
    while(1){
    plintf(" %d %d \n",i,my_token[i]);
    if(my_token[i]==ENDTOK)break;
    i++;
  } */
  if (err != 0)
    return (1);
  err = alg_to_rpn(my_token, command);
  if (err != 0)
    return (1);
  i = 0;
  while (command[i] != ENDEXP)
    i++;
  *length = i + 1;
  /*  for(i=0;i<*length;i++)printf("%d \n",command[i]);  */
  return (0);
}

static void fixup_endfun(int *u, int l, int narg) {
  u[l - 1] = ENDFUN;
  u[l] = narg;
  u[l + 1] = ENDEXP;
}

int parse_ufun_expr(const UserFunction *ufun, const char *expr, int *command,
                    int *length) {
  int err;

  /* Set the user-function's argument names in my_symb. */
  for (int i = 0; i < ufun->narg; i++) {
    strcpy(my_symb.elems[FIRST_ARG + i].name, ufun->args[i]);
    my_symb.elems[FIRST_ARG + i].len = strlen(ufun->args[i]);
  }

  err = parse_expr(expr, command, length);

  /* Switch back to ARG* names. */
  for (int i = 0; i < ufun->narg; i++) {
    sprintf(my_symb.elems[FIRST_ARG + i].name, "ARG%d", i + 1);
    my_symb.elems[FIRST_ARG + i].len = strlen(my_symb.elems[FIRST_ARG + i].name);
  }

  if (!err)
    fixup_endfun(command, *length, ufun->narg);

  return err;
}

int add_net_name(int index, const char *name) {
  if (add_symbol(name, 10, 1, COM(NETTYPE, index)) < 0)
    return 1;

  return 0;
}

int add_2d_table(char *name, char *file) {
  plintf(" TWO D NOT HERE YET \n");
  return (1);
}

/**
 * Remove non-printable ASCII-characters and quotes.
 * Remove anything after the first quoted string.
 */
static void clean_path(char *buf, int size, const char *path) {
  int inquotes = 0;

  for (; *path && size > 1; ++path) {
    if (*path == '"') {
      if (inquotes)
        break;
      else
        inquotes = 1;
    } else if (*path > 31 && *path < 127) {
      *buf = *path;
      ++buf;
      --size;
    }
  }
  *buf = '\0';
}

int add_file_table(int index, char *file) {
  char file2[XPP_MAX_NAME];
  clean_path(file2, sizeof(file2), file);
  if (load_table(file2, index) == 0) {
    plintf("Problem with creating table !!\n");
    return (1);
  }

  return (0);
}

int add_table_name(int index, const char *name) {
  set_table_name(name, index);

  if (add_symbol(name, 10, 1, COM(TABTYPE, index)) < 0)
    return 1;

  return 0;
}

int add_form_table(int index, int nn, double xlo, double xhi, char *formula) {
  if (create_fun_table(nn, xlo, xhi, formula, index) == 0) {
    plintf("Problem with creating table !!\n");
    return (1);
  }
  return (0);
}

int add_ufun_name(char *name, int narg) {
  UserFunction *p = parser_ufuns_append(&ufuns);

  if (!p) return 1;

  strncpy(p->name, name, sizeof(p->name));
  p->name[sizeof(p->name) - 1] = '\0';

  if (add_symbol(name, 10, narg, COM(UFUNTYPE, ufuns.len - 1)) < 0) {
    parser_ufuns_remove(&ufuns, ufuns.len - 1, 1);
    return 1;
  }

  return 0;
}

int set_ufun_new(int index, int narg, char *rhs, char args[MAXARG][11]) {
  if (narg > MAXARG) {
    plintf("Maximal arguments exceeded \n");
    return 1;
  }

  ufuns.elems[index].narg = narg;
  for (int i = 0; i < narg; i++)
    strcpy(ufuns.elems[index].args[i], args[i]);

  if (parser_set_ufun_rhs(index, rhs)) {
    plintf("ERROR IN FUNCTION DEFINITION\n");
    return 1;
  }

  return 0;
}

int parser_set_ufun_rhs(int index, const char *rhs) {
  UserFunction *uf = &ufuns.elems[index];
  int len = 0;

  char *def = strdup(rhs);
  int *command = malloc(1024);

  if (!def || !command) {
    plintf("not enough memory!!\n");
    goto err1;
  }

  if (parse_ufun_expr(uf, rhs, command, &len))
    goto err1;

  uf->def = def;
  uf->rpn = command;

  return 0;

 err1:
  free(command);
  free(def);

  return 1;
}

int add_ufun(const char *name, const char *expr, int narg) {
  int i, l;
  int end;
  UserFunction *p = parser_ufuns_append(&ufuns);

  if (!p) return 1;

  if ((p->rpn = malloc(1024)) == NULL) {
    plintf("not enough memory!!\n");
    return 1;
  }
  if ((p->def = malloc(MAXEXPLEN)) == NULL) {
    free(p->rpn);
    parser_ufuns_remove(&ufuns, ufuns.len - 1, 1);
    plintf("not enough memory!!\n");
    return 1;
  }

  strcpy(p->def, expr);
  l = strlen(p->def);
  p->def[l - 1] = '\0';
  strcpy(p->name, name);
  p->narg = narg;
  for (i = 0; i < narg; i++) {
    sprintf(p->args[i], "ARG%d", i + 1);
  }

  if (parse_expr(expr, p->rpn, &end)) {
    free(p->def);
    free(p->rpn);
    parser_ufuns_remove(&ufuns, ufuns.len - 1, 1);
    plintf("ERROR IN FUNCTION DEFINITION\n");
    return 1;
  }

  fixup_endfun(p->rpn, end, narg);

  if (add_symbol(name, 10, narg, COM(UFUNTYPE, ufuns.len - 1)) < 0) {
    free(p->def);
    free(p->rpn);
    parser_ufuns_remove(&ufuns, ufuns.len - 1, 1);
    return 1;
  }

  return 0;
}

int check_num(int *tok, double value) {
  int bob, in, i;
  /*int m;*/
  for (i = 0; i < my_symb.len; i++) {
    if (strncmp(my_symb.elems[i].name, "NUM##", 5) == 0) {
      bob = my_symb.elems[i].com;
      in = bob % MAXTYPE;
      /*m=bob/MAXTYPE;*/
      if (constants.elems[in] == value) {
        *tok = i;
        return (1);
      }
    }
  }
  return (0);
}

static int is_ufun(int x) { return x / MAXTYPE == UFUNTYPE; }
static int is_ucon(int x) { return x / MAXTYPE == CONTYPE; }
static int is_uvar(int x) { return x / MAXTYPE == VARTYPE; }
static int isvar(int y) { return y == VARTYPE; }
static int iscnst(int y) { return y == CONTYPE; }
static int isker(int y) { return y == KERTYPE; }
static int is_lookup(int x) { return x / MAXTYPE == TABTYPE; }

int find_lookup(char *name) {
  int index, com;
  find_name(name, &index);
  if (index == -1)
    return (-1);
  com = my_symb.elems[index].com;
  if (is_lookup(com))
    return (com % MAXTYPE);
  return (-1);
}

static void find_name(const char *string, int *index) {
  char junk[100];
  int i, len;
  convert(string, junk);
  len = strlen(junk);
  for (i = 0; i < my_symb.len; i++) {
    if (len == my_symb.elems[i].len)
      if (strncmp(my_symb.elems[i].name, junk, len) == 0)
        break;
  }
  if (i < my_symb.len)
    *index = i;
  else
    *index = -1;
}

int get_param_index(char *name) {
  int type, com;
  find_name(name, &type);
  if (type < 0)
    return (-1);
  com = my_symb.elems[type].com;
  if (is_ucon(com)) {
    return (com % MAXTYPE);
  }
  return (-1);
}

int get_val(char *name, double *value) {
  int type, com;
  *value = 0.0;
  find_name(name, &type);
  if (type < 0)
    return (0);
  com = my_symb.elems[type].com;
  if (is_ucon(com)) {
    *value = constants.elems[com % MAXTYPE];
    return (1);
  }
  if (is_uvar(com)) {
    *value = variables.elems[com % MAXTYPE];
    return (1);
  }
  return (0);
}

int set_val(char *name, double value) {
  int type, com;
  find_name(name, &type);
  if (type < 0)
    return (0);
  com = my_symb.elems[type].com;
  if (is_ucon(com)) {
    constants.elems[com % MAXTYPE] = value;

    return (1);
  }
  if (is_uvar(com)) {
    variables.elems[com % MAXTYPE] = value;
    return (1);
  }
  return (0);
}

void set_ivar(int i, double value) {
  if (i < variables.len)
    variables.elems[i] = value;
}

double get_ivar(int i) {
  return i < variables.len ? variables.elems[i] : 0.0;
}

static int alg_to_rpn(int *toklist, int *command) {
  int tokstak[500], comptr = 0, stakptr = 0, lstptr = 0, temp;
  int ncomma = 0;
  int loopstk[100];
  int loopptr = 0;
  int nif = 0, nthen = 0, nelse = 0;

  tokstak[0] = STARTTOK;
  stakptr = 1;
  for (;;) {
    int newtok = toklist[lstptr++];
    int oldtok;

    switch (newtok) {
    case DELSYM:
      temp = my_symb.elems[toklist[lstptr + 1]].com;
      if (!is_uvar(temp)) {
        printf("Illegal use of DELAY \n");
        return 1;
      }

      /* ram -- is this right? not sure I understand what was happening here */
      /* create a temporary sybol */
      my_symb.elems[shift_index].com = COM(SVARTYPE, temp % MAXTYPE);
      NDELAYS++;
      toklist[lstptr + 1] = shift_index;
      break;

    case DELSHFTSYM:
      temp = my_symb.elems[toklist[lstptr + 1]].com;
      if (!is_uvar(temp)) {
        printf("Illegal use of DELAY Shift \n");
        return 1;
      }

      /* ram -- same issue */
      /* create a temporary sybol */
      my_symb.elems[shift_index].com = COM(SVARTYPE, temp % MAXTYPE);
      NDELAYS++;
      toklist[lstptr + 1] = shift_index;
      break;

    case SHIFTSYM:
    case ISHIFTSYM:
      temp = my_symb.elems[toklist[lstptr + 1]].com;
      if (!is_uvar(temp) && !is_ucon(temp)) {
        printf("Illegal use of SHIFT \n");
        return 1;
      }

      /* ram -- same issue */
      /* create a temporary sybol */
      if (is_uvar(temp))
        my_symb.elems[shift_index].com = COM(SVARTYPE, temp % MAXTYPE);
      if (is_ucon(temp))
        my_symb.elems[shift_index].com = COM(SCONTYPE, temp % MAXTYPE);
      toklist[lstptr + 1] = shift_index;
      break;
    }

  next:
    oldtok = tokstak[stakptr - 1];

    if (newtok == ENDTOK && oldtok == STARTTOK)
      break;

    if (newtok == LPAREN) {
      tokstak[stakptr++] = LPAREN;
      continue;
    }
    if (newtok == RPAREN) {
      switch (oldtok) {
      case LPAREN:
        stakptr--;
        continue;
      case COMMA:
        stakptr--;
        ncomma++;
        goto next;
      }
    }
    if (newtok == COMMA && oldtok == COMMA) {
      tokstak[stakptr++] = COMMA;
      continue;
    }

    if (my_symb.elems[oldtok].pri >= my_symb.elems[newtok].pri) {
      int my_com = my_symb.elems[oldtok].com;
      command[comptr] = my_com;
      comptr++;

      switch (my_com) {
      case NUMSYM:
        /* NUMSYM stores the encoded value as two extra integers. */
        stakptr--;
        command[comptr] = tokstak[stakptr - 1];
        comptr++;
        stakptr--;
        command[comptr] = tokstak[stakptr - 1];
        comptr++;
        break;

      case SUMSYM:
        /* SUMSYM has a jump location to be filled in. */
        loopstk[loopptr] = comptr;
        comptr++;
        loopptr++;
        ncomma -= 1;
        break;

      case ENDSUM:
        /* Fill in SUMSYM jump location. */
        loopptr--;
        command[loopstk[loopptr]] = comptr - loopstk[loopptr] - 1;
        break;

      case MYIF:
        /* MYIF has a jump location to be filled in. */
        loopstk[loopptr] = comptr;
        comptr++;
        loopptr++;
        nif++;
        break;

      case MYTHEN:
        /* Fill in the MYIF jump location. */
        loopptr--;
        command[loopstk[loopptr]] = comptr - loopstk[loopptr]; /* -1 is old */

        /* MYTHEN has a jump location to be filled in. */
        loopstk[loopptr] = comptr;
        loopptr++;
        comptr++;
        nthen++;
        break;

      case MYELSE:
        /* Fill in the MYTHEN jump location. */
        loopptr--;
        command[loopstk[loopptr]] = comptr - loopstk[loopptr] - 1;
        nelse++;
        break;

      case ENDDELAY:
      case ENDSHIFT:
      case ENDISHIFT:
        ncomma -= 1;
        break;

      case ENDDELSHFT:
        ncomma -= 2;
        break;

      default:
        if (my_com / MAXTYPE == FUN2TYPE && my_symb.elems[oldtok].arg == 2)
          ncomma--;

        /*    CHECK FOR USER FUNCTION       */
        if (is_ufun(my_com)) {
          int my_arg = my_symb.elems[oldtok].arg;
          command[comptr] = my_arg;
          comptr++;
          ncomma += 1 - my_arg;
        }
      }

      stakptr--;
      goto next;
    }

    if (newtok == NUMTOK) {
      /* NUMTOK stores the encoded value as two extra integers. */
      tokstak[stakptr++] = toklist[lstptr++];
      tokstak[stakptr++] = toklist[lstptr++];
    }
    tokstak[stakptr++] = newtok;
  }

  if (ncomma != 0) {
    plintf("Illegal number of arguments\n");
    return 1;
  }
  if (nif != nelse || nif != nthen) {
    plintf("If statement missing ELSE or THEN \n");
    return 1;
  }
  command[comptr] = my_symb.elems[ENDTOK].com;

  return 0;
}

static void show_where(const char *string, int index) {
  char junk[MAXEXPLEN];
  int i;
  /* exit(-1); */
  for (i = 0; i < index; i++)
    junk[i] = ' ';
  junk[index] = '^';
  junk[index + 1] = 0;
  plintf("%s\n%s\n", string, junk);
}

/* functions should have ( after them  */
int function_sym(int token) {
  int com = my_symb.elems[token].com;
  int i1 = com / MAXTYPE;

  if (i1 == FUN1TYPE && !unary_sym(token))
    return (1); /* single variable functions */
  if (i1 == FUN2TYPE && !binary_sym(token))
    return (1); /* two-variable function */
  /* ram this was: if(i1==UFUN||i1==7||i1==6||i1==5)return(1); recall: 5 was bad
   */
  if (i1 == UFUNTYPE || i1 == TABTYPE || i1 == NETTYPE)
    return (1);
  if (token == DELSHFTSYM || token == DELSYM || token == SHIFTSYM ||
      token == ISHIFTSYM || com == MYIF || com == MYTHEN || com == MYELSE ||
      com == SUMSYM || com == ENDSUM)
    return (1);
  return (0);
}

static int unary_sym(int token) {
  /* ram: these are tokens not byte code, so no change here? */
  if (token == 9 || token == 55)
    return (1);
  return (0);
}

static int binary_sym(int token) {
  /* ram: these are tokens not byte code, so no change here? */
  if (token > 2 && token < 9)
    return (1);
  if (token > 43 && token < 51)
    return (1);
  if (token == 54)
    return (1);
  return (0);
}

int pure_number(int token) {
  int com = my_symb.elems[token].com;
  int i1 = com / MAXTYPE;
  /* !! */ if (token == NUMTOK || isvar(i1) || iscnst(i1) || isker(i1) ||
               i1 == USTACKTYPE || token == INDX)
    return (1);
  return (0);
}

int gives_number(int token) {
  int com = my_symb.elems[token].com;
  int i1 = com / MAXTYPE;
  if (token == INDX)
    return (1);
  if (token == NUMTOK)
    return (1);
  if (i1 == FUN1TYPE && !unary_sym(token))
    return (1); /* single variable functions */
  if (i1 == FUN2TYPE && !binary_sym(token))
    return (1); /* two-variable function */
                /* !! */
                /* ram: 5 issue; was
                 * if(i1==8||isvar(i1)||iscnst(i1)||i1==7||i1==6||i1==5||isker(i1)||i1==UFUN)return(1);
                 */
  if (i1 == USTACKTYPE || isvar(i1) || iscnst(i1) || i1 == TABTYPE ||
      i1 == NETTYPE || isker(i1) || i1 == UFUNTYPE)
    return (1);
  if (com == MYIF || token == DELSHFTSYM || token == DELSYM ||
      token == SHIFTSYM || token == ISHIFTSYM || com == SUMSYM)
    return (1);
  return (0);
}

/* 1 is BAD!   */
int check_syntax(int oldtoken, int newtoken) {
  int com2 = my_symb.elems[newtoken].com;

  /* if the first symbol or (  or binary symbol then must be unary symbol or
     something that returns a number or another (
  */

  if (unary_sym(oldtoken) || oldtoken == COMMA || oldtoken == STARTTOK ||
      oldtoken == LPAREN || binary_sym(oldtoken)) {
    if (unary_sym(newtoken) || gives_number(newtoken) || newtoken == LPAREN)
      return (0);
    return (1);
  }

  /* if this is a regular function, then better have (
  */

  if (function_sym(oldtoken)) {
    if (newtoken == LPAREN)
      return (0);
    return (1);
  }

  /* if we have a constant or variable or ) or kernel then better
     have binary symbol or "then" or "else" as next symbol
  */

  if (pure_number(oldtoken)) {
    if (binary_sym(newtoken) || newtoken == RPAREN || newtoken == COMMA ||
        newtoken == ENDTOK)
      return (0);

    return (1);
  }

  if (oldtoken == RPAREN) {
    if (binary_sym(newtoken) || newtoken == RPAREN || newtoken == COMMA ||
        newtoken == ENDTOK)
      return (0);
    if (com2 == MYELSE || com2 == MYTHEN || com2 == ENDSUM)
      return (0);

    return (1);
  }

  plintf("Bad token %d \n", oldtoken);
  return (1);
}

/******************************
*    PARSER                   *
******************************/

static int make_toks(const char *source, int *my_token) {
  int old_tok = STARTTOK, tok_in = 0;
  int index = 0, nparen = 0, lastindex = 0;

  while (source[index] != '\0') {
    int token = find_tok(&my_symb, source + index);

    lastindex = index;
    if (token != my_symb.len)
      index += my_symb.elems[token].len;

    if ((token == MINUS) &&
        ((old_tok == STARTTOK) || (old_tok == COMMA) || (old_tok == LPAREN)))
      token = NEGATE;
    else if (token == LPAREN)
      ++nparen;
    else if (token == RPAREN)
      --nparen;

    if (token == my_symb.len) {
      char num[40];
      double value;
      /*  WARNING  -- ASSUMES 32 bit int  and 64 bit double  */
      union {
        struct {
          int int1;
          int int2;
        } pieces;
        struct {
          double z;
        } num;
      } encoder;

      if (do_num(source, num, &value, &index)) {
        plintf("illegal expression: %s\n", num);
        show_where(source, index);
        return (1);
      }
      /*    new code        3/95      */
      encoder.num.z = value;
      my_token[tok_in++] = NUMTOK;
      my_token[tok_in++] = encoder.pieces.int1;
      my_token[tok_in++] = encoder.pieces.int2;
      if (check_syntax(old_tok, NUMTOK) == 1) {
        plintf("Illegal syntax \n");
        show_where(source, lastindex);
        return (1);
      }
      old_tok = NUMTOK;
    } else {
      my_token[tok_in++] = token;
      if (check_syntax(old_tok, token) == 1) {
        plintf("Illegal syntax (Ref:%d %d) \n", old_tok, token);
        show_where(source, lastindex);
        tokeninfo(old_tok);
        tokeninfo(token);
        return (1);
      }

      old_tok = token;
    }
  }

  my_token[tok_in++] = ENDTOK;
  if (check_syntax(old_tok, ENDTOK) == 1) {
    plintf("Premature end of expression \n");
    show_where(source, lastindex);
    return (1);
  }
  if (nparen != 0) {
    plintf("parentheses don't match\n");
    return (1);
  }
  return (0);
}

static void tokeninfo(int tok) {
  plintf(" %s %d %d %d %d \n", my_symb.elems[tok].name, my_symb.elems[tok].len,
         my_symb.elems[tok].com, my_symb.elems[tok].arg,
         my_symb.elems[tok].pri);
}

int do_num(const char *source, char *num, double *value, int *ind) {
  int j = 0, i = *ind, error = 0;
  int ndec = 0, nexp = 0, ndig = 0;
  char ch, oldch;
  oldch = '\0';
  *value = 0.0;
  while (1) {
    ch = source[i];
    if (((ch == '+') || (ch == '-')) && (oldch != 'E'))
      break;
    if ((ch == '*') || (ch == '^') || (ch == '/') || (ch == ',') ||
        (ch == ')') || (ch == '\0') || (ch == '|') || (ch == '>') ||
        (ch == '<') || (ch == '&') || (ch == '='))
      break;
    if ((ch == 'E') || (ch == '.') || (ch == '+') || (ch == '-') ||
        isdigit(ch)) {
      if (isdigit(ch))
        ndig++;
      switch (ch) {
      case 'E':
        nexp++;
        if ((nexp == 2) || (ndig == 0))
          goto err;
        break;
      case '.':
        ndec++;
        if ((ndec == 2) || (nexp == 1))
          goto err;
        break;
      }
      num[j] = ch;
      j++;
      i++;
      oldch = ch;
    } else {
    err:
      num[j] = ch;
      j++;
      error = 1;
      break;
    }
  }
  num[j] = '\0';
  if (error == 0)
    *value = atof(num);
  *ind = i;
  return (error);
}

void convert(const char *source, char *dest) {
  char ch;
  int i = 0, j = 0;
  while (1) {
    ch = source[i];
    if (!isspace(ch))
      dest[j++] = ch;
    i++;
    if (ch == '\0')
      break;
  }
  strupr(dest);
}

/**
 * Find the longest match for beginning of source in syms.
 *
 * @param syms symbols to look up in.
 * @param source the string to parse.
 * @return an index into my_symb.
 */
static int find_tok(const ParserSymbols *syms, const char *source) {
  int maxlen = 0;
  int tok = syms->len;

  for (int k = 0; k < syms->len; k++) {
    int symlen = syms->elems[k].len;
    if (symlen <= maxlen)
      continue;

    if (strncmp(source, syms->elems[k].name, symlen))
      continue;

    tok = k;
    maxlen = symlen;
  }

  return tok;
}

/*********************************************
          FANCY DELAY HERE                   *-------------------------<<<
*********************************************/

static double do_shift(double shift, double variable) {
  int it, in;
  int i = (int)(variable), ish = (int)shift;

  if (i < 0)
    return (0.0);
  it = i / MAXTYPE;
  in = (i % MAXTYPE) + ish;
  switch (it) {
  case CONTYPE:
    if (in > constants.len)
      return 0.0;
    else
      return constants.elems[in];
    break;
  case VARTYPE:
    if (in > MAXODE)
      return 0.0;
    else
      return variables.elems[in];
  default:
    plintf("This can't happen: Invalid symbol index for SHIFT: i = %d\n", i);
    return 0.0;
  }
}

static double do_ishift(double shift, double variable) {
  return variable + shift;
}

static double do_delay_shift(double delay, double shift, double variable) {
  int in;
  int i = (int)(variable), ish = (int)shift;
  if (i < 0)
    return (0.0);
  in = (i % MAXTYPE) + ish;

  if (in > MAXODE)
    return 0.0;

  if (del_stab_flag > 0) {
    if (DelayFlag && delay > 0.0)
      return (get_delay(in - 1, delay));
    return (variables.elems[in]);
  }

  return (delay_stab_eval(delay, in));
}

static double do_delay(double delay, double i) {
  int variable;
  /* ram - this was a little weird, since i is a double... except I think it's
   * secretely an integer */
  variable = ((int)i) % MAXTYPE;

  if (del_stab_flag > 0) {
    if (DelayFlag && delay > 0.0) {
      /* printf("do_delay for var #%d, delay %f\n", variable-1, delay); */
      return (get_delay(variable - 1, delay));
    }
    return (variables.elems[variable]);
  }

  return (delay_stab_eval(delay, (int)variable));
}

double evaluate(int *equat) {
  uptr = 0;
  stack_pointer = 0;
  return (eval_rpn(equat));
}

static double eval_rpn(int *equat) {
  int i;
  double temx, temy, temz;
  union /*  WARNING  -- ASSUMES 32 bit int  and 64 bit double  */
  {
    struct {
      int int1;
      int int2;
    } pieces;
    struct {
      double z;
    } num;
  } encoder;

  while ((i = *equat++) != ENDEXP) {
    switch (i) {
    case NUMSYM:
      encoder.pieces.int2 = *equat++;
      encoder.pieces.int1 = *equat++;
      PUSH(encoder.num.z);
      break;
    case ENDFUN:
      i = *equat++;
      uptr -= i;
      break;
    case MYIF: {
      int ijmp;

      temx = POP;
      ijmp = *equat++;
      if (temx == 0.0)
        equat += ijmp;
      break;
    }
    case MYTHEN: {
      int ijmp = *equat++;

      equat += ijmp;
      break;
    }
    case MYELSE:
      break;
    case ENDDELSHFT:
      temx = POP;
      temy = POP;
      temz = POP;
      PUSH(do_delay_shift(temx, temy, temz));
      break;
    case ENDDELAY:
      temx = POP;
      temy = POP;
      PUSH(do_delay(temx, temy));
      break;
    case ENDSHIFT:
      temx = POP;
      temy = POP;
      PUSH(do_shift(temx, temy));
      break;
    case ENDISHIFT:
      temx = POP;
      temy = POP;
      PUSH(do_ishift(temx, temy));
      break;
    case SUMSYM: {
      int high;
      int low;
      int ijmp;
      double sum;

      temx = POP;
      high = (int)temx;
      temx = POP;
      low = (int)temx;
      ijmp = *equat++;
      sum = 0.0;
      if (low <= high) {
        for (int is = low; is <= high; is++) {
          constants.elems[SumIndex] = (double)is;
          sum += eval_rpn(equat);
        }
      }
      equat += ijmp;
      PUSH(sum);
      break;
    }
    case ENDSUM:
      return (POP);
    case INDXCOM:
      PUSH(0.0);
      break;
    default: {
      int it = i / MAXTYPE;
      int in = i % MAXTYPE;
      switch (it) {
      case FUN1TYPE:
        PUSH(expr_fun1[in](POP));
        break;
      case FUN2TYPE:
        switch (in) {
        case 0:
          temx = POP;
          temy = POP;
          PUSH(temx + temy);
          break;
        case 1:
          temx = POP;
          temy = POP;
          PUSH(temy - temx);
          break;
        case 2:
          temx = POP;
          temy = POP;
          PUSH(temx * temy);
          break;
        case 3:
          temx = POP;
          if (temx == 0.0)
            temx = DOUB_EPS;
          temy = POP;
          PUSH(temy / temx);
          break;
        default:
          temx = POP;
          temy = POP;
          PUSH(expr_fun2[in](temy, temx));
          break;
        }
        break;
      case CONTYPE:
        PUSH(constants.elems[in]);
        break;
      case NETTYPE:
        PUSH(network_value(POP, in));
        break;
      case TABTYPE:
        PUSH(lookup(POP, in));
        break;
      case USTACKTYPE:
        /* ram: so this means ustacks really do need to be of USTACKTYPE */
        PUSH(ustack[uptr - 1 - in]);
        break;
      case KERTYPE:
        PUSH(ker_val(in));
        break;
      case VARTYPE:
        PUSH(variables.elems[in]);
        break;
      /* indexes for shift and delay operators... */
      case SCONTYPE:
        PUSH((double)(COM(CONTYPE, in)));
        break;
      case SVARTYPE:
        PUSH((double)(COM(VARTYPE, in)));
        break;
      case UFUNTYPE:
        i = *equat++;
        for (int j = 0; j < i; j++) {
          ustack[uptr] = POP;
          uptr++;
        }
        PUSH(eval_rpn(ufuns.elems[in].rpn));
        break;
      }
      break;
    }
    }
  }
  return (POP);
}
