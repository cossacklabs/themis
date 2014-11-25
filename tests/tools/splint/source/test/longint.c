void f (void) 
{
  long int li1 = 3L;
  int i1 = 7;
  short int si1 = 3;

  i1 = li1; /* error unless +longint */
  li1 = i1; /* okay */
  i1 = si1; /* okay */
  si1 = i1; /* error unless +shortint */

  li1 = si1;
  si1 = li1; /* error unless +shortint +longint */
}
