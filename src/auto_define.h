#ifndef XPPAUT_AUTO_DEFINE_H
#define XPPAUT_AUTO_DEFINE_H

/* --- Data --- */
struct {
    int ndim, ips, irs, ilp, icp[20];
    double  par[20];
} blbcn_;

#define blbcn_1 blbcn_

struct {
    int ntst, ncol, iad, isp, isw, iplt, nbc, nint;
} blcde_;

#define blcde_1 blcde_

struct {
    double ds, dsmin, dsmax;
    int iads;
} bldls_;

#define bldls_1 bldls_

struct {
    int nmx, nuzr;
    double rl0, rl1, a0, a1;
} bllim_;

#define bllim_1 bllim_

struct {
    int npr, mxbf, iid, itmx, itnw, nwtn, jac;
} blmax_;

#define blmax_1 blmax_

struct {
    double epsl[20], epsu, epss;
} bleps_;

#define bleps_1 bleps_

#endif /* XPPAUT_AUTO_DEFINE_H */
