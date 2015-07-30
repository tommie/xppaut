#ifndef XPPAUT_LUNCH_NEW_H
#define XPPAUT_LUNCH_NEW_H

#include <stdio.h>

/* --- Macros --- */
/* flag values */
#define WRITEM 0
#define READEM 1

/* --- Functions --- */
void do_lunch(int f);
void file_inf(void);
void io_double(double *z, FILE *fp, int f, char *ss);
void io_ic_file(char *fn, int flag);
void io_int(int *i, FILE *fp, int f, char *ss);
void io_parameter_file(char *fn, int flag);
void io_string(char *s, int len, FILE *fp, int f);
void ps_write_pars(FILE *fp);
int read_lunch(FILE *fp);

#endif /* XPPAUT_LUNCH_NEW_H */
