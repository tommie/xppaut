#include "eigen.h"

#include <math.h>

/* --- Forward Declarations --- */
static void hqr(int n, int low, int igh, double *h, double *ev, int *ierr);
static void orthes(int n, int low, int igh, double *a, double *ort);
static int imin(int x, int y);
static double sign(double x, double y);

void eigen(int n, double *a, double *ev, double *work, int *ierr) {
  orthes(n, 1, n, a, work);
  hqr(n, 1, n, a, ev, ierr);
}

static void hqr(int n, int low, int igh, double *h, double *ev, int *ierr) {
  int i, j, k, l = 0, m = 0, en, ll, mm, na, its, mp2, enm2;
  double p = 0.0, q = 0.0, r = 0.0, s, t, w, x, y, zz, norm, machep = 1.e-10;
  int notlas;
  *ierr = 0;
  norm = 0.0;
  k = 1;
  for (i = 1; i <= n; i++) {
    for (j = k; j <= n; j++)
      norm = norm + fabs(h[i - 1 + (j - 1) * n]);
    k = i;
    if ((i >= low) && (i <= igh))
      continue;
    ev[(i - 1) * 2] = h[i - 1 + (i - 1) * n];
    ev[1 + (i - 1) * 2] = 0.0;
  }
  en = igh;
  t = 0.0;
l60:
  if (en < low)
    return;
  its = 0;
  na = en - 1;
  enm2 = na - 1;
l70:
  for (ll = low; ll <= en; ll++) {
    l = en + low - ll;
    if (l == low)
      break;
    s = fabs(h[l - 2 + (l - 2) * n]) + fabs(h[l - 1 + (l - 1) * n]);
    if (s == 0.0)
      s = norm;
    if (fabs(h[l - 1 + (l - 2) * n]) <= machep * s)
      break;
  }
  x = h[en - 1 + (en - 1) * n];
  if (l == en)
    goto l270;
  y = h[na - 1 + (na - 1) * n];
  w = h[en - 1 + (na - 1) * n] * h[na - 1 + (en - 1) * n];
  if (l == na)
    goto l280;
  if (its == 30)
    goto l1000;
  if ((its != 10) && (its != 20))
    goto l130;
  t = t + x;
  for (i = low; i <= en; i++)
    h[i - 1 + (i - 1) * n] = h[i - 1 + (i - 1) * n] - x;
  s = fabs(h[en - 1 + (na - 1) * n]) + fabs(h[na - 1 + (enm2 - 1) * n]);
  x = 0.75 * s;
  y = x;
  w = -0.4375 * s * s;
l130:
  its++; /*its = its++; This may be undefined. Use its++ instead.*/
  for (mm = l; mm <= enm2; mm++) {
    m = enm2 + l - mm;
    zz = h[m - 1 + (m - 1) * n];
    r = x - zz;
    s = y - zz;
    p = (r * s - w) / h[m + (m - 1) * n] + h[m - 1 + m * n];
    q = h[m + m * n] - zz - r - s;
    r = h[m + 1 + m * n];
    s = fabs(p) + fabs(q) + fabs(r);
    p = p / s;
    q = q / s;
    r = r / s;
    if (m == l)
      break;
    if ((fabs(h[m - 1 + (m - 2) * n]) * (fabs(q) + fabs(r))) <=
        (machep * fabs(p) *
         (fabs(h[m - 2 + (m - 2) * n]) + fabs(zz) + fabs(h[m + m * n]))))
      break;
  }
  mp2 = m + 2;
  for (i = mp2; i <= en; i++) {
    h[i - 1 + (i - 3) * n] = 0.0;
    if (i == mp2)
      continue;
    h[i - 1 + (i - 4) * n] = 0.0;
  }
  for (k = m; k <= na; k++) /*260 */
  {
    notlas = 0;
    if (k != na)
      notlas = 1;
    if (k == m)
      goto l170;
    p = h[k - 1 + (k - 2) * n];
    q = h[k + (k - 2) * n];
    r = 0.0;
    if (notlas)
      r = h[k + 1 + (k - 2) * n];
    x = fabs(p) + fabs(q) + fabs(r);
    if (x == 0.0)
      continue;
    p = p / x;
    q = q / x;
    r = r / x;
  l170:
    s = sign(sqrt(p * p + q * q + r * r), p);
    if (k != m)
      h[k - 1 + (k - 2) * n] = -s * x;
    else if (l != m)
      h[k - 1 + (k - 2) * n] = -h[k - 1 + (k - 2) * n];
    p = p + s;
    x = p / s;
    y = q / s;
    zz = r / s;
    q = q / p;
    r = r / p;
    for (j = k; j <= en; j++) {
      p = h[k - 1 + (j - 1) * n] + q * h[k + (j - 1) * n];
      if (notlas) {
        p = p + r * h[k + 1 + (j - 1) * n];
        h[k + 1 + (j - 1) * n] = h[k + 1 + (j - 1) * n] - p * zz;
      }
      h[k + (j - 1) * n] = h[k + (j - 1) * n] - p * y;
      h[k - 1 + (j - 1) * n] = h[k - 1 + (j - 1) * n] - p * x;
    }
    j = imin(en, k + 3);
    for (i = l; i <= j; i++) {
      p = x * h[i - 1 + (k - 1) * n] + y * h[i - 1 + k * n];
      if (notlas) {
        p = p + zz * h[i - 1 + (k + 1) * n];
        h[i - 1 + (k + 1) * n] = h[i - 1 + (k + 1) * n] - p * r;
      }
      h[i - 1 + k * n] = h[i - 1 + k * n] - p * q;
      h[i - 1 + (k - 1) * n] = h[i - 1 + (k - 1) * n] - p;
    }
  }
  goto l70;
l270:
  ev[(en - 1) * 2] = x + t;
  ev[1 + (en - 1) * 2] = 0.0;
  en = na;
  goto l60;
l280:
  p = (y - x) / 2.0;
  q = p * p + w;
  zz = sqrt(fabs(q));
  x = x + t;
  if (q < 0.0)
    goto l320;
  zz = p + sign(zz, p);
  ev[(na - 1) * 2] = x + zz;
  ev[(en - 1) * 2] = ev[(na - 1) * 2];
  if (zz != 0.0)
    ev[(en - 1) * 2] = x - w / zz;
  ev[1 + (na - 1) * 2] = 0.0;
  ev[1 + (en - 1) * 2] = 0.0;
  goto l330;
l320:
  ev[(na - 1) * 2] = x + p;
  ev[(en - 1) * 2] = x + p;
  ev[1 + (na - 1) * 2] = zz;
  ev[1 + (en - 1) * 2] = -zz;
l330:
  en = enm2;
  goto l60;

l1000:
  *ierr = en;
}

