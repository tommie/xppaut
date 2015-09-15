#include "storage.h"

#include <stdlib.h>

#include "form_ode.h"
#include "ggets.h"
#include "integrate.h"
#include "solver.h"

/* --- Data --- */
float **storage;
int MAXSTOR, storind;

void init_alloc_info(void) {
  int i;
  xpv.node = NODE + NMarkov;
  xpv.nvec = 0; /* this is just for now */
  xpv.x = (double *)malloc((xpv.nvec + xpv.node) * sizeof(double));
  /* plintf(" node=%d nvec=%d \n",xpv.node,xpv.nvec); */
  for (i = xpv.node; i < (xpv.nvec + xpv.node); i++)
    xpv.x[i] = 0.0;
}

void alloc_meth(void) {
  solver_alloc(xpv.node + xpv.nvec);
}

int reallocstor(int ncol, int nrow) {
  int i = 0;
  while ((storage[i] = (float *)realloc(storage[i], nrow * sizeof(float))) !=
         NULL) {
    i++;
    if (i == ncol)
      return 1;
  }
  err_msg("Cannot allocate sufficient storage");
  return 0;
}

void init_stor(int nrow, int ncol) {
  int i;
  storage = (float **)malloc((MAXODE + 1) * sizeof(float *));
  MAXSTOR = nrow;
  storind = 0;
  if (storage != NULL) {
    i = 0;
    while ((storage[i] = (float *)malloc(nrow * sizeof(float))) != NULL) {
      i++;
      if (i == ncol)
        return;
    }
  }
  /*  } */
  /*  plintf("col=%d\n",i); */
  err_msg("Cannot allocate sufficient storage");
  exit(0);
}
