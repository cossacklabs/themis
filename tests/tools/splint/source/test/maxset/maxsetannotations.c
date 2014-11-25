
void anstrcpy( /*@unique@*/ char * a, char *b) /*@requires MaxSet(a) >= MaxRead (b); @*/ {
  strcpy (a,b);
}