static void orthes(int n, int low, int igh, double *a, double *ort) {
  int i, j, m, ii, jj, la, mp, kp1;
  double f, g, h, scale;
  la = igh - 1;
  kp1 = low + 1;
  if (la < kp1)
    return;
  for (m = kp1; m <= la; m++) /*180*/
  {
    h = 0.0;
    ort[m - 1] = 0.0;
    scale = 0.0;
    for (i = m; i <= igh; i++)
      scale = scale + fabs(a[i - 1 + (m - 2) * n]);
    if (scale == 0.0)
      continue;
    mp = m + igh;
    for (ii = m; ii <= igh; ii++) /*100*/
    {
      i = mp - ii;
      ort[i - 1] = a[i - 1 + (m - 2) * n] / scale;
      h = h + ort[i - 1] * ort[i - 1];
    }
    g = -sign(sqrt(h), ort[m - 1]);
    h = h - ort[m - 1] * g;
    ort[m - 1] = ort[m - 1] - g;
    for (j = m; j <= n; j++) /*130 */
    {
      f = 0.0;
      for (ii = m; ii <= igh; ii++) {
        i = mp - ii;
        f = f + ort[i - 1] * a[i - 1 + (j - 1) * n];
      }
      f = f / h;
      for (i = m; i <= igh; i++)
        a[i - 1 + (j - 1) * n] = a[i - 1 + (j - 1) * n] - f * ort[i - 1];
    }
    for (i = 1; i <= igh; i++) /*160*/
    {
      f = 0.0;
      for (jj = m; jj <= igh; jj++) /*140 */
      {
        j = mp - jj;
        f = f + ort[j - 1] * a[i - 1 + (j - 1) * n];
      }
      f = f / h;
      for (j = m; j <= igh; j++)
        a[i - 1 + (j - 1) * n] = a[i - 1 + (j - 1) * n] - f * ort[j - 1];
    }
    ort[m - 1] = scale * ort[m - 1];
    a[m - 1 + (m - 2) * n] = scale * g;
  }
}

static int imin(int x, int y) {
  if (x < y)
    return (x);
  return (y);
}

static double sign(double x, double y) {
  if (y >= 0.0)
    return (fabs(x));
  return (-fabs(x));
}
