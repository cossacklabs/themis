/*
** Boolean type checking
*/
// bool is now defined in the standard library
#include<stdbool.h>

bool f (int i, bool b) {
  if (i) { /* error with -boolint -predboolint */
    return i; /* error with -boolint */
  } else {
    return b; /* okay */
  }
}

