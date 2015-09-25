#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "expr_builtins.h"

#include <math.h>
#include <stdlib.h>

#include "homsup.h"
#include "base/ndrand.h"

/* --- Macros --- */
#define ACC 40.0
#define BIGNO 1.0e10
#define BIGNI 1.0e-10

static double max(double x, double y) { return (((x > y) ? x : y)); }
static double min(double x, double y) { return (((x < y) ? x : y)); }

/* This always gives an answer in [0,y) for mod(x,y) */
static double pmod(double x, double y) {
  double z = fmod(x, y);
  if (z < 0)
    z += y;
  return (z);
}

/*   These are the Bessel Functions; if you dont have them then
     return some sort of dummy value or else write a program
     to compute them
*/

static double bessel_j(double x, double y) {
  int n = (int)x;
  return (jn(n, y));
}

static double bessel_y(double x, double y) {
  int n = (int)x;
  return (yn(n, y));
}

static double bessi0(double x) {
  double ax, ans;
  double y;

  if ((ax = fabs(x)) < 3.75) {
    y = x / 3.75;
    y *= y;
    ans =
        1.0 +
        y * (3.5156229 +
             y * (3.0899424 +
                  y * (1.2067492 +
                       y * (0.2659732 + y * (0.360768e-1 + y * 0.45813e-2)))));
  } else {
    y = 3.75 / ax;
    ans = (exp(ax) / sqrt(ax)) *
          (0.39894228 +
           y * (0.1328592e-1 +
                y * (0.225319e-2 +
                     y * (-0.157565e-2 +
                          y * (0.916281e-2 +
                               y * (-0.2057706e-1 +
                                    y * (0.2635537e-1 +
                                         y * (-0.1647633e-1 +
                                              y * 0.392377e-2))))))));
  }
  return ans;
}

static double bessi1(double x) {
  double ax, ans;
  double y;

  if ((ax = fabs(x)) < 3.75) {
    y = x / 3.75;
    y *= y;
    ans = ax * (0.5 +
                y * (0.87890594 +
                     y * (0.51498869 +
                          y * (0.15084934 +
                               y * (0.2658733e-1 +
                                    y * (0.301532e-2 + y * 0.32411e-3))))));
  } else {
    y = 3.75 / ax;
    ans = 0.2282967e-1 +
          y * (-0.2895312e-1 + y * (0.1787654e-1 - y * 0.420059e-2));
    ans = 0.39894228 +
          y * (-0.3988024e-1 +
               y * (-0.362018e-2 +
                    y * (0.163801e-2 + y * (-0.1031555e-1 + y * ans))));
    ans *= (exp(ax) / sqrt(ax));
  }
  return x < 0.0 ? -ans : ans;
}

static double bessi(double nn, double x) {
  int j, n;
  double bi, bim, bip, tox, ans;
  n = (int)nn;
  if (n == 0)
    return bessi0(x);
  if (n == 1)
    return bessi1(x);
  if (x == 0.0)
    return 0.0;
  else {
    tox = 2.0 / fabs(x);
    bip = ans = 0.0;
    bi = 1.0;
    for (j = 2 * (n + (int)sqrt(ACC * n)); j > 0; j--) {
      bim = bip + j * tox * bi;
      bip = bi;
      bi = bim;
      if (fabs(bi) > BIGNO) {
        ans *= BIGNI;
        bi *= BIGNI;
        bip *= BIGNI;
      }
      if (j == n)
        ans = bip;
    }
    ans *= bessi0(x) / bi;
    return x < 0.0 && (n & 1) ? -ans : ans;
  }
}

static double dand(double x, double y) { return ((double)(x && y)); }
static double dor(double x, double y) { return ((double)(x || y)); }
static double dge(double x, double y) { return ((double)(x >= y)); }
static double dle(double x, double y) { return ((double)(x <= y)); }
static double deq(double x, double y) { return ((double)(x == y)); }
static double dne(double x, double y) { return ((double)(x != y)); }
static double dgt(double x, double y) { return ((double)(x > y)); }
static double dlt(double x, double y) { return ((double)(x < y)); }

double (*expr_fun2[])(double, double) = {
    /* add */ NULL,
    /* sub */ NULL,
    /* mul */ NULL,
    /* div */ NULL, atan2, pow, max, min, pmod, dand, dor, dgt, dlt, deq, dge,
    dle, dne, ndrand48_normal, bessel_j, bessel_y, bessi,
};

static double neg(double z) { return (-z); }
static double recip(double z) { return (1.00 / z); }

static double heaviside(double z) {
  float w = 1.0;
  if (z < 0)
    w = 0.0;
  return (w);
}

static double rndom(double z) { return (z * ndrand48()); }

static double signum(double z) {
  if (z < 0.0)
    return (-1.0);
  if (z > 0.0)
    return (1.0);
  return (0.0);
}

static double dnot(double x) { return ((double)(x == 0.0)); }

#ifndef HAVE_LGAMMA
/* code for log-gamma if you dont have it */
static double lgamma(double xx) {
  double x, y, tmp, ser;
  static double cof[6] = {76.18009172947146,     -86.50532032941677,
                          24.01409824083091,     -1.231739572450155,
                          0.1208650973866179e-2, -0.5395239384953e-5};
  int j;

  y = x = xx;
  tmp = x + 5.5;
  tmp -= (x + 0.5) * log(tmp);
  ser = 1.000000000190015;
  for (j = 0; j <= 5; j++)
    ser += cof[j] / ++y;
  return -tmp + log(2.5066282746310005 * ser / x);
}
#endif /* !HAVE_LGAMMA */

double (*expr_fun1[])(double) = {
    sin, cos, tan, asin, acos, atan, sinh, tanh, cosh, fabs, exp, log, log10,
    sqrt, neg, recip, heaviside, signum, floor, rndom, dnot, erf, erfc, hom_bcs,
    ndrand48_poidev, lgamma,
};
