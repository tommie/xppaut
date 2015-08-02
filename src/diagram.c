#include "diagram.h"

#include <stdlib.h>

#include "autevd.h"
#include "auto_nox.h"
#include "browse.h"
#include "form_ode.h"
#include "ggets.h"
#include "graf_par.h"
#include "graphics.h"
#include "init_conds.h"
#include "my_ps.h"
#include "my_svg.h"
#include "storage.h"

/* --- Macros --- */
#define DALLOC(a) (double *) malloc((a) * sizeof(double))

/* --- Forward Declarations --- */
static void edit_diagram(DIAGRAM *d, int ibr, int ntot, int itp, int lab,
                         int nfpar, double a, double *uhi, double *ulo,
                         double *u0, double *ubar, double *par, double per,
                         int n, int icp1, int icp2, int flag2, double *evr,
                         double *evi, double tp);

/* --- Data --- */
DIAGRAM *bifd = NULL;
static int NBifs = 0;

/* Only unit 8,3 or q.prb is important; all others are unnecesary */
int get_bif_type(int ibr, int ntot, int lab) {
  int type = CSEQ;
  if (ibr < 0 && ntot < 0)
    type = SPER;
  if (ibr < 0 && ntot > 0)
    type = UPER;
  if (ibr > 0 && ntot > 0)
    type = CUEQ;
  if (ibr > 0 && ntot < 0)
    type = CSEQ;
  return (type);
}

static void edit_diagram(DIAGRAM *d, int ibr, int ntot, int itp, int lab,
                         int nfpar, double a, double *uhi, double *ulo,
                         double *u0, double *ubar, double *par, double per,
                         int n, int icp1, int icp2, int flag2, double *evr,
                         double *evi, double tp) {
  int i;
  d->ibr = ibr;
  d->ntot = ntot;
  d->itp = itp;
  d->lab = lab;
  d->nfpar = nfpar;
  d->norm = a;
  for (i = 0; i < 5; i++)
    d->par[i] = par[i];

  d->per = per;

  d->icp1 = icp1;
  d->icp2 = icp2;

  d->flag2 = flag2;
  for (i = 0; i < n; i++) {
    d->ulo[i] = ulo[i];
    d->uhi[i] = uhi[i];
    d->ubar[i] = ubar[i];
    d->u0[i] = u0[i];
    d->evr[i] = evr[i];
    d->evi[i] = evi[i];
  }
  d->torper = tp;
}

DIAGRAM *add_diagram(int ibr, int ntot, int itp, int lab, int nfpar, double a,
                     double *uhi, double *ulo, double *u0, double *ubar,
                     double *par, double per, int n, int icp1, int icp2,
                     int flag2, double *evr, double *evi) {
  DIAGRAM **d, *prev = NULL;

  d = &bifd;
  while (*d != NULL) {
    prev = *d;
    d = &(*d)->next;
  }
  *d = (DIAGRAM *)malloc(sizeof(DIAGRAM));
  (*d)->next = NULL;
  (*d)->prev = prev;
  (*d)->uhi = DALLOC(n);
  (*d)->ulo = DALLOC(n);
  (*d)->u0 = DALLOC(n);
  (*d)->ubar = DALLOC(n);
  (*d)->evr = DALLOC(n);
  (*d)->evi = DALLOC(n);
  (*d)->index = NBifs;
  NBifs++;
  edit_diagram(*d, ibr, ntot, itp, lab, nfpar, a, uhi, ulo, u0, ubar, par, per,
               n, icp1, icp2, flag2, evr, evi, blrtn.torper);

  return (*d);
}

void kill_diagrams(void) {
  DIAGRAM *d = bifd;
  /* Move to the end of the tree */
  while (d != NULL && d->next != NULL) {
    d = d->next;
  }
  while (d != NULL) {
    DIAGRAM *dnew = d->prev;

    free(d->uhi);
    free(d->ulo);
    free(d->u0);
    free(d->ubar);
    free(d->evr);
    free(d->evi);
    free(d);
    d = dnew;
  }
  bifd = NULL;
  NBifs = 0;
}

void draw_diagram(DIAGRAM *d) {
  int type = get_bif_type(d->ibr, d->ntot, d->lab);

  add_point(d->par, d->per, d->uhi, d->ulo, d->ubar, d->norm, type,
            d->ntot != 1, d->lab, d->nfpar, d->icp1, d->icp2, d->flag2, d->evr,
            d->evi);
}

void redraw_diagram(void) {
  draw_bif_axes();

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    draw_diagram(d);
  }
}

