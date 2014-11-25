void (**sig_func)(void);
void (**(sig_func1))(void);

void s_f (int x) {
  if (x > 3) {
    (**sig_func)();
  } else {
    (****sig_func1)(); /* No error - C apparently allows this... */
  }
}
