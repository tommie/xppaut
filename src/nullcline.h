#ifndef XPPAUT_NULLCLINE_H
#define XPPAUT_NULLCLINE_H

/* --- Data --- */
extern double ColorViaHi;
extern double ColorViaLo;
extern int ColorizeFlag;
extern char ColorVia[15];
extern int DF_FLAG;
extern int DF_GRID;
extern int DFBatch;
extern int DOING_DFIELD;
extern int NCBatch;
extern int XNullColor;
extern int YNullColor;

/* --- Functions --- */
void create_new_cline();
void direct_field_com(int c);
void do_batch_dfield();
void do_batch_nclines();
void froz_cline_stuff_com(int i);
int get_nullcline_floats(float **v, int *n, int who, int type);
void new_clines_com(int c);
void redraw_dfield(void);
void restore_nullclines(void);
void set_colorization_stuff();

#endif /* XPPAUT_NULLCLINE_H */
