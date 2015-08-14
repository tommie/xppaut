#ifndef XPPAUT_LOAD_EQN_H
#define XPPAUT_LOAD_EQN_H

#include <stdio.h>
#include "xpplim.h"

/* --- Types --- */
typedef struct {
  char *name;
  char *does;
  unsigned int use;
} INTERN_SET;

/* --- Data --- */
extern double x_3d[2];
extern double y_3d[2];
extern double z_3d[2];
extern double ATOLER;
extern int AXES;
extern double BOUND;
extern double BVP_EPS;
extern int BVP_FLAG;
extern int BVP_MAXIT;
extern double BVP_TOL;
extern double DELAY;
extern char delay_string[MAXODE][80];
extern double DELTA_T;
extern int ENDSING;
extern double EulTol;
extern double EVEC_ERR;
extern int EVEC_ITER;
extern int FFT;
extern int FOREVER;
extern int HIST;
extern double HMAX;
extern double HMIN;
extern int INFLAG;
extern INTERN_SET intern_set[MAX_INTERN_SET];
extern int itor[MAXODE];
extern int IXPLT;
extern int IYPLT;
extern int IZPLT;
extern int IX_PLT[10];
extern int IY_PLT[10];
extern int IZ_PLT[10];
extern double last_ic[MAXODE];
extern int MaxEulIter;
extern int MultiWin;
extern double MY_XHI;
extern double MY_XLO;
extern double MY_YHI;
extern double MY_YLO;
extern double X_HI[10];
extern double X_LO[10];
extern double Y_HI[10];
extern double Y_LO[10];
extern int NC_ITER;
extern int NEQ;
extern double NEWT_ERR;
extern int Nintern_set;
extern int NJMP;
extern int NMESH;
extern int NPltV;
extern double NULL_ERR;
extern int NULL_HERE;
extern int PAR_FOL;
extern int PAUSER;
extern int PLOT_3D;
extern int POIEXT;
extern int POIMAP;
extern double POIPLN;
extern int POISGN;
extern int POIVAR;
extern int RunImmediately;
extern int SHOOT;
extern int silent;
extern int SOS;
extern int START_LINE_TYPE;
extern int STORFLAG;
extern double T0;
extern double TEND;
extern char this_file[XPP_MAX_NAME];
extern char this_internset[XPP_MAX_NAME];
extern int TIMPLOT;
extern double TOLER;
extern double TOR_PERIOD;
extern int TORUS;
extern double TRANS;
extern int xorfix;

/* --- Functions --- */
void add_intern_set(char *name, char *does);
void loadeqn_load_xpprc(void);
void dump_torus(FILE *fp, int f);
void extract_action(char *ptr);
void extract_internset(int j);
void load_eqn(void);
void loadeqn_init_options(void);
void loadeqn_set_internopt(char *line);
void loadeqn_setup_all(void);
int msc(char *s1, char *s2);
void set_option(char *s1, char *s2);

#endif /* XPPAUT_LOAD_EQN_H */
