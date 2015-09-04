#include "gear.h"

#include <math.h>

#include "flags.h"
#include "odesol2.h"
#include "xpplim.h"

/* --- Forward Declarations --- */
static double Max(double x, double y);
static double Min(double x, double y);
static int isamax(int n, double *sx, int incx);
static void saxpy(int n, double sa, double *sx, int incx, double *sy, int incy);
static double sdot(int n, double *sx, int incx, double *sy, int incy);
static double sgnum(double x, double y);
static double sqr2(double z);
static void sscal(int n, double sa, double *sx, int incx);

/* --- Data --- */
static const double PERTST[7][2][3] = {
    {{2, 3, 1}, {2, 12, 1}},
    {{4.5, 6, 1}, {12, 24, 1}},
    {{7.333, 9.167, .5}, {24, 37.89, 2}},
    {{10.42, 12.5, .1667}, {37.89, 53.33, 1}},
    {{13.7, 15.98, .04133}, {53.33, 70.08, .3157}},
    {{17.15, 1, .008267}, {70.08, 87.97, .07407}},
    {{1, 1, 1}, {87.97, 1, .0139}}};
static int gear_pivot[MAXODE];

int gear(int n, double *t, double tout, double *y, double hmin, double hmax,
         double eps, int mf, double *error, int *kflag, int *jstart,
         double *work, int *iwork) {
  if (NFlags == 0)
    return (ggear(n, t, tout, y, hmin, hmax, eps, mf, error, kflag, jstart,
                  work, iwork));
  return (one_flag_step_gear(n, t, tout, y, hmin, hmax, eps, mf, error, kflag,
                             jstart, work, iwork));
}

