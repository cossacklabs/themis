int f (int x)
{
  if (x > 3) {
    /*@notreached@*/ ;
  } else {
    return 3;
    /*@notreached@*/ x = 12;
  }
}
