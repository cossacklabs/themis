int f (float x) {
  int m = __alignof__ x;
  int n = __alignof__ (x);

  return n + m;
}
