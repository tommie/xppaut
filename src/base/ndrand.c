#include "ndrand.h"

#include <math.h>

/* --- Macros --- */
#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)
#define EPS 1.2e-12
#define RNMX (1.0 - EPS)
#define PI 3.1415926

/* --- Types --- */
typedef struct {
  int v2_valid;
  double v2;
} BoxMullerContext;

/* --- Data --- */
static BoxMullerContext g_ctx;
static long g_seed = -1;

static double ran1(long *idum) {
  int j;
  long k;
  static long iy = 0;
  static long iv[NTAB];
  double temp;

  if (*idum <= 0 || !iy) {
    if (-(*idum) < 1)
      *idum = 1;
    else
      *idum = -(*idum);
    for (j = NTAB + 7; j >= 0; j--) {
      k = (*idum) / IQ;
      *idum = IA *(*idum - k * IQ) - IR * k;
      if (*idum < 0)
        *idum += IM;
      if (j < NTAB)
        iv[j] = *idum;
    }
    iy = iv[0];
  }
  k = (*idum) / IQ;
  *idum = IA *(*idum - k * IQ) - IR * k;
  if (*idum < 0)
    *idum += IM;
  j = iy / NDIV;
  iy = iv[j];
  iv[j] = *idum;
  if ((temp = AM * iy) > RNMX)
    return RNMX;
  else
    return temp;
}

double ndrand48(void) { return ran1(&g_seed); }
void nsrand48(long seed) { g_seed = -seed; }

static double box_muller_next(BoxMullerContext *ctx, double mean, double std) {
  double fac, r, v1, v2;

  ctx->v2_valid = !ctx->v2_valid;

  if (!ctx->v2_valid)
    return ctx->v2 * std + mean;

  do {
    v1 = 2.0 * ndrand48() - 1.0;
    v2 = 2.0 * ndrand48() - 1.0;
    r = v1 * v1 + v2 * v2;
  } while (r >= 1.0);

  fac = sqrt(-2.0 * log(r) / r);
  ctx->v2 = v2 * fac;

  return v1 * fac * std + mean;
}

double ndrand48_normal(double mean, double std) {
  return box_muller_next(&g_ctx, mean, std);
}

static double gammln(double xx) {
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

double ndrand48_poidev(double xm) {
  static double sq, alxm, g, oldm = (-1.0);

  double em, t, y;

  if (xm < 12.0) {
    if (xm != oldm) {
      oldm = xm;
      g = exp(-xm);
    }
    em = -1;
    t = 1.0;
    do {
      ++em;
      t *= ndrand48();
    } while (t > g);
  } else {
    if (xm != oldm) {
      oldm = xm;
      sq = sqrt(2.0 * xm);
      alxm = log(xm);
      g = xm * alxm - gammln(xm + 1.0);
    }
    do {
      do {
        y = tan(PI * ndrand48());
        em = sq * y + xm;
      } while (em < 0.0);
      em = floor(em);
      t = 0.9 * (1.0 + y * y) * exp(em * alxm - gammln(em + 1.0) - g);
    } while (ndrand48() > t);
  }
  return em;
}
