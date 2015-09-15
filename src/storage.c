#include "storage.h"

#include <stdlib.h>

#include "form_ode.h"
#include "ggets.h"
#include "integrate.h"
#include "solver.h"

/* --- Data --- */
float **storage;
double *WORK;
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
  int nn = xpv.node + xpv.nvec;
  int sz = 30 * nn;
  switch (METHOD) {
  case METHOD_STIFF:
    sz = 2 * nn * nn + 13 * nn + 100;

    break;
  case METHOD_GEAR:
    sz = 30 * nn + nn * nn + 100;
    break;
  case METHOD_BACKEUL:
  case METHOD_VOLTERRA:
    sz = 10 * nn + nn * nn + 100;
    break;
  case METHOD_RB23:
    sz = 12 * nn + 100 + nn * nn;
    break;
  default:
    break;
  }
  if (WORK)
    free(WORK);
  WORK = (double *)malloc(sz * sizeof(double));
  /* plintf(" I have allocated %d doubles \n",sz); */
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
  /* WORK=(double *)malloc(WORKSIZE*sizeof(double));
     if(WORK!=NULL){ */
  WORK = NULL;
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
