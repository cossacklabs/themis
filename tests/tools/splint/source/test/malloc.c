void f1 ()
{
  int *ip = (int *) malloc (89); /* not divisible */
  if (ip != NULL) {
    ip[88]=23; /* out of range */
    free (ip);
  }
}

void f2 ()
{
  int *ip = (int *) malloc (88); /* divisible okay */
  if (ip != NULL) {
    ip[21]=23; /* okay */
    ip[22]=23; /* bad */
    free (ip);
  }
}

void f3 ()
{
  int *ip = (int *) malloc (87 * sizeof (int)); 
  if (ip != NULL) {
    ip[21]=23; /* okay */
    ip[86]=23; /* okay */
    ip[87]=23; /* bad */
    free (ip);
  }
}

void f4 ()
{
  int *ip = (int *) malloc (87 * sizeof (short)); /* not divisible */
  if (ip != NULL) {
    ip[86]=23; /* bad */
    free (ip);
  }
}
