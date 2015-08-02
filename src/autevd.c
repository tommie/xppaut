#include "autevd.h"

#include "auto_define.h"

void init_auto(int ndim, int nbc, int ips, int irs, int ilp, int ntst, int isp,
               int isw, int nmx, int npr, double ds, double dsmin, double dsmax,
               double rl0, double rl1, double a0, double a1, int ip1, int ip2,
               int ip3, int ip4, int ip5, int nuzr, double epsl, double epsu,
               double epss, int ncol) {
  bllim_1.nuzr = nuzr;

  blbcn_1.ndim = ndim;
  blbcn_1.ips = ips;
  blbcn_1.irs = irs;
  blbcn_1.ilp = ilp;

  blcde_1.isp = isp;
  blcde_1.nbc = nbc;
  blcde_1.nint = ndim - nbc; /* here is where the integral conditions come in!!! */

  blbcn_1.icp[0] = ip1 + 1;
  blbcn_1.icp[1] = ip2 + 1;
  blbcn_1.icp[2] = ip3 + 1;
  blbcn_1.icp[3] = ip4 + 1;
  blbcn_1.icp[4] = ip5 + 1;

  bllim_1.rl0 = rl0;
  bldls_1.ds = ds;
  bllim_1.rl1 = rl1;
  bldls_1.dsmin = dsmin;
  bllim_1.a0 = a0;
  bldls_1.dsmax = dsmax;
  bllim_1.a1 = a1;
  blcde_1.ntst = ntst;
  blcde_1.isw = isw;
  bllim_1.nmx = nmx;
  blcde_1.ncol = ncol;

  blmax_1.npr = npr;
  blmax_1.jac = 0;
  for (int i = 0; i < 20; i++)
    bleps_1.epsl[i] = epsl;
  bleps_1.epsu = epsu;
  bleps_1.epss = epss;
}
