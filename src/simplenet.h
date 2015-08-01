#ifndef _simplenet_h_
#define _simplenet_h_

int add_spec_fun(char *name, char *rhs);
void add_special_name(char *name, char *rhs);
void eval_all_nets(void);
int g_namelist(char *s, char *root, int *flag, int *i1, int *i2);
double network_value(double x, int i);
void update_all_ffts(void);


#endif
