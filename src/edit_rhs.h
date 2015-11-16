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

/* --- Functions --- */
void edit_functions(void);
void edit_menu(void);
void edit_rhs(void);
int save_as(void);

#endif /* XPPAUT_EDIT_RHS_H */
