#include "markov.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "adj2.h"
#include "browse.h"
#include "do_fit.h"
#include "form_ode.h"
#include "ggets.h"
#include "histogram.h"
#include "init_conds.h"
#include "integrate.h"
#include "load_eqn.h"
#include "my_rhs.h"
#include "parserslow.h"
#include "storage.h"
#include "base/ndrand.h"

/* --- Types --- */
typedef struct {
  int **command;
  char **trans;
  double *fixed;
  int nstates;
  double *states;
  int type; /* 0 is default and state dependent.  1 is fixed for all time  */
  char name[12];
} MARKOV;

/* --- Forward Declarations --- */
static void add_markov_entry(int index, int j, int k, char *expr);
static int compile_markov(int index, int j, int k);
static void compute_em(void);
static void create_markov(int nstates, double *st, int type, char *name);
static void extract_expr(char *source, char *dest, int *i0);
static void free_stoch(void);
static void init_stoch(int len);
static double new_state(double old, int index, double dt);
static void update_markov(double *x, double t, double dt);

/* --- Data --- */
int STOCH_FLAG;
int Wiener[MAXPAR];
int NWiener;

static MARKOV markov[MAXMARK];
static float *my_mean[MAXODE], *my_variance[MAXODE];
static int stoch_len;
static int STOCH_HERE, N_TRIALS;

void add_wiener(int index) {
  Wiener[NWiener] = index;
  NWiener++;
}

void set_wieners(double dt, double *x, double t) {
  int i;
  update_markov(x, t, fabs(dt));
  for (i = 0; i < NWiener; i++)
    constants[Wiener[i]] = ndrand48_normal(0.00, 1.00) / sqrt(fabs(dt));
}

void add_markov(int nstate, char *name) {
  double st[50];
  int i;
  for (i = 0; i < 50; i++)
    st[i] = (double)i;
  create_markov(nstate, st, 0, name);
}

int build_markov(char **ma, char *name) {
  int len = 0, ll;
  char line[256], expr[256];
  int istart;

  int i, j, nstates, index;
  index = -1;
  /* find it -- if not defined, then abort  */
  for (i = 0; i < NMarkov; i++) {
    ll = strlen(markov[i].name);
    if (strncasecmp(name, markov[i].name, ll) == 0) {

      if (len < ll) {
        index = i;
        len = ll;
      }
    }
  }
  if (index == -1) {
    plintf(" Markov variable |%s| not found \n", name);
    exit(0);
  }
  /* get number of states  */
  nstates = markov[index].nstates;
  if (ConvertStyle)
    fprintf(convertf, "markov %s %d\n", name, nstates);
  plintf(" Building %s %d states...\n", name, nstates);
  for (i = 0; i < nstates; i++) {
    sprintf(line, "%s", ma[i]);
    if (ConvertStyle)
      fprintf(convertf, "%s", line);
    istart = 0;
    for (j = 0; j < nstates; j++) {
      extract_expr(line, expr, &istart);
      plintf("%s ", expr);
      add_markov_entry(index, i, j, expr);
    }
    plintf("\n");
  }
  return index;
}

int old_build_markov(FILE *fptr, char *name) {
  int len = 0, ll;
  char line[256], expr[256];
  int istart;

  int i, j, nstates, index;
  index = -1;
  /* find it -- if not defined, then abort  */
  for (i = 0; i < NMarkov; i++) {
    ll = strlen(markov[i].name);
    if (strncasecmp(name, markov[i].name, ll) == 0) {

      if (len < ll) {
        index = i;
        len = ll;
      }
    }
  }
  if (index == -1) {
    plintf(" Markov variable |%s| not found \n", name);
    exit(0);
  }
  /* get number of states  */
  nstates = markov[index].nstates;
  if (ConvertStyle)
    fprintf(convertf, "markov %s %d\n", name, nstates);
  plintf(" Building %s ...\n", name);
  for (i = 0; i < nstates; i++) {
    fgets(line, 256, fptr);

    if (ConvertStyle)
      fprintf(convertf, "%s", line);
    istart = 0;
    for (j = 0; j < nstates; j++) {
      extract_expr(line, expr, &istart);
      plintf("%s ", expr);
      add_markov_entry(index, i, j, expr);
    }
    plintf("\n");
  }
  return index;
}

static void extract_expr(char *source, char *dest, int *i0) {
  char ch;
  int len = 0;
  int flag = 0;
  while (1) {
    ch = source[*i0];
    *i0 = *i0 + 1;
    if (ch == '}')
      break;
    if (ch == '{')
      flag = 1;
    else {
      if (flag) {
        dest[len] = ch;
        len++;
      }
    }
  }
  dest[len] = 0;
}