void write_info_out(void) {
  /*char filename[256];*/
  char filename[XPP_MAX_NAME];
  /*int flag=0
  */
  int status;
  int icp1, icp2;
  double *par;
  double par1, par2 = 0, *uhigh, *ulow, per;
  /*double a,*ubar,*u0;*/
  FILE *fp;
  sprintf(filename, "allinfo.dat");
  /* status=get_dialog("Write all info","Filename",filename,"Ok","Cancel",60);
   */
  status = file_selector("Write all info", filename, "*.dat");

  if (status == 0)
    return;
  fp = fopen(filename, "w");
  if (fp == NULL) {
    err_msg("Can't open file");
    return;
  }

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    int i;
    int type = get_bif_type(d->ibr, d->ntot, d->lab);

    /*if(d->ntot==1)flag=0;
    else flag=1;
    */
    icp1 = d->icp1;
    icp2 = d->icp2;
    par = d->par;
    per = d->per;
    uhigh = d->uhi;
    ulow = d->ulo;
    /*ubar=d->ubar; Not used*/
    /* u0=d->u0; Not used*/
    /* a=d->norm; Not used*/
    par1 = par[icp1];
    if (icp2 < NAutoPar)
      par2 = par[icp2];
    else
      par2 = par1;

    fprintf(fp, "%d %d %g %g %g ", type, d->ibr, par1, par2, per);
    for (i = 0; i < NODE; i++)
      fprintf(fp, "%g ", uhigh[i]);
    for (i = 0; i < NODE; i++)
      fprintf(fp, "%g ", ulow[i]);
    for (i = 0; i < NODE; i++)
      fprintf(fp, "%g %g ", d->evr[i], d->evi[i]);
    fprintf(fp, "\n");
  }
  fclose(fp);
}

void load_browser_with_branch(int ibr, int pts, int pte) {
  int i, j, pt;
  /*int flag=0;
  */
  int icp1;
  double *par;
  double par1, *u0;
  int first, last, nrows;
  first = abs(pts);
  last = abs(pte);
  if (last <
      first) { /* reorder the points so that we will store in right range*/
    i = first;
    first = last;
    last = i;
  }
  nrows = last - first + 1;
  j = 0;
  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    get_bif_type(d->ibr, d->ntot, d->lab);
    pt = abs(d->ntot);
    if ((d->ibr == ibr) && (pt >= first) && (pt <= last)) {
      icp1 = d->icp1;
      par = d->par;
      u0 = d->u0;

      par1 = par[icp1];
      storage[0][j] = par1;
      for (i = 0; i < NODE; i++)
        storage[i + 1][j] = u0[i];
      j++;
    }
  }
  storind = nrows;
  refresh_browser(nrows);
}

void write_init_data_file(void) {
  /*char filename[256];*/
  char filename[XPP_MAX_NAME];
  int type, i;
  /*int flag=0;
  */
  int status;
  int icp1, icp2;
  double *par;
  double par1, par2 = 0, *u0, per;
  /*double a,*uhigh,*ulow,*ubar;*/
  FILE *fp;
  sprintf(filename, "initdata.dat");
  /* status=get_dialog("Write all info","Filename",filename,"Ok","Cancel",60);
   */
  status = file_selector("Write init data file", filename, "*.dat");

  if (status == 0)
    return;
  fp = fopen(filename, "w");
  if (fp == NULL) {
    err_msg("Can't open file");
    return;
  }

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    type = get_bif_type(d->ibr, d->ntot, d->lab);

    /*if(d->ntot==1)flag=0;
    else flag=1;
    Unused here?
    */
    icp1 = d->icp1;
    icp2 = d->icp2;
    par = d->par;
    per = d->per;
    /*
    uhigh=d->uhi;
    ulow=d->ulo;
    ubar=d->ubar;
    Unused here??
    */
    u0 = d->u0;

    /*
    a=d->norm;

    Unused here??
    */
    par1 = par[icp1];
    if (icp2 < NAutoPar)
      par2 = par[icp2];
    else
      par2 = par1;

    fprintf(fp, "%d %d %g %g %g ", type, d->ibr, par1, par2, per);
    for (i = 0; i < NODE; i++)
      fprintf(fp, "%g ", u0[i]);
    fprintf(fp, "\n");
  }
  fclose(fp);
}

void write_pts(void) {
  /*char filename[256];*/
  char filename[XPP_MAX_NAME];
  int type;
  /*int flag=0;
  */
  int status;
  int icp1, icp2;
  double *par;
  double x, y1, y2, par1, par2 = 0, a, *uhigh, *ulow, *ubar, per;
  FILE *fp;
  sprintf(filename, "diagram.dat");
  status = file_selector("Write points", filename, "*.dat");
  /* get_dialog("Write points","Filename",filename,"Ok","Cancel",60); */
  if (status == 0)
    return;
  fp = fopen(filename, "w");
  if (fp == NULL) {
    err_msg("Can't open file");
    return;
  }

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    type = get_bif_type(d->ibr, d->ntot, d->lab);

    /*if(d->ntot==1)flag=0;
    else flag=1;

    Unused here??
    */
    icp1 = d->icp1;
    icp2 = d->icp2;
    par = d->par;
    per = d->per;
    uhigh = d->uhi;
    ulow = d->ulo;
    ubar = d->ubar;
    a = d->norm;
    par1 = par[icp1];
    if (icp2 < NAutoPar)
      par2 = par[icp2];
    auto_xy_plot(&x, &y1, &y2, par1, par2, per, uhigh, ulow, ubar, a);
    fprintf(fp, "%g %g %g %d %d \n", x, y1, y2, type, abs(d->ibr));
  }
  fclose(fp);
}

