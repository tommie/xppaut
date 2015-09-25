#ifndef XPPAUT_BASE_NDRAND_H
#define XPPAUT_BASE_NDRAND_H

/* --- Functions --- */
double ndrand48(void);
double ndrand48_normal(double mean, double std);
double ndrand48_poidev(double xm);
void nsrand48(long seed);

#endif /* XPPAUT_BASE_NDRAND_H */
