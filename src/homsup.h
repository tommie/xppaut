#ifndef XPPAUT_HOMSUP_H
#define XPPAUT_HOMSUP_H

/* --- Types --- */
typedef struct {
  int nunstab,nstab,n; /* dimension of unstable, stable, phasespace */
  int eleft,eright; /* addresses of variables holding left and right
                       equilibria */
  int u0; /* address of first phase variable */
  double cprev[1600],a[400];
  int iflag[4];
  double fb[400]; /* values of the boundary projections
                     first nstab are proj to unstable mfld
                     at left ane then the proj to the stabl
		     mfld on the right */
} HOMOCLIN;

/* --- Data --- */
extern HOMOCLIN my_hom;

/* --- Functions --- */
void do_projection(double *x0,double t0,double *x1,double t1);
double hom_bcs(double x);

#endif /* XPPAUT_HOMSUP_H */
