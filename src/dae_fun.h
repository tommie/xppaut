#ifndef XPPAUT_DAE_FUN_H
#define XPPAUT_DAE_FUN_H

/* --- Functions --- */
int add_aeqn(char *rhs);
int add_svar(char *name, char *rhs);
int add_svar_names(void);
int compile_svars(void);
void do_daes(void);
void err_dae(void);
void get_new_guesses(void);
void reset_dae(void);
void set_init_guess(void);

#endif /* XPPAUT_DAE_FUN_H */
