#ifndef XPPAUT_NUMERICS_H
#define XPPAUT_NUMERICS_H

/* --- Macros --- */
/* METHOD values */
#define VOLTERRA 6
#define BACKEUL 7
#define RKQS 8
#define STIFF 9
#define CVODE 10
#define GEAR 5
#define DP5 11
#define DP83 12
#define RB23 13
#define SYMPLECT 14

/* --- Data --- */
extern int cv_bandflag;
extern int cv_bandlower;
extern int cv_bandupper;
extern int METHOD;

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
