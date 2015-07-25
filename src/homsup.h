#ifndef XPPAUT_HOMSUP_H
#define XPPAUT_HOMSUP_H

/* --- Data --- */
extern int (*rhs)();

/* --- Functions --- */
double hom_bcs();
void do_projection();
int pdfdu_();
int projection_();
int hqr3loc_();
int split_();
int exchng_();
int qrstep_();
int orthes_();
int ortran_();

#endif /* XPPAUT_HOMSUP_H */
