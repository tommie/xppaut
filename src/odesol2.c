#include "odesol2.h"

/* --- Data --- */
int (*rhs)(double t, double *y, double *ydot, int neq);
