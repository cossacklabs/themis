void f (char *s) {
  syslog (LOG_INFO, "error: %m (%s)", s);
}
