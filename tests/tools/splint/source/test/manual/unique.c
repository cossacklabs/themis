# include <string.h>
 
void  
capitalize (/*@out@*/ char *s,
            char *t)
{
  strcpy (s, t);
  *s = toupper (*s);
}