int ggear(int n, double *t, double tout, double *y, double hmin, double hmax,
          double eps, int mf, double *error, int *kflag, int *jstart,
          double *work, int *iwork) {
  /* int ipivot[MAXODE]; */
  double deltat = 0.0, hnew = 0.0, hold = 0.0, h = 0.0, racum = 0.0, told = 0.0,
         r = 0.0, d = 0.0;
  double *a, pr1, pr2, pr3, r1;
  double *dermat, *save[8], *save9, *save10, *save11, *save12;
  double enq1 = 0.0, enq2 = 0.0, enq3 = 0.0, pepsh = 0.0, e = 0.0, edwn = 0.0,
         eup = 0.0, bnd = 0.0;
  double *ytable[8], *ymax, *work2;
  int i, iret = 0, maxder = 0, j = 0, k = 0, iret1 = 0, nqold = 0, nq = 0,
         newq = 0;
  int idoub = 0, mtyp = 0, iweval = 0, j1 = 0, j2 = 0, l = 0, info = 0, job = 0,
      nt = 0;
  /* plintf("entering gear ... with start=%d \n",*jstart);*/
  for (i = 0; i < 8; i++) {
    save[i] = work + i * n;
    ytable[i] = work + (8 + i) * n;
  }
  save9 = work + 16 * n;
  save10 = work + 17 * n;
  save11 = work + 18 * n;
  save12 = work + 19 * n;
  ymax = work + 20 * n;
  dermat = work + 21 * n;
  a = work + 21 * n + n * n;
  work2 = work + 21 * n + n * n + 10;
  if (*jstart != 0) {

    k = iwork[0];
    nq = iwork[1];
    nqold = iwork[2];
    idoub = iwork[3];
    maxder = 6;
    mtyp = 1;
    iret1 = iwork[4];
    iret = iwork[5];
    newq = iwork[6];
    iweval = iwork[7];
    hold = work2[1];
    h = work2[0];
    hnew = work2[2];
    told = work2[3];
    racum = work2[4];
    enq1 = work2[5];
    enq2 = work2[6];
    enq3 = work2[7];
    pepsh = work2[8];
    e = work2[9];
    edwn = work2[10];
    eup = work2[11];
    bnd = work2[12];
  }
  deltat = tout - *t;
  if (*jstart == 0)
    h = sgnum(hmin, deltat);
  if (fabs(deltat) < hmin) {
    return (-1);
  }
  maxder = 6;
  for (i = 0; i < n; i++)
    ytable[0][i] = y[i];

L70:

  iret = 1;
  *kflag = 1;
  if ((h > 0.0) && ((*t + h) > tout))
    h = tout - *t;
  if ((h < 0.0) && ((*t + h) < tout))
    h = tout - *t;
  if (*jstart <= 0)
    goto L120;

L80:

  for (i = 0; i < n; i++)
    for (j = 1; j <= k; j++)
      save[j - 1][i] = ytable[j - 1][i];

  hold = hnew;
  if (h == hold)
    goto L110;

L100:

  racum = h / hold;
  iret1 = 1;
  goto L820;

L110:

  nqold = nq;
  told = *t;
  racum = 1.0;
  if (*jstart > 0)
    goto L330;
  goto L150;

L120:

  if (*jstart == -1)
    goto L140;

  nq = 1;

  rhs(*t, ytable[0], save11, n);

  for (i = 0; i < n; i++) {
    ytable[1][i] = save11[i] * h;
    ymax[i] = 1.00;
  }

  hnew = h;
  k = 2;
  goto L80;

L140:

  if (nq == nqold)
    *jstart = 1;
  *t = told;
  nq = nqold;
  k = nq + 1;
  goto L100;

L150:

  if (nq > 6) {
    *kflag = -2;
    goto L860;
  }
  switch (nq) {

  case 1:
    a[0] = -1.00;
    a[1] = -1.00;
    break;
  case 2:
    a[0] = -2.0 / 3.0;
    a[1] = -1.00;
    a[2] = -1.0 / 3.0;
    break;
  case 3:
    a[0] = -6.0 / 11.0;
    a[1] = -1.00;
    a[2] = a[0];
    a[3] = -1.0 / 11.0;
    break;
  case 4:
    a[0] = -.48;
    a[1] = -1.00;
    a[2] = -.70;
    a[3] = -.2;
    a[4] = -.02;
    break;
  case 5:
    a[0] = -120.0 / 274.;
    a[1] = -1.00;
    a[2] = -225. / 274.;
    a[3] = -85. / 274.;
    a[4] = -15. / 274.;
    a[5] = -1. / 274.;
    break;
  case 6:
    a[0] = -180. / 441.;
    a[1] = -1.0;
    a[2] = -58. / 63.;
    a[3] = -5. / 12.;
    a[4] = -25. / 252.;
    a[5] = -3. / 252.;
    a[6] = -1. / 1764;
    break;
  }

  /*L310:*/

  k = nq + 1;
  idoub = k;
  mtyp = (4 - mf) / 2;
  enq2 = .5 / (double)(nq + 1);
  enq3 = .5 / (double)(nq + 2);
  enq1 = .5 / (double)nq;
  pepsh = eps;
  eup = sqr2(PERTST[nq - 1][0][1] * pepsh);
  e = sqr2(PERTST[nq - 1][0][0] * pepsh);
  edwn = sqr2(PERTST[nq - 1][0][2] * pepsh);
  if (edwn == 0.0)
    goto L850;
  bnd = eps * enq3 / (double)n;

  /*L320:*/

  iweval = 2;
  if (iret == 2)
    goto L750;

L330:

  *t = *t + h;
  for (j = 2; j <= k; j++)
    for (j1 = j; j1 <= k; j1++) {
      j2 = k - j1 + j - 1;
      for (i = 0; i < n; i++)
        ytable[j2 - 1][i] = ytable[j2 - 1][i] + ytable[j2][i];
    }
  for (i = 0; i < n; i++)
    error[i] = 0.0;
  for (l = 0; l < 3; l++) {
    rhs(*t, ytable[0], save11, n);
    if (iweval < 1) {
      /*  plintf("iweval=%d \n",iweval);
        for(i=0;i<n;i++)printf("up piv = %d \n",gear_pivot[i]);*/
      goto L460;
    }
    /*       JACOBIAN COMPUTED   */
    for (i = 0; i < n; i++)
      save9[i] = ytable[0][i];
    for (j = 0; j < n; j++) {
      r = eps * Max(eps, fabs(save9[j]));
      ytable[0][j] = ytable[0][j] + r;
      d = a[0] * h / r;
      rhs(*t, ytable[0], save12, n);
      for (i = 0; i < n; i++)
        dermat[n * i + j] = (save12[i] - save11[i]) * d;
      ytable[0][j] = save9[j];
    }
    for (i = 0; i < n; i++)
      dermat[n * i + i] += 1.0;
    iweval = -1;
    /*      plintf(" Jac = %f %f %f %f \n",dermat[0],dermat[1],dermat[2],dermat[3]);
*/ sgefa(
        dermat, n, n, gear_pivot, &info);
    /* for(i=0;i<n;i++)printf("gear_pivot[%d]=%d \n",i,gear_pivot[i]);*/
    if (info == -1)
      j1 = 1;
    else
      j1 = -1;
    if (j1 < 0)
      goto L520;

  L460:

    for (i = 0; i < n; i++)
      save12[i] = ytable[1][i] - save11[i] * h;
    for (i = 0; i < n; i++)
      save9[i] = save12[i];
    job = 0;
    sgesl(dermat, n, n, gear_pivot, save9, job);
    nt = n;
    for (i = 0; i < n; i++) {
      ytable[0][i] = ytable[0][i] + a[0] * save9[i];
      ytable[1][i] = ytable[1][i] - save9[i];
      error[i] += save9[i];
      if (fabs(save9[i]) <= (bnd * ymax[i]))
        nt--;
    }
    if (nt <= 0)
      goto L560;
  }

L520:

  /*        UH Oh */
  *t = told;
  if ((h <= (hmin * 1.000001)) && ((iweval - mtyp) < -1))
    goto L530;
  if (iweval != 0)
    racum *= .25;
  iweval = mf;
  iret1 = 2;
  goto L820;

L530:

  *kflag = -3;

L540:

  for (i = 0; i < n; i++)
    for (j = 1; j <= k; j++)
      ytable[j - 1][i] = save[j - 1][i];
  h = hold;
  nq = nqold;
  *jstart = nq;
  goto L860;

L560:

  d = 0.0;
  for (i = 0; i < n; i++)
    d += sqr2(error[i] / ymax[i]);
  iweval = 0;
  if (d > e)
    goto L610;
  if (k >= 3) {
    for (j = 3; j <= k; j++)
      for (i = 0; i < n; i++)
        ytable[j - 1][i] = ytable[j - 1][i] + a[j - 1] * error[i];
  }
  *kflag = 1;
  hnew = h;
  if (idoub <= 1)
    goto L620;
  idoub--;
  if (idoub <= 1)
    for (i = 0; i < n; i++)
      save10[i] = error[i];
  goto L770;

L610:

  *kflag -= 2;
  if (h <= hmin * 1.00001)
    goto L810;
  *t = told;
  if (*kflag <= -5)
    goto L790;

L620:

  pr2 = 1.2 * pow(d / e, enq2);
  pr3 = 1.0e20;
  if ((nq < maxder) && (*kflag > -1)) {
    d = 0.0;
    for (i = 0; i < n; i++)
      d += sqr2((error[i] - save10[i]) / ymax[i]);
    pr3 = 1.4 * pow(d / eup, enq3);
  }
  pr1 = 1.0e20;
  if (nq > 1) {
    d = 0.0;
    for (i = 0; i < n; i++)
      d += sqr2(ytable[k - 1][i] / ymax[i]);
    pr1 = 1.3 * pow(d / edwn, enq1);
  }
  if (pr2 <= pr3)
    goto L720;
  if (pr3 < pr1)
    goto L730;

L670:

  r = 1.0 / Max(pr1, 0.0001);
  newq = nq - 1;

L680:

  idoub = 10;
  if ((*kflag == 1) && (r < 1.1))
    goto L770;
  if (newq <= nq)
    goto L700;
  for (i = 0; i < n; i++)
    ytable[newq][i] = error[i] * a[k - 1] / (double)k;

L700:

  k = newq + 1;
  if (*kflag == 1)
    goto L740;
  racum = racum * r;
  iret1 = 3;
  goto L820;

L710:

  if (newq == nq)
    goto L330;
  nq = newq;
  goto L150;

L720:

  if (pr2 > pr1)
    goto L670;
  newq = nq;
  r = 1.0 / Max(pr2, .0001);
  goto L680;

L730:

  r = 1.0 / Max(pr3, .0001);
  newq = nq + 1;
  goto L680;

L740:

  iret = 2;
  h = h * r;
  hnew = h;
  if (nq == newq)
    goto L750;
  nq = newq;
  goto L150;

L750:

  r1 = 1.0;
  for (j = 2; j <= k; j++) {
    r1 = r1 * r;
    for (i = 0; i < n; i++)
      ytable[j - 1][i] = ytable[j - 1][i] * r1;
  }
  idoub = k;

L770:

  for (i = 0; i < n; i++)
    ymax[i] = Max(ymax[i], fabs(ytable[0][i]));
  *jstart = nq;
  if ((h > 0.0) && (*t >= tout))
    goto L860;
  if ((h < 0.0) && (*t <= tout))
    goto L860;
  goto L70;

L790:

  if (nq == 1)
    goto L850;
  rhs(*t, ytable[0], save11, n);
  r = h / hold;
  for (i = 0; i < n; i++) {
    ytable[0][i] = save[0][i];
    save[1][i] = hold * save11[i];
    ytable[1][i] = r * save[1][i];
  }
  nq = 1;
  *kflag = 1;
  goto L150;

L810:

  *kflag = -1;
  hnew = h;
  *jstart = nq;
  goto L860;

L820:

  racum = Max(fabs(hmin / hold), racum);
  racum = Min(racum, fabs(hmax / hold));
  r1 = 1.0;
  for (j = 2; j <= k; j++) {
    r1 = r1 * racum;
    for (i = 0; i < n; i++)
      ytable[j - 1][i] = save[j - 1][i] * r1;
  }
  h = hold * racum;
  for (i = 0; i < n; i++)
    ytable[0][i] = save[0][i];
  idoub = k;
  if (iret1 == 1)
    goto L110;
  if (iret1 == 2)
    goto L330;
  if (iret1 == 3)
    goto L710;

L850:

  *kflag = -4;

  goto L540;

L860:
  for (i = 0; i < n; i++)
    y[i] = ytable[0][i];
  iwork[0] = k;
  iwork[1] = nq;
  iwork[2] = nqold;
  work2[0] = h;
  work2[1] = hold;
  work2[2] = hnew;
  work2[3] = told;
  work2[4] = racum;
  work2[5] = enq1;
  work2[6] = enq2;
  work2[7] = enq3;
  work2[8] = pepsh;
  work2[9] = e;
  work2[10] = edwn;
  work2[11] = eup;
  work2[12] = bnd;
  iwork[3] = idoub;
  iwork[4] = iret1;
  iwork[5] = iret;
  iwork[6] = newq;
  iwork[7] = iweval;

  return (1);
}

