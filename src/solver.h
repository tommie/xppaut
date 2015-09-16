#ifndef XPPAUT_SOLVER_H
#define XPPAUT_SOLVER_H

/* --- Types --- */
/* Keep METHOD_DISPLAY_NAMES in sync with this. */
typedef enum {
  METHOD_UNKNOWN = -1,
  METHOD_DISCRETE,
  METHOD_EULER,
  METHOD_MODEULER,
  METHOD_RK4,
  METHOD_ADAMS,
  METHOD_GEAR,
  METHOD_VOLTERRA,
  METHOD_BACKEUL,
  METHOD_RKQS,
  METHOD_STIFF,
  METHOD_CVODE,
  METHOD_DP5,
  METHOD_DP83,
  METHOD_RB23,
  METHOD_SYMPLECT,

  NUM_METHODS
} Method;

/* --- Data --- */
extern Method METHOD;

/* --- Functions --- */
void solver_alloc(int nn);
const char *solver_display_name(Method m);
void solver_end(void);
void solver_errmsg(int kflag);
int solver_integrate(double *y, double *t, int node, double tend, int *istart);
void solver_set_method(Method m);

#endif /* XPPAUT_SOLVER_H */
