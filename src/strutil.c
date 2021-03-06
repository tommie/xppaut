#include "strutil.h"

#include <ctype.h>
#include <string.h>

void de_space(char *s) {
  int n = strlen(s);
  int i, j = 0;
  char ch;
  for (i = 0; i < n; i++) {
    ch = s[i];
    if (!isspace(ch)) {
      s[j] = ch;
      j++;
    }
  }
  s[j] = 0;
}

void movmem(char *s1, const char *s2, int len) {
  int i;
  for (i = len - 1; i >= 0; i--)
    s1[i] = s2[i];
}

void memmov(char *s1, const char *s2, int len) {
  int i;
  for (i = 0; i < len; i++)
    s1[i] = s2[i];
}

/**
 * Returns whether pre is a prefix of s.
 *
 * @param pre the prefix to search for.
 * @param s the string to search in.
 * @return non-zero if pre is a prefix of s.
 */
int strprefix(const char *pre, const char *s) {
  int n = strlen(pre);

  if (strlen(s) < n)
    return 0;
  for (int i = 0; i < n; i++)
    if (pre[i] != s[i])
      return 0;
  return 1;
}

void stringintersect(char *target, const char *sother) {
  int m = strlen(target);
  int n = strlen(sother);
  if (n < m) {
    m = n;
  }
  int j = 0;
  while (j < m) {
    if (target[j] != sother[j]) {
      break;
    }
    j++;
  }
  target[j] = '\0';
}

void strupr(char *s) {
  int i = 0;
  while (s[i]) {
    if (islower(s[i]))
      s[i] -= 32;
    i++;
  }
}

void strlwr(char *s) {
  int i = 0;
  while (s[i]) {
    if (isupper(s[i]))
      s[i] += 32;
    i++;
  }
}
