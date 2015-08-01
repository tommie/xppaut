#ifndef XPPAUT_COLOR_H
#define XPPAUT_COLOR_H

/* --- Macros --- */
#define FIRSTCOLOR 30

/* --- Data --- */
extern int COLOR;
extern int color_total;
extern int custom_color;
extern int periodic;

/* --- Functions --- */
int ColorMap(int i);
void MakeColormap(void);
void NewColormap(int type);
void get_ps_color(int i, float *r, float *g, float *b);
void get_svg_color(int i,int *r,int *g,int *b);
void set_color(int col);
void set_scolor(int col);

#endif /* XPPAUT_COLOR_H */
