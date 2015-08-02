#include "autevd.h"
#undef abs

#include <stdlib.h>

#include "auto_define.h"
#include "auto_nox.h"
#include "auto_x11.h"
#include "diagram.h"

/* --- Macros --- */
#define EPSU bleps_1.epsu
#define EPSS bleps_1.epss
#define EPSL(a) bleps_1.epsl[(a)]

#define IRS blbcn_1.irs
#define NDIM blbcn_1.ndim
#define IPS blbcn_1.ips
#define ILP blbcn_1.ilp

#define NTST blcde_1.ntst
#define NCOL blcde_1.ncol
#define IAD blcde_1.iad
#define ISP blcde_1.isp
#define ISW blcde_1.isw
#define NBC blcde_1.nbc
#define NIC blcde_1.nint

#define DS bldls_1.ds
#define DSMAX bldls_1.dsmax
#define DSMIN bldls_1.dsmin

#define NMX bllim_1.nmx
#define NUZR bllim_1.nuzr
#define RL0 bllim_1.rl0
#define RL1 bllim_1.rl1
#define AUTO_A0 bllim_1.a0
#define AUTO_A1 bllim_1.a1

void init_auto(int ndim, int nbc, int ips, int irs, int ilp, int ntst, int isp,
               int isw, int nmx, int npr, double ds, double dsmin, double dsmax,
               double rl0, double rl1, double a0, double a1, int ip1, int ip2,
               int ip3, int ip4, int ip5, int nuzr, double epsl, double epsu,
               double epss, int ncol) {
  int i;

  NUZR = nuzr;
  NDIM = ndim;

  IPS = ips;
  IRS = irs;
  ILP = ilp;
  ISP = isp;

  NBC = nbc;
  NIC = ndim - nbc; /* here is weher the integral conditions come in!!! */
  blbcn_1.icp[0] = ip1 + 1;
  blbcn_1.icp[1] = ip2 + 1;
  blbcn_1.icp[2] = ip3 + 1;
  blbcn_1.icp[3] = ip4 + 1;
  blbcn_1.icp[4] = ip5 + 1;

  RL0 = rl0;
  DS = ds;
  RL1 = rl1;
  DSMIN = dsmin;
  AUTO_A0 = a0;
  DSMAX = dsmax;
  AUTO_A1 = a1;

  NTST = ntst;
  ISW = isw;
  NMX = nmx;
  NCOL = ncol;

  blmax_1.npr = npr;
  blmax_1.jac = 0;
  for (i = 0; i < 20; i++)
    EPSL(i) = epsl;
  EPSU = epsu;
  EPSS = epss;
}
