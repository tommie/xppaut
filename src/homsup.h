#ifndef XPPAUT_HOMSUP_H
#define XPPAUT_HOMSUP_H

/* --- Types --- */
typedef struct {
  /* dimension of unstable, stable, phasespace */
  int nunstab, nstab, n;
  /* addresses of variables holding left and right equilibria */
  int eleft, eright;
  /* address of first phase variable */
  int u0;
  double cprev[1600], a[400];
  int iflag[4];

  /* values of the boundary projections first nstab are proj to unstable mfld
     at left ane then the proj to the stabl mfld on the right */
  double fb[400];
} HOMOCLIN;

/* --- Data --- */
extern HOMOCLIN my_hom;

/* --- Functions --- */
void do_projection(double *x0, double t0, double *x1, double t1);
double hom_bcs(double x);

#endif /* XPPAUT_HOMSUP_H */
