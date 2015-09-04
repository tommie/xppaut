#include "findsing.h"

#include <math.h>
#include <stdlib.h>

#include "abort.h"
#include "eig_list.h"
#include "eigen.h"
#include "form_ode.h"
#include "gear.h"
#include "ggets.h"
#include "graphics.h"
#include "integrate.h"
#include "load_eqn.h"
#include "markov.h"
#include "menudrive.h"
#include "numerics.h"
#include "odesol2.h"

/* --- Forward Declarations --- */
static void get_evec(double *a, double *anew, double *b, double *bp, int n,
                     int maxit, double err, int *ipivot, double eval,
                     int *ierr);
static void getjac(double *x, double *y, double *yp, double *xp, double eps,
                   double *dermat, int n);
static void pr_evec(double *x, double *ev, int n, int pr, double eval,
                    int type);

/* --- Data --- */
int UnstableManifoldColor = 5;
int StableManifoldColor = 8;
double ShootIC[8][MAXODE];
int ShootICFlag;
int ShootIndex;
static int ShootType[8];

/* main fixed point finder */
void do_sing(double *x, double eps, double err, double big, int maxit, int n,
             int *ierr, float *stabinfo) {
  int kmem, i, j, ipivot[MAXODE];
  int oldcol, dummy;
  int rp = 0, rn = 0, cp = 0, cn = 0, im = 0;
  int pose = 0, nege = 0, pr;
  double *work, *eval, *b, *bp, *oldwork, *ework;
  double temp, oldt = DELTA_T, old_x[MAXODE];

  char ch;
  double real, imag;
  double bigpos = -1e10, bigneg = 1e10;
  int bpos = 0, bneg = 0;
  /* float xl[MAXODE]; */
  kmem = n * (2 * n + 5) + 50;
  if ((work = (double *)malloc(sizeof(double) * kmem)) == NULL) {
    err_msg("Insufficient core ");
    return;
  }
  ShootICFlag = 0;
  ShootIndex = 0;
  for (i = 0; i < n; i++)
    old_x[i] = x[i];
  oldwork = work + n * n;
  eval = oldwork + n * n;
  b = eval + 2 * n;
  bp = b + n;
  ework = bp + n;
  rooter(x, err, eps, big, work, ierr, maxit, n);
  if (*ierr != 0) {
    free(work);
    err_msg("Could not converge to root");
    for (i = 0; i < n; i++)
      x[i] = old_x[i];
    return;
  }
  ping();
  /* for(i=0;i<n;i++)xl[i]=(float)x[i]; */

  for (i = 0; i < n * n; i++) {
    oldwork[i] = work[i];
    /* plintf("dm=%g\n",oldwork[i]); */
  }
  /* Transpose for Eigen        */
  for (i = 0; i < n; i++) {
    for (j = i + 1; j < n; j++) {
      temp = work[i + j * n];
      work[i + j * n] = work[i * n + j];
      work[i * n + j] = temp;
    }
  }
  eigen(n, work, eval, ework, ierr);
  if (*ierr != 0) {
    err_msg("Could not compute eigenvalues");
    free(work);
    return;
  }
  /* succesfully computed evals now lets work with them */
  ch = 'n';
  if (!PAR_FOL) {
    ch = (char)TwoChoice("YES", "NO", "Print eigenvalues?", "yn");
  }
  pr = 0;

  if (ch == 'y') {
    plintf("\n Eigenvalues:\n");
    pr = 1;
  }
  for (i = 0; i < n; i++) {
    real = eval[2 * i];
    imag = eval[2 * i + 1];
    if (pr == 1) {
      plintf(" %f  +  i  %f \n", real, imag);
    }
    if (METHOD == 0)
      real = real * real + imag * imag - 1.00;
    if (fabs(imag) < .00000001)
      imag = 0.0;
    if (real < 0.0) {
      if (imag != 0.0) {
        cn++;
        if (real < bigneg) {
          bigneg = real;
          bneg = -1;
        }
      } else {
        rn++;
        nege = i;
        if (real < bigneg) {
          bigneg = real;
          bneg = i;
        }
      }
    }
    if (real > 0.0) {
      if (imag != 0.0) {
        cp++;
        if (real > bigpos) {
          bigpos = real;
          bpos = -1;
        }
      } else {
        rp++;
        pose = i;
        if (real > bigpos) {
          bigpos = real;
          bpos = i;
        }
      }
    }
    if ((real == 0.0) && (imag != 0.0))
      im++;
  } /* eigenvalue count */
  if (((rp + cp) != 0) && ((rn + cn) != 0))
    eq_symb(x, 1);
  else {
    if ((rp + cp) != 0)
      eq_symb(x, 0);
    else
      eq_symb(x, 3);
  }

  *stabinfo = (float)(cp + rp) + (float)(cn + rn) / 1000.0;

  /* Lets change Work back to transposed oldwork */
  for (i = 0; i < n; i++) {
    for (j = i + 1; j < n; j++) {
      temp = oldwork[i + j * n];
      work[i + j * n] = oldwork[i * n + j];
      work[i * n + j] = temp;
    }
  }
  create_eq_box(cp, cn, rp, rn, im, x, eval, n);
  if (((rp == 1) || (rn == 1)) && (n > 1)) {
    ch = 'n';
    if (!PAR_FOL) {
      ch = (char)TwoChoice("YES", "NO", "Draw Invariant Sets?", "yn");
    }
    if ((ch == 'y') || (PAR_FOL && SHOOT)) {
      oldt = DELTA_T;

      if (rp == 1) {
        /* plintf(" One real positive -- pos=%d lam=%g \n",pose,eval[2*pose]);
         */
        /*     for(i=0;i<n*n;i++)printf(" w=%g o=%g \n",work[i],oldwork[i]); */
        get_evec(work, oldwork, b, bp, n, maxit, err, ipivot, eval[2 * pose],
                 ierr);
        if (*ierr == 0) {
          change_current_linestyle(UnstableManifoldColor, &oldcol);
          pr_evec(x, b, n, pr, eval[2 * pose], 1);
          DELTA_T = fabs(DELTA_T);
          shoot(bp, x, b, 1);
          shoot(bp, x, b, -1);
          change_current_linestyle(oldcol, &dummy);

        } else
          err_msg("Failed to compute eigenvector");
      }
      if (rn == 1) {

        get_evec(work, oldwork, b, bp, n, maxit, err, ipivot, eval[2 * nege],
                 ierr);
        if (*ierr == 0) {
          change_current_linestyle(StableManifoldColor, &oldcol);
          pr_evec(x, b, n, pr, eval[2 * nege], -1);
          DELTA_T = -fabs(DELTA_T);
          shoot(bp, x, b, 1);
          shoot(bp, x, b, -1);
          change_current_linestyle(oldcol, &dummy);
        } else
          err_msg("Failed to compute eigenvector");
      }
      DELTA_T = oldt;
    }
  } /* end of normal shooting stuff */

  /* strong (un) stable manifold calculation
     only one-d manifolds calculated */
  /* lets check to see if it is relevant */
  if (((rn > 1) && (bneg >= 0)) || ((rp > 1) && (bpos >= 0))) {
    ch = 'n';
    if (!PAR_FOL) {
      ch = (char)TwoChoice("YES", "NO", "Draw Strong Sets?", "yn");
    }

    if ((ch == 'y') || (PAR_FOL && SHOOT)) {
      oldt = DELTA_T;

      if ((rp > 1) && (bpos >= 0)) /* then there is a strong unstable */
      {
        plintf("strong unstable %g \n", bigpos);
        get_evec(work, oldwork, b, bp, n, maxit, err, ipivot, bigpos, ierr);
        if (*ierr == 0) {
          change_current_linestyle(UnstableManifoldColor, &oldcol);
          pr_evec(x, b, n, pr, bigpos, 1);
          DELTA_T = fabs(DELTA_T);
          shoot(bp, x, b, 1);
          shoot(bp, x, b, -1);
          change_current_linestyle(oldcol, &dummy);

        } else
          err_msg("Failed to compute eigenvector");
      }

      if ((rn > 1) && (bneg >= 0)) /* then there is a strong stable */
      {
        plintf("strong stable %g \n", bigneg);
        get_evec(work, oldwork, b, bp, n, maxit, err, ipivot, bigneg, ierr);
        if (*ierr == 0) {
          change_current_linestyle(StableManifoldColor, &oldcol);
          pr_evec(x, b, n, pr, bigneg, -1);
          DELTA_T = -fabs(DELTA_T);
          shoot(bp, x, b, 1);
          shoot(bp, x, b, -1);
          change_current_linestyle(oldcol, &dummy);
        } else
          err_msg("Failed to compute eigenvector");
      }
    }
    DELTA_T = oldt;
  }

  free(work);
  return;
}

