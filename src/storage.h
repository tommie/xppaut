#ifndef XPPAUT_STORAGE_H
#define XPPAUT_STORAGE_H

/* --- Data --- */
extern int MAXSTOR;
extern float **storage;
extern int storind;

/* --- Functions --- */
void alloc_meth(void);
void init_alloc_info(void);
void init_stor(int nrow, int ncol);
int reallocstor(int ncol, int nrow);

#endif /* XPPAUT_STORAGE_H */
