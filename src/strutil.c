#include <ctype.h>

#include "strutil.h"


void strupr(char *s)
{
  int i=0;
  while(s[i])
    {
      if(islower(s[i]))
        s[i]-=32;
      i++;
    }
}


void strlwr(char *s)
{
  int i=0;
  while(s[i])
    {
      if(isupper(s[i]))
        s[i]+=32;
      i++;
    }
}