static void create_markov(int nstates, double *st, int type, char *name) {
  int i;
  int n2 = nstates * nstates;
  int j = NMarkov;
  if (j >= MAXMARK) {
    plintf("Too many Markov chains...\n");
    exit(0);
  }

  markov[j].nstates = nstates;
  markov[j].states = (double *)malloc(nstates * sizeof(double));
  if (type == 0) {
    markov[j].trans = (char **)malloc(n2 * sizeof(char *));
    markov[j].command = (int **)malloc(n2 * sizeof(int *));
  } else {
    markov[j].fixed = (double *)malloc(n2 * sizeof(double));
  }

  for (i = 0; i < nstates; i++)
    markov[j].states[i] = st[i];
  strcpy(markov[j].name, name);
  NMarkov++;
}

static void add_markov_entry(int index, int j, int k, char *expr) {
  int l0 = markov[index].nstates * j + k;
  int type = markov[index].type;
  if (type == 0) {
    markov[index].trans[l0] = (char *)malloc(sizeof(char) * (strlen(expr) + 1));
    strcpy(markov[index].trans[l0], expr);
  } else {
    markov[index].fixed[l0] = atof(expr);
  }
}

void compile_all_markov(void) {
  int index, j, k, ns, l0;
  if (NMarkov == 0)
    return;
  for (index = 0; index < NMarkov; index++) {
    ns = markov[index].nstates;
    for (j = 0; j < ns; j++) {
      for (k = 0; k < ns; k++) {
        l0 = ns * j + k;
        if (compile_markov(index, j, k) == -1) {
          plintf("Bad expression %s[%d][%d] = %s \n", markov[index].name, j, k,
                 markov[index].trans[l0]);
          exit(0);
        }
      }
    }
  }
}

static int compile_markov(int index, int j, int k) {
  char *expr;
  int l0 = markov[index].nstates * j + k, leng;
  int i;
  int com[256];
  expr = markov[index].trans[l0];

  if (add_expr(expr, com, &leng))
    return -1;
  markov[index].command[l0] = (int *)malloc(sizeof(int) * (leng + 2));
  for (i = 0; i < leng; i++) {
    markov[index].command[l0][i] = com[i];
  }

  return 1;
}

static void update_markov(double *x, double t, double dt) {
  int i;
  double yp[MAXODE];
  /*  plintf(" NODE=%d x=%g \n",NODE,x[0]); */
  if (NMarkov == 0)
    return;
  set_ivar(0, t);
  for (i = 0; i < NODE; i++)
    set_ivar(i + 1, x[i]);
  for (i = NODE + FIX_VAR; i < NODE + FIX_VAR + NMarkov; i++)
    set_ivar(i + 1, x[i - FIX_VAR]);
  for (i = NODE; i < NODE + FIX_VAR; i++)
    set_ivar(i + 1, evaluate(my_ode[i]));
  for (i = 0; i < NMarkov; i++)
    yp[i] = new_state(x[NODE + i], i, dt);
  for (i = 0; i < NMarkov; i++) {
    x[NODE + i] = yp[i];
    set_ivar(i + NODE + FIX_VAR + 1, yp[i]);
  }
}

static double new_state(double old, int index, double dt) {
  double prob, sum;
  double coin = ndrand48();
  int row = -1, rns;
  double *st;
  int i, ns = markov[index].nstates;
  int type = markov[index].type;
  st = markov[index].states;
  /*  plintf(" old=%g i=%d st=%g\n",old,index,st); */
  for (i = 0; i < ns; i++)
    if (fabs(st[i] - old) < .0001) {
      row = i;
      break;
    }
  if (row == -1)
    return (old);
  rns = row * ns;
  sum = 0.0;
  if (type == 0) {
    for (i = 0; i < ns; i++) {
      if (i != row) {
        prob = evaluate(markov[index].command[rns + i]) * dt;
        sum = sum + prob;
        if (coin <= sum) {
          /*	   plintf("index %d switched state to %d \n",index,i); */
          return (st[i]);
        }
      }
    }
  } else {
    for (i = 0; i < ns; i++) {
      if (i != row) {
        prob = markov[index].fixed[rns + i] * dt;
        sum = sum + prob;
        if (coin <= sum) {
          /*	   plintf("index %d switched state to %d \n",index,i); */
          return (st[i]);
        }
      }
    }
  }

  return (old);
}

void make_gill_nu(double *nu, int n, int m, double *v) {
  /* nu[j+m*i] = nu_{i,j} i=1,n-1 -- assume first eqn is tr'=tr+z(0)
     i species j reaction
    need this for improved tau stepper
   */
  double *y, *yp, *yold;
  int ir, iy;

  y = (double *)malloc(n * sizeof(double));
  yold = (double *)malloc(n * sizeof(double));
  yp = (double *)malloc(n * sizeof(double));
  for (ir = 0; ir < m; ir++)
    v[ir + 1] = 0;
  rhs_only(y, yold);
  for (ir = 0; ir < m; ir++) {
    v[ir + 1] = 1;
    rhs_only(y, yp);
    for (iy = 0; iy < n; iy++) {
      nu[ir + m * iy] = yp[iy];
      plintf("ir=%d iy=%d nu=%g\n", ir + 1, iy, yp[iy] - yold[iy]);
    }
    v[ir + 1] = 0;
  }

  free(y);
  free(yp);
  free(yold);
}

