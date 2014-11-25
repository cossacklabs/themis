int main()
{
  char x[3];
  char y[3];
  
  x[(sizeof x)] = 'i';
  y[((sizeof y) - 1)] = '0';
  return 0;
}