/* this uses the current labeled saddle point stuff to integrate */
void shoot_this_now(void) {
  int i, k, type, oldcol, dummy;
  double x[MAXODE], olddt;
  if (ShootIndex < 1)
    return;
  olddt = DELTA_T;

  for (k = 0; k < ShootIndex; k++) {
    for (i = 0; i < NODE; i++)
      x[i] = ShootIC[k][i];

    type = ShootType[k];
    if (type > 0) {
      change_current_linestyle(UnstableManifoldColor, &oldcol);
      DELTA_T = fabs(DELTA_T);
      shoot_easy(x);
      change_current_linestyle(oldcol, &dummy);
    }
    if (type < 0) {
      change_current_linestyle(StableManifoldColor, &oldcol);
      DELTA_T = -fabs(DELTA_T);
      shoot_easy(x);
      change_current_linestyle(oldcol, &dummy);
    }
  }
  DELTA_T = olddt;
}

/* fixed point with no requests and store manifolds */
void do_sing_info(double *x, double eps, double err, double big, int maxit,
                  int n, double *er, double *em, int *ierr) {
  int kmem, i, j, ipivot[MAXODE];

  int rp = 0, rn = 0, cp = 0, cn = 0, im = 0;
  int pose = 0, nege = 0, pr = 0;
  double *work, *eval, *b, *bp, *oldwork, *ework;
  double temp, old_x[MAXODE];

  double real, imag;
  double bigpos = -1e10, bigneg = 1e10;

  /* float xl[MAXODE]; */
  kmem = n * (2 * n + 5) + 50;
  if ((work = (double *)malloc(sizeof(double) * kmem)) == NULL) {
    /* err_msg("Insufficient core "); */
    return;
  }
  ShootICFlag = 0;
  ShootIndex = 0;
  for (i = 0; i < n; i++)
    old_x[i] = x[i];
  oldwork = work + n * n;
  eval = oldwork + n * n;
  b = eval + 2 * n;
  bp = b + n;
  ework = bp + n;
  rooter(x, err, eps, big, work, ierr, maxit, n);
  if (*ierr != 0) {
    free(work);
    /* err_msg("Could not converge to root"); */
    for (i = 0; i < n; i++)
      x[i] = old_x[i];
    return;
  }

  /* for(i=0;i<n;i++)xl[i]=(float)x[i]; */

  for (i = 0; i < n * n; i++) {
    oldwork[i] = work[i];
    /* plintf("dm=%g\n",oldwork[i]); */
  }
  /* Transpose for Eigen        */
  for (i = 0; i < n; i++) {
    for (j = i + 1; j < n; j++) {
      temp = work[i + j * n];
      work[i + j * n] = work[i * n + j];
      work[i * n + j] = temp;
    }
  }
  eigen(n, work, eval, ework, ierr);
  if (*ierr != 0) {

    free(work);
    return;
  }
  /* succesfully computed evals now lets work with them */

  for (i = 0; i < n; i++) {
    real = eval[2 * i];
    imag = eval[2 * i + 1];
    er[i] = real;
    em[i] = imag;

    if (METHOD == 0)
      real = real * real + imag * imag - 1.00;
    if (fabs(imag) < .00000001)
      imag = 0.0;
    if (real < 0.0) {
      if (imag != 0.0) {
        cn++;
        if (real < bigneg) {
          bigneg = real; /*bneg=-1;Not used*/
        }
      } else {
        rn++;
        nege = i;
        if (real < bigneg) {
          bigneg = real; /*bneg=i;Not used*/
        }
      }
    }
    if (real > 0.0) {
      if (imag != 0.0) {
        cp++;
        if (real > bigpos) {
          bigpos = real; /*bpos=-1;Not used*/
        }
      } else {
        rp++;
        pose = i;
        if (real > bigpos) {
          bigpos = real; /*bpos=i;Not used*/
        }
      }
    }
    if ((real == 0.0) && (imag != 0.0))
      im++;
  } /* eigenvalue count */
  if (((rp + cp) != 0) && ((rn + cn) != 0))
    eq_symb(x, 1);
  else {
    if ((rp + cp) != 0)
      eq_symb(x, 0);
    else
      eq_symb(x, 3);
  }

  /* Lets change Work back to transposed oldwork */
  for (i = 0; i < n; i++) {
    for (j = i + 1; j < n; j++) {
      temp = oldwork[i + j * n];
      work[i + j * n] = oldwork[i * n + j];
      work[i * n + j] = temp;
    }
  }

  if ((n > 1)) {

    if (rp == 1) {
      /* plintf(" One real positive -- pos=%d lam=%g \n",pose,eval[2*pose]); */
      /*     for(i=0;i<n*n;i++)printf(" w=%g o=%g \n",work[i],oldwork[i]); */
      get_evec(work, oldwork, b, bp, n, maxit, err, ipivot, eval[2 * pose],
               ierr);

      if (*ierr == 0) {
        pr_evec(x, b, n, pr, eval[2 * pose], 1);
      }
    }

    if (rn == 1) {

      get_evec(work, oldwork, b, bp, n, maxit, err, ipivot, eval[2 * nege],
               ierr);

      if (*ierr == 0) {
        pr_evec(x, b, n, pr, eval[2 * nege], -1);
      }
    }
  }

  free(work);
  return;
}

