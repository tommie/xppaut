#ifndef XPPAUT_STRUTIL_H
#define XPPAUT_STRUTIL_H

/* --- Functions --- */
void de_space(char *s);
#ifndef HAVE_STRUPR
void strupr(char *s);
void strlwr(char *s);
#endif

#endif /* XPPAUT_STRUTIL_H */
