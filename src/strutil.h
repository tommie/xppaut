#ifndef XPPAUT_STRUTIL_H
#define XPPAUT_STRUTIL_H

/* --- Functions --- */
void de_space(char *s);
void memmov(char *s1, const char *s2, int len);
void movmem(char *s1, const char *s2, int len);
int strprefix(const char *pre, const char *s);
void stringintersect(char *target, const char *sother);
#ifndef HAVE_STRUPR
void strupr(char *s);
void strlwr(char *s);
#endif

#endif /* XPPAUT_STRUTIL_H */