static void pr_evec(double *x, double *ev, int n, int pr, double eval,
                    int type) {
  int i;
  double d = fabs(DELTA_T) * .1;
  ShootICFlag = 1;
  if (ShootIndex < 7) {
    for (i = 0; i < n; i++) {
      ShootIC[ShootIndex][i] = x[i] + d * ev[i];
      ShootType[ShootIndex] = type;
      ShootIC[ShootIndex + 1][i] = x[i] - d * ev[i];
      ShootType[ShootIndex + 1] = type;
    }
    ShootIndex += 2;
  }
  if (pr == 0)
    return;
  /* plintf("Initial conditions for %f \n",eval);

  for(i=0;i<n;i++)
  {
   plintf(" %.16g   %.16g   %.16g \n",ev[i],x[i]+d*ev[i],x[i]-d*ev[i]);

  }
  */
}

static void get_evec(double *a, double *anew, double *b, double *bp, int n,
                     int maxit, double err, int *ipivot, double eval,
                     int *ierr) {
  int j, iter, jmax;
  double temp;
  double zz = fabs(eval);
  if (zz < err)
    zz = err;
  *ierr = 0;
  for (j = 0; j < n * n; j++) {
    anew[j] = a[j];
    /*  plintf(" %d %g \n",j,a[j]);   */
  }
  for (j = 0; j < n; j++)
    anew[j * (1 + n)] = anew[j * (1 + n)] - eval - err * err * zz;

  sgefa(anew, n, n, ipivot, ierr);
  if (*ierr != -1) {
    plintf(" Pivot failed\n");
    return;
  }
  for (j = 0; j < n; j++) {
    b[j] = 1 + .1 * ndrand48();
    bp[j] = b[j];
  }
  iter = 0;
  *ierr = 0;
  while (1) {
    sgesl(anew, n, n, ipivot, b, 0);
    temp = fabs(b[0]);
    jmax = 0;

    for (j = 0; j < n; j++) {

      if (fabs(b[j]) > temp) {
        temp = fabs(b[j]);
        jmax = j;
      }
    }
    temp = b[jmax];
    for (j = 0; j < n; j++)
      b[j] = b[j] / temp;
    temp = 0.0;
    for (j = 0; j < n; j++) {
      temp = temp + fabs(b[j] - bp[j]);
      bp[j] = b[j];
    }
    if (temp < err)
      break;
    iter++;
    if (iter > maxit) {
      plintf(" max iterates exceeded\n");

      *ierr = 1;
      break;
    }
  }
  if (*ierr == 0) {
    temp = fabs(b[0]);
    jmax = 0;
    for (j = 0; j < n; j++) {
      if (fabs(b[j]) > temp) {
        temp = fabs(b[j]);
        jmax = j;
      }
    }
    temp = b[jmax];
    for (j = 0; j < n; j++)
      b[j] = b[j] / temp;
  }
  return;
}

