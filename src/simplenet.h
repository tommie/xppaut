#ifndef XPPAUT_SIMPLENET_H
#define XPPAUT_SIMPLENET_H

/* --- Functions --- */
int add_spec_fun(char *name, char *rhs);
void add_special_name(char *name, char *rhs);
void eval_all_nets(void);
int g_namelist(char *s, char *root, int *flag, int *i1, int *i2);
double network_value(double x, int i);
void update_all_ffts(void);

#endif /* XPPAUT_SIMPLENET_H */
