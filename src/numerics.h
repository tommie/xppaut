#ifndef XPPAUT_NUMERICS_H
#define XPPAUT_NUMERICS_H

/* --- Types --- */
typedef enum {
  METHOD_UNKNOWN = -1,
  METHOD_DISCRETE,
  METHOD_EULER,
  METHOD_MODEULER,
  METHOD_RK4,
  METHOD_ADAMS,
  METHOD_GEAR,
  METHOD_VOLTERRA,
  METHOD_BACKEUL,
  METHOD_RKQS,
  METHOD_STIFF,
  METHOD_CVODE,
  METHOD_DP5,
  METHOD_DP83,
  METHOD_RB23,
  METHOD_SYMPLECT,

  NUM_METHODS
} Method;

/* --- Data --- */
extern int cv_bandflag;
extern int cv_bandlower;
extern int cv_bandupper;
extern Method METHOD;

/* --- Functions --- */
void chk_delay(void);
void chk_volterra(void);
void compute_one_period(double period, double *x, char *name);
void do_meth(void);
void get_num_par(int ch);
void get_pmap_pars_com(int l);
void quick_num(int com);
void set_col_par_com(int i);
void set_delay(void);
void set_total(double total);
void user_set_color_par(int flag, char *via, double lo, double hi);

#endif /* XPPAUT_NUMERICS_H */
