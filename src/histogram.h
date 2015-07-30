#ifndef XPPAUT_HISTOGRAM_H
#define XPPAUT_HISTOGRAM_H

/* --- Types --- */
typedef struct {
  int nbins, nbins2, type, col, col2, fftc;
  double xlo, xhi;
  double ylo, yhi;
  char cond[80];
} HIST_INFO;

/* --- Data --- */
extern int FOUR_HERE;
extern HIST_INFO hist_inf;
extern int post_process;
extern int spec_col;
extern int spec_col2;
extern int spec_wid;
extern int spec_win;

/* --- Functions --- */
void column_mean(void);
void compute_correl(void);
void compute_fourier(void);
void compute_hist(void);
void compute_power(void);
void compute_sd(void);
void compute_stacor(void);
void hist_back(void);
int new_2d_hist(void);
void post_process_stuff(void);

#endif /* XPPAUT_HISTOGRAM_H */
