/*  This is an edit box widget which handles a list of
	editable strings
 */
#ifndef XPPAUT_EDIT_RHS_H
#define XPPAUT_EDIT_RHS_H

#include <stdio.h>
#include "xpplim.h"

/* --- Macros --- */
#define FORGET_ALL 0
#define DONE_ALL 2
#define FORGET_THIS 3
#define DONE_THIS 1
#define RESET_ALL 4

/* --- Functions --- */
void edit_functions(void);
void edit_menu(void);
void edit_rhs(void);
int save_as(void);
void user_fun_info(FILE *fp);

#endif /* XPPAUT_EDIT_RHS_H */
