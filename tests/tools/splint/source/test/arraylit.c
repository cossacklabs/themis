# include <stdio.h>

int main (/*@unused@*/ int argc, /*@unused@*/ char **argv) /*@*/
{
  /*@observer@*/ char *unmodstr = "unmodifiable string";
  /*@exposed@*/  char  modstr[20]  = "modifiable string";
  /*@unused@*/ char modstr1[5] = "12345"; /* not enough space */
  /*@unused@*/ char modstr2[6] = "12345";
  /*@unused@*/ char modstr3[7] = "12345"; /* if +stringliteralsmaller */

  modstr1 = modstr;

  unmodstr[0] = 'U';  /* line 6; not OK */
  modstr[0] = 'M';    /* line 7; OK */

  fprintf (stderr, "mod: %s mod1: %s", modstr, modstr1);
  return 0;
}