static double sqr2(double z) { return (z * z); }

static double sgnum(double x, double y) {
  if (y < 0.0)
    return (-fabs(x));
  else
    return (fabs(x));
}

static double Max(double x, double y) {
  if (x > y)
    return (x);
  return (y);
}

static double Min(double x, double y) {
  if (x < y)
    return (x);
  return (y);
}

void sgefa(double *a, int lda, int n, int *ipvt, int *info) {
  int j, k, kp1, l, nm1;
  double t;
  *info = -1;
  nm1 = n - 1;
  if (nm1 > 0) {
    for (k = 1; k <= nm1; k++) {
      kp1 = k + 1;
      l = isamax(n - k + 1, &a[(k - 1) * lda + k - 1], lda) + k - 1;
      ipvt[k - 1] = l;
      if (a[l * lda + k - 1] != 0.0) {
        if (l != (k - 1)) {
          t = a[l * lda + k - 1];
          a[l * lda + k - 1] = a[(k - 1) * lda + k - 1];
          a[(k - 1) * lda + k - 1] = t;
        }
        t = -1.0 / a[(k - 1) * lda + k - 1];
        sscal(n - k, t, (a + k * lda + k - 1), lda);
        for (j = kp1; j <= n; j++) {
          t = a[l * lda + j - 1];
          if (l != (k - 1)) {
            a[l * lda + j - 1] = a[(k - 1) * lda + j - 1];
            a[(k - 1) * lda + j - 1] = t;
          }
          saxpy(n - k, t, (a + k * lda + k - 1), lda, (a + k * lda + j - 1),
                lda);
        }
      } else
        *info = k - 1;
    }
  }
  ipvt[n - 1] = n - 1;
  if (a[(n - 1) * lda + n - 1] == 0.0)
    *info = n - 1;
}

