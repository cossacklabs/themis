/*@-booltype Kzam@*/
typedef int Kzam;

enum { true, false } ; /* changed to be consistent with ISO true/false */

int main() {
  Kzam b = true;
  Kzam b1 = b;

  b = true;
  b = b1;
  b = 12; /* Assignment of int to Kzam: b = 12 */

  b = true && false;
  b = b && false;
  b = false && b;

  if ((b && false) == 0)
    return(0);
  return(0);
}
