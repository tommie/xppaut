#ifndef XPPAUT_BASE_TIMEUTIL_H
#define XPPAUT_BASE_TIMEUTIL_H

#include <sys/time.h>

/* --- Functions --- */
int gettimenow(void);
void waitasec(int msec);

/**
 * Returns a < b.
 *
 * @param a the first time.
 * @param b the second time.
 * @return zero or non-zero.
 */
static inline int timeval_less(const struct timeval *a,
                               const struct timeval *b) {
  return a->tv_sec < b->tv_sec ||
         (a->tv_sec == b->tv_sec && a->tv_usec < b->tv_usec);
}

/**
 * Computes a += b.
 *
 * @param a the destination time.
 * @param b the second source time.
 */
static inline void timeval_add(struct timeval *a, const struct timeval *b) {
  a->tv_usec += b->tv_usec;
  a->tv_sec += b->tv_sec + a->tv_usec / 1000000;
  a->tv_usec %= 1000000;
}

/**
 * Computes a += b, rounding b correctly.
 *
 * @param a the destination time.
 * @param b the second source time.
 */
static inline void timeval_add_ts(struct timeval *a, const struct timespec *b) {
  long fivehundred = b->tv_nsec < 0 ? -500 : 500;

  a->tv_usec += (b->tv_nsec + fivehundred) / 1000;
  if (a->tv_usec < 0) {
    --a->tv_sec;
    a->tv_usec += 1000000;
  }
  a->tv_sec += b->tv_sec + a->tv_usec / 1000000;
  a->tv_usec %= 1000000;
}

#endif /* XPPAUT_BASE_TIMEUTIL_H */