double amax(double u, double v) {
  if (u > v)
    return (u);
  return (v);
}

static void getjac(double *x, double *y, double *yp, double *xp, double eps,
                   double *dermat, int n) {
  int i, j, k;
  double r;
  rhs(0.0, x, y, n);
  if (METHOD == 0)
    for (i = 0; i < n; i++)
      y[i] = y[i] - x[i];

  for (i = 0; i < n; i++) {
    /*    plintf(" y=%g x=%g\n",y[i],x[i]); */
    for (k = 0; k < n; k++)
      xp[k] = x[k];
    r = eps * amax(eps, fabs(x[i]));
    xp[i] = xp[i] + r;
    rhs(0.0, xp, yp, n);
    /*
       for(j=0;j<n;j++)
       plintf(" r=%g yp=%g xp=%g\n",r,yp[j],xp[j]);
    */
    if (METHOD == 0) {
      for (j = 0; j < n; j++)
        yp[j] = yp[j] - xp[j];
    }
    for (j = 0; j < n; j++) {
      dermat[j * n + i] = (yp[j] - y[j]) / r;
      /*    plintf("dm=%g \n",dermat[j*n+i]); */
    }
  }
}

void rooter(double *x, double err, double eps, double big, double *work,
            int *ierr, int maxit, int n) {
  int i, iter, ipivot[MAXODE], info;
  char ch;
  double *xp, *yp, *y, *xg, *dermat, *dely;
  double r;
  dermat = work;
  xg = dermat + n * n;
  yp = xg + n;
  xp = yp + n;
  y = xp + n;
  dely = y + n;
  iter = 0;
  *ierr = 0;
  while (1) {
    ch = my_abort();

    {
      if (ch == 27) {
        *ierr = 1;
        return;
      }
      if (ch == '/') {
        *ierr = 1;
        ENDSING = 1;
        return;
      }
      if (ch == 'p')
        PAUSER = 1;
    }

    getjac(x, y, yp, xp, eps, dermat, n);
    sgefa(dermat, n, n, ipivot, &info);
    if (info != -1) {
      *ierr = 1;
      return;
    }
    for (i = 0; i < n; i++)
      dely[i] = y[i];
    sgesl(dermat, n, n, ipivot, dely, 0);
    r = 0.0;
    for (i = 0; i < n; i++) {
      x[i] = x[i] - dely[i];
      r = r + fabs(dely[i]);
    }
    if (r < err) {
      getjac(x, y, yp, xp, eps, dermat, n);
      if (METHOD == 0)
        for (i = 0; i < n; i++)
          dermat[i * (n + 1)] += 1.0;
      /* for(i=0;i<n*n;i++)printf("dm=%g \n",dermat[i]); */
      return; /* success !! */
    }
    if ((r / (double)n) > big) {
      *ierr = 1;
      return;
    }
    iter++;
    if (iter > maxit) {
      *ierr = 1;
      return;
    }
  }
}
