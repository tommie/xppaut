#include "autevd.h"
#undef abs

#include <math.h>
#include <stdlib.h>

#include "auto_define.h"
#include "auto_nox.h"
#include "auto_x11.h"
#include "autpp.h"
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

#define SPECIAL 5
#define SPER 3
#define UPER 4
#define SEQ 1
#define UEQ 2

/* --- Types --- */
typedef struct {
  int pt, br;
  double evr[NAUTO], evi[NAUTO];
} EIGVAL;

/* --- Forward Declarations --- */
static void add_bif(int ibr, int ntot, int itp, int lab, int npar, double a,
                    const double *uhigh, const double *ulow, const double *u0,
                    const double *ubar, int ndim);
static void send_eigen(int ibr, int ntot, int n, const doublecomplex *ev);
static void send_mult(int ibr, int ntot, int n, const doublecomplex *ev);

/* --- Data --- */
static const AUTPP_CALLBACKS autpp_callbacks = {
  .add_bif = add_bif, .check_stop = check_stop_auto, .send_eigen = send_eigen, .send_mult = send_mult,
};
static EIGVAL my_ev;

/* Only unit 8,3 or q.prb is important; all others are unnecesary */
int get_bif_type(int ibr, int ntot, int lab) {
  int type = SEQ;
  if (ibr < 0 && ntot < 0)
    type = SPER;
  if (ibr < 0 && ntot > 0)
    type = UPER;
  if (ibr > 0 && ntot > 0)
    type = UEQ;
  if (ibr > 0 && ntot < 0)
    type = SEQ;
  /* if(lab>0)type=SPECIAL; */
  return (type);
}

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

  autpp_set_callbacks(&autpp_callbacks);
}

static void add_bif(int ibr, int ntot, int itp, int lab, int npar, double a,
                    const double *uhigh, const double *ulow, const double *u0,
                    const double *ubar, int ndim) {
  int type;
  int icp1 = blbcn_1.icp[0] - 1, icp2 = blbcn_1.icp[1] - 1;
  double per = blbcn_1.par[10];
  type = get_bif_type(ibr, ntot, lab);

  if (ntot == 1) {
    add_point(blbcn_1.par, per, (double*)uhigh, (double*)ulow, (double*)ubar, a, type, 0, lab, npar, icp1,
              icp2, AutoTwoParam, my_ev.evr, my_ev.evi);
  } else {
    add_point(blbcn_1.par, per, (double*)uhigh, (double*)ulow, (double*)ubar, a, type, 1, lab, npar, icp1,
              icp2, AutoTwoParam, my_ev.evr, my_ev.evi);
  }

  add_diagram(ibr, ntot, itp, lab, npar, a, (double*)uhigh, (double*)ulow, (double*)u0, (double*)ubar, blbcn_1.par,
              per, ndim, icp1, icp2, AutoTwoParam, my_ev.evr, my_ev.evi);
}

static void send_eigen(int ibr, int ntot, int n, const doublecomplex *ev) {
  int i;
  double er, cs, sn;
  my_ev.pt = abs(ntot);
  my_ev.br = abs(ibr);
  for (i = 0; i < n; i++) {
    er = exp((ev + i)->r);
    cs = cos((ev + i)->i);
    sn = sin((ev + i)->i);
    my_ev.evr[i] = er * cs;
    my_ev.evi[i] = er * sn;
  }
}

static void send_mult(int ibr, int ntot, int n, const doublecomplex *ev) {
  int i;
  my_ev.pt = abs(ntot);
  my_ev.br = abs(ibr);
  for (i = 0; i < n; i++) {
    my_ev.evr[i] = (ev + i)->r;
    my_ev.evi[i] = (ev + i)->i;
  }
}
