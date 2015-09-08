#ifndef XPPAUT_NUMERICS_H
#define XPPAUT_NUMERICS_H

/* --- Data --- */
extern int cv_bandflag;
extern int cv_bandlower;
extern int cv_bandupper;

/* --- Functions --- */
void chk_delay(void);
void compute_one_period(double period, double *x, char *name);
void get_num_par(int ch);
void get_pmap_pars_com(int l);
void quick_num(int com);
void set_col_par_com(int i);
void set_delay(void);
void set_total(double total);
void user_set_color_par(int flag, char *via, double lo, double hi);

#endif /* XPPAUT_NUMERICS_H */