void sgesl(double *a, int lda, int n, int *ipvt, double *b, int job) {
  int k, kb, l, nm1;
  double t;
  nm1 = n - 1;
  /* for(k=0;k<n;k++)printf("ipiv=%d  b=%f \n",
                          ipvt[k],b[k]);*/

  if (job == 0) {
    if (nm1 >= 1) {
      for (k = 1; k <= nm1; k++) {
        l = ipvt[k - 1];
        t = b[l];
        if (l != (k - 1)) {
          b[l] = b[k - 1];
          b[k - 1] = t;
        }
        saxpy(n - k, t, (a + lda * k + k - 1), lda, (b + k), 1);
      }
    }
    for (kb = 1; kb <= n; kb++) {
      k = n + 1 - kb;
      b[k - 1] = b[k - 1] / a[(k - 1) * lda + k - 1];
      t = -b[k - 1];
      saxpy(k - 1, t, (a + k - 1), lda, b, 1);
    }
    return;
  }
  for (k = 1; k <= n; k++) {
    t = sdot(k - 1, (a + k - 1), lda, b, 1);
    b[k - 1] = (b[k - 1] - t) / a[(k - 1) * lda + k - 1];
  }
  if (nm1 > 0) {
    for (kb = 1; kb <= nm1; kb++) {
      k = n - kb;
      b[k - 1] = b[k - 1] + sdot(n - k, (a + k * lda + k - 1), lda, b + k, 1);
      l = ipvt[k - 1];
      if (l != (k - 1)) {
        t = b[l];
        b[l] = b[k - 1];
        b[k - 1] = t;
      }
    }
  }
}

