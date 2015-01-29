# include "palindrome.h"

int main (void)
{
  printf ("cat: %s\n", bool_unparse (isPalindrome ("cat")));
  printf ("catac: %s\n", bool_unparse (isPalindrome ("catac")));
  printf ("caac: %s\n", bool_unparse (isPalindrome ("caac")));
  printf ("catttac: %s\n", bool_unparse (isPalindrome ("catttac")));
  printf ("cabddeac: %s\n", bool_unparse (isPalindrome ("cabddeac")));
}
