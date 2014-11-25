int main (void) {
  char buffer[6];

  (void) fgets (buffer, (int) sizeof (buffer), stdin);

  while (buffer[0] != 'x') {
    (void) fgets (buffer, (int) sizeof (buffer), stdin);
  }

  return 3;
}