static void saxpy(int n, double sa, double *sx, int incx, double *sy,
                  int incy) {
  int i, ix, iy;
  if (n <= 0)
    return;
  if (sa == 0.0)
    return;
  ix = 0;
  iy = 0;
  if (incx < 0)
    ix = -n * incx;
  if (incy < 0)
    iy = -n * incy;
  for (i = 0; i < n; i++, ix += incx, iy += incy)
    sy[iy] = sy[iy] + sa * sx[ix];
}

static int isamax(int n, double *sx, int incx) {
  int i, ix, imax;
  double smax;
  if (n < 1)
    return (-1);
  if (n == 1)
    return (0);
  if (incx != 1) {
    ix = 0;
    imax = 0;
    smax = fabs(sx[0]);
    ix += incx;
    for (i = 1; i < n; i++, ix += incx) {
      if (fabs(sx[ix]) > smax) {
        imax = i;
        smax = fabs(sx[ix]);
      }
    }
    return (imax);
  }
  imax = 0;
  smax = fabs(sx[0]);
  for (i = 1; i < n; i++) {
    if (fabs(sx[i]) > smax) {
      imax = i;
      smax = fabs(sx[i]);
    }
  }
  return (imax);
}

static double sdot(int n, double *sx, int incx, double *sy, int incy) {
  int i, ix, iy;
  double stemp = 0.0;
  if (n <= 0)
    return (0.0);
  ix = 0;
  iy = 0;
  if (incx < 0)
    ix = -n * incx;
  if (incy < 0)
    iy = -n * incy;
  for (i = 0; i < n; i++, ix += incx, iy += incy)
    stemp += sx[ix] * sx[iy];
  return (stemp);
}

static void sscal(int n, double sa, double *sx, int incx) {
  int i, nincx;
  if (n <= 0)
    return;
  nincx = n * incx;
  for (i = 0; i < nincx; i += incx)
    sx[i] *= sa;
}
