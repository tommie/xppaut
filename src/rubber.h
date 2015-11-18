#ifndef XPPAUT_RUBBER_H
#define XPPAUT_RUBBER_H

#include <X11/Xlib.h>

/* --- Macros --- */
/* f values */
#define RUBBOX 0
#define RUBLINE 1

/* --- Types --- */
typedef void (*RubberEndFunc)(void *cookie, int commit, const int *start/*[2]*/, const int *end/*[2]*/);

/* --- Functions --- */
void rubber(Window w, int f, RubberEndFunc end_func, void *cookie);

#endif /* XPPAUT_RUBBER_H */
