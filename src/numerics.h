#ifndef XPPAUT_NUMERICS_H
#define XPPAUT_NUMERICS_H

/* --- Data --- */
extern int cv_bandflag;
extern int cv_bandlower;
extern int cv_bandupper;

/* --- Functions --- */
void chk_volterra(void);
void check_pos(int *j);
void quick_num(int com);
void get_num_par(int ch);
void chk_delay(void);
void set_delay(void);
void ruelle(void);
void init_numerics(void);
void meth_dialog(void);
void get_pmap_pars_com(int l);
void get_method(void);
void set_col_par_com(int i);
void do_meth(void);
void set_total(double total);
void user_set_color_par(int flag,char *via,double lo,double hi);
void compute_one_period(double period,double *x, char *name);

#endif /* XPPAUT_NUMERICS_H */
