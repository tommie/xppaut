#ifndef XPPAUT_UI_X11_TORUS_BOX_H
#define XPPAUT_UI_X11_TORUS_BOX_H

/* --- Types --- */
typedef int (*X11TorusBoxCommitFunc)(void *, const int *sel, int n);
typedef struct X11TorusBox X11TorusBox;

/* --- Functions --- */
int x11_tor_box_open(const char *title, const char *names /*[MAXODE][12]*/,
                     int *sel, int n, X11TorusBoxCommitFunc commit,
                     void *cookie);

#endif /* XPPAUT_UI_X11_TORUS_BOX_H */
