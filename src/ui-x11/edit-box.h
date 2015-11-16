#ifndef XPPAUT_UI_X11_EDIT_BOX_H
#define XPPAUT_UI_X11_EDIT_BOX_H

/* --- Macros --- */
#define MAX_N_EBOX MAXODE
#define MAX_LEN_EBOX 86

/* --- Types --- */
typedef int (*EditBoxCommitFunc)(void *,
                                 const char * /*[MAX_N_EBOX][MAX_LEN_EBOX]*/,
                                 int n);

/* --- Functions --- */
/**
 * Opens an edit dialog box.
 *
 * @param n number of edit rows.
 * @param title the window title.
 * @param names a list of row labels. Not used after function returns.
 * @param values a list of initial values. Not used after function returns.
 * @param commit the function to call to commit the values.
 * @param cookie an arbitrary cookie passed to the commit function.
 * @return zero on success, 1 on error.
 */
int x11_edit_box_open(int n, const char *title, char *const *names,
                      char *const *values, EditBoxCommitFunc commit,
                      void *cookie);

#endif /* XPPAUT_UI_X11_EDIT_BOX_H */
