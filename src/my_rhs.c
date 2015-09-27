#include "my_rhs.h"

#include <stdlib.h>

#include "dae_fun.h"
#include "extra.h"
#include "form_ode.h"
#include "parserslow.h"
#include "simplenet.h"

/* --- Forward Declarations --- */
static void vec_rhs(double t, double *y, double *ydot, int neq);

void extra(double *y__y, double t, int nod, int neq) {
  int i;
  if (nod >= neq)
    return;
  set_ivar(0, t);
  for (i = 0; i < nod; i++)
    set_ivar(i + 1, y__y[i]);
  for (i = nod + FIX_VAR; i < nod + FIX_VAR + NMarkov; i++)
    set_ivar(i + 1, y__y[i - FIX_VAR]);
  for (i = nod; i < nod + FIX_VAR; i++)
    set_ivar(i + 1, evaluate(my_ode[i]));
  do_in_out();
  for (i = nod + NMarkov; i < neq; i++)
    y__y[i] = evaluate(my_ode[i + FIX_VAR - NMarkov]);
}

void set_fix_rhs(double t, double *y) {
  int i;
  set_ivar(0, t);
  for (i = 0; i < NODE; i++)
    set_ivar(i + 1, y[i]);
  for (i = 0; i < NMarkov; i++)
    set_ivar(i + 1 + NODE + FIX_VAR, y[i + NODE]);
  for (i = NODE; i < NODE + FIX_VAR; i++)
    set_ivar(i + 1, evaluate(my_ode[i]));
  eval_all_nets();
  do_in_out();
}

int my_rhs(double t, double *y, double *ydot, int neq) {
  int i;
  set_ivar(0, t);
  for (i = 0; i < NODE; i++)
    set_ivar(i + 1, y[i]);

  for (i = NODE; i < NODE + FIX_VAR; i++) {
    set_ivar(i + 1, evaluate(my_ode[i]));
    /* plintf("%d %g \n",i+1,get_ivar(i+1)); */
  }
  /*printf("WTF %g\n",evaluate(my_ode[1]));
  */
  eval_all_nets();

  do_daes();
  do_in_out();
  for (i = 0; i < NODE; i++) {
    ydot[i] = evaluate(my_ode[i]);
  }
  if (neq > NODE)
    vec_rhs(t, y, ydot, neq);

  return (1);
}

void update_based_on_current(void) {
  int i;
  for (i = NODE; i < NODE + FIX_VAR; i++)
    set_ivar(i + 1, evaluate(my_ode[i]));

  eval_all_nets();
  do_in_out();
}

void fix_only(void) {
  int i;
  for (i = NODE; i < NODE + FIX_VAR; i++)
    set_ivar(i + 1, evaluate(my_ode[i]));
}

void rhs_only(double *y, double *ydot) {
  int i;
  for (i = 0; i < NODE; i++) {
    ydot[i] = evaluate(my_ode[i]);
  }
}

static void vec_rhs(double t, double *y, double *ydot, int neq) {}
