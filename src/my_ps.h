#ifndef XPPAUT_MY_PS_H
#define XPPAUT_MY_PS_H

/* --- Data --- */
extern int LastPSX;
extern int LastPSY;
extern int LastPtLine;
extern int NoBreakLine;
extern int PltFmtFlag;
extern int PSColorFlag;
extern char PS_FONT[100];
extern int PS_FONTSIZE;
extern int PSLines;
extern double PS_LW;

/* --- Functions --- */
void ps_bead(int x, int y);
void ps_do_color(int color);
void ps_end(void);
void ps_frect(int x, int y, int w, int h);
int ps_init(char *filename, int color);
void ps_last_pt_off(void);
void ps_line(int xp1, int yp1, int xp2, int yp2);
void ps_linetype(int linetype);
void ps_point(int x, int y);
void ps_stroke(void);
void ps_text(int x, int y, char *str);
void special_put_text_ps(int x, int y, char *str, int size);

#endif /* XPPAUT_MY_PS_H */
