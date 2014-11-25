int main () {
  int i = 1;

  switch (i) {
    printf ("here 1"); /* should complain about this! */
  case 1:
    printf ("here 2");
    while (i < 2) {
      printf ("here 3");
    case 2:      
      printf ("here 4");
      i++;
      printf ("here 5");
    }
    printf ("here 6");
  case 3:
    printf ("here 7");
    i++;
  }
}