void post_auto(void) {
  /*char filename[256];*/
  char filename[XPP_MAX_NAME];
  int type, flag = 0;
  int status;
  sprintf(filename, "auto.ps");
  /* status=get_dialog("Postscript","Filename",filename,"Ok","Cancel",60); */
  status = file_selector("Postscript", filename, "*.ps");
  if (status == 0)
    return;
  if (!ps_init(filename, PS_Color))
    return;
  draw_ps_axes();

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    type = get_bif_type(d->ibr, d->ntot, d->lab);
    if (type < 0) {
      plintf("Unable to get bifurcation type.\n");
    }
    if (d->ntot == 1)
      flag = 0;
    else
      flag = 1;
    add_ps_point(d->par, d->per, d->uhi, d->ulo, d->ubar, d->norm, type, flag,
                 d->lab, d->nfpar, d->icp1, d->icp2, d->flag2, d->evr, d->evi);
  }
  ps_end();
  set_normal_scale();
}

void svg_auto(void) {
  /*char filename[256];*/
  char filename[XPP_MAX_NAME];
  int type, flag = 0;
  int status;
  sprintf(filename, "auto.svg");
  /* status=get_dialog("Postscript","Filename",filename,"Ok","Cancel",60); */
  status = file_selector("SVG", filename, "*.svg");
  if (status == 0)
    return;
  if (!svg_init(filename, PS_Color))
    return;
  draw_svg_axes();

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    type = get_bif_type(d->ibr, d->ntot, d->lab);
    if (type < 0) {
      plintf("Unable to get bifurcation type.\n");
    }
    if (d->ntot == 1)
      flag = 0;
    else
      flag = 1;
    add_ps_point(d->par, d->per, d->uhi, d->ulo, d->ubar, d->norm, type, flag,
                 d->lab, d->nfpar, d->icp1, d->icp2, d->flag2, d->evr, d->evi);
  }
  svg_end();

  set_normal_scale();
}

void bound_diagram(double *xlo, double *xhi, double *ylo, double *yhi) {
  double x, y1, y2, par1, par2 = 0.0;
  *xlo = 1.e16;
  *ylo = *xlo;
  *xhi = -*xlo;
  *yhi = -*ylo;

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    int type = get_bif_type(d->ibr, d->ntot, d->lab);
    if (type < 1) {
      plintf("Unable to get bifurcation type.\n");
    }
    /*if(d->ntot==1)flag=0;
    else flag=1;
    Unused here?
    */
    par1 = d->par[d->icp1];
    if (d->icp2 < NAutoPar)
      par2 = d->par[d->icp2];
    auto_xy_plot(&x, &y1, &y2, par1, par2, d->per, d->uhi, d->ulo, d->ubar,
                 d->norm);
    if (x < *xlo)
      *xlo = x;
    if (x > *xhi)
      *xhi = x;
    if (y2 < *ylo)
      *ylo = y2;
    if (y1 > *yhi)
      *yhi = y1;
  }
}

int save_diagram(FILE *fp, int n) {
  /* Store the highest index we have. */
  fprintf(fp, "%d\n", NBifs - 1);

  for (DIAGRAM *d = bifd; d != NULL; d = d->next) {
    int i;

    fprintf(fp, "%d %d %d %d %d %d %d %d %d\n", d->ibr, d->ntot, d->itp, d->lab,
            d->index, d->nfpar, d->icp1, d->icp2, d->flag2);
    for (i = 0; i < 5; i++)
      fprintf(fp, "%g ", d->par[i]);
    fprintf(fp, "%g %g \n", d->norm, d->per);

    for (i = 0; i < n; i++)
      fprintf(fp, "%f %f %f %f %f %f\n", d->u0[i], d->uhi[i], d->ulo[i],
              d->ubar[i], d->evr[i], d->evi[i]);
  }
  return (1);
}

int load_diagram(FILE *fp, int node) {
  int n;
  fscanf(fp, "%d", &n);

  while (1) {
    double u0[NAUTO], uhi[NAUTO], ulo[NAUTO], ubar[NAUTO], evr[NAUTO],
        evi[NAUTO], norm, par[5], per;
    int i;
    int ibr, ntot, itp, lab, index, nfpar, icp1, icp2, flag2;

    fscanf(fp, "%d %d %d %d %d %d %d %d %d ", &ibr, &ntot, &itp, &lab, &index,
           &nfpar, &icp1, &icp2, &flag2);
    for (i = 0; i < 5; i++)
      fscanf(fp, "%lg ", &par[i]);
    fscanf(fp, "%lg %lg ", &norm, &per);
    for (i = 0; i < node; i++)
      fscanf(fp, "%lg %lg %lg %lg %lg %lg", &u0[i], &uhi[i], &ulo[i], &ubar[i],
             &evr[i], &evi[i]);
    add_diagram(ibr, ntot, itp, lab, nfpar, norm, uhi, ulo, u0, ubar, par, per,
                node, icp1, icp2, flag2, evr, evi);
    if (index >= n)
      break;
  }
  return (1);
}
