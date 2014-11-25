long long llf (long long llx, long lx, long double lld) {
  int m = llx; /* int <- long long */
  m = 243LL;
  lld = 3.542L;

  lx = 5234LL;
  lx = 5342L;
  llx = lx;
  lx = llx; /* long <- long long */
  llx = m;

  return lx;
}

