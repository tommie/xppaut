#include "timeutil.h"

#include <stdlib.h>
#include <sys/time.h>

int gettimenow(void) {
  struct timeval now;
  /* Excerpt from the man (Section 2) for  gettimeofday:
     "The use of the timezone structure is obsolete; the tz argument should
     normally be spec-
     ified as NULL.  The tz_dsttime field has never been used under Linux; it
     has  not  been
     and will not be supported by libc or glibc.  Each and every occurrence of
     this field in
     the kernel source (other than the declaration) is a bug."
   */
  gettimeofday(&now, NULL);
  return now.tv_usec;
}

void waitasec(int msec) {
  struct timeval tim;
  double sec = (double)msec / 1000;
  double t1, t2;
  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);

  while (1) {
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec + (tim.tv_usec / 1000000.0);

    if ((t2 - t1) > sec)
      return;
  }
}