void one_gill_step(int meth, int nrxn, int *rxn, double *v) {
  double rate = 0, test;
  double r[1000];
  /*double rold[1000]; Not used*/

  int i;
  switch (meth) {
  case 0: /* std gillespie method */
    for (i = 0; i < nrxn; i++) {
      v[i + 1] = 0.0;
      r[i] = get_ivar(rxn[i]);
      rate += r[i];
    }
    if (rate <= 0.0)
      return;
    /* plintf("rate=%g \n",rate); */
    v[0] = -log(ndrand48()) / rate; /* next step */
    test = rate * ndrand48();
    rate = r[0];
    for (i = 0; i < nrxn; i++) {
      if (test < rate) {
        v[i + 1] = 1.0;
        break;
      }
      rate += r[i + 1];
    }
    break;
  case 1: /* tau stepping method  */
    perror("Tau stepping method not implemented yet.");
    /*for(i=0;i<nrxn;i++)
      rold[i]=get_ivar(rxn[i]);
        */
    break;
  }
}

void do_stochast_com(int i) {
  static char key[] = "ncdmvhofpislare2";
  char ch = key[i];

  if (ch == 27)
    return;
  switch (ch) {
  case 'n':
    new_int("Seed:", &RandSeed);
    nsrand48(RandSeed);
    break;
  case 'd':
    data_back();
    break;
  case 'm':
    mean_back();
    break;
  case 'v':
    variance_back();
    break;
  case 'c':
    compute_em();
    STOCH_FLAG = 0;
    break;
  case 'h':
    compute_hist();
    break;
  case 'o':
    hist_back();
    break;
  case 'f':
    compute_fourier();
    break;
  case 'p':
    compute_power();
    break;
  case 'i':
    test_fit();
    redraw_params();
    redraw_ics();
    break;
  case 's':
    column_mean();
    break;
  case 'l':
    do_liapunov();
    break;
  case 'a':
    compute_stacor();
    break;
  case 'r':
    compute_correl();
    break;
  case 'e':
    compute_sd();
    break;
  case '2':
    new_2d_hist();
    break;
  }
}

void mean_back(void) {
  if (STOCH_HERE) {
    set_browser_data(my_mean, 1);
    refresh_browser(stoch_len);
    storind = stoch_len;
  }
}

void variance_back(void) {
  if (STOCH_HERE) {
    set_browser_data(my_variance, 1);
    refresh_browser(stoch_len);
    storind = stoch_len;
  }
}

static void compute_em(void) {
  double *x;
  x = &MyData[0];
  free_stoch();
  STOCH_FLAG = 1;
  do_range(x, 0);
  redraw_ics();
}

static void free_stoch(void) {
  int i;
  if (STOCH_HERE) {
    data_back();
    for (i = 0; i < (NEQ + 1); i++) {
      free(my_mean[i]);
      free(my_variance[i]);
    }
    STOCH_HERE = 0;
  }
}

static void init_stoch(int len) {
  int i, j;
  N_TRIALS = 0;
  stoch_len = len;
  for (i = 0; i < (NEQ + 1); i++) {
    my_mean[i] = (float *)malloc(sizeof(float) * stoch_len);
    my_variance[i] = (float *)malloc(sizeof(float) * stoch_len);
    for (j = 0; j < stoch_len; j++) {
      my_mean[i][j] = 0.0;
      my_variance[i][j] = 0.0;
    }
  }
  for (j = 0; j < stoch_len; j++) {
    my_mean[0][j] = storage[0][j];
    my_variance[0][j] = storage[0][j];
  }
  STOCH_HERE = 1;
}

void append_stoch(int first, int length) {
  int i, j;
  float z;
  if (first == 0)
    init_stoch(length);
  if (length != stoch_len || !STOCH_HERE)
    return;
  for (i = 0; i < stoch_len; i++) {
    for (j = 1; j <= NEQ; j++) {
      z = storage[j][i];
      my_mean[j][i] = my_mean[j][i] + z;
      my_variance[j][i] = my_variance[j][i] + z * z;
    }
  }
  N_TRIALS++;
}

void do_stats(int ierr) {
  int i, j;
  float ninv, mean;
  /*  STOCH_FLAG=0; */
  if (ierr != -1 && N_TRIALS > 0) {
    ninv = 1. / (float)(N_TRIALS);
    for (i = 0; i < stoch_len; i++) {
      for (j = 1; j <= NEQ; j++) {
        mean = my_mean[j][i] * ninv;
        my_mean[j][i] = mean;
        my_variance[j][i] = (my_variance[j][i] * ninv - mean * mean);
      }
    }
  }
}
