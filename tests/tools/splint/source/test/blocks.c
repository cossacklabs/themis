int f (int x)
{
  if (x > 3) ;
  while (x < 2) ;

  if (x > 3) x++;
  while (x < 2) x++;

  if (x > 3) { x++; }
  while (x < 2) { x++; }

  for (; x < 2; x++) ;
  for (; x < 2; x++) x++;

  if (x == 0) { x = 3; }
  else if (x == 1) { x = 4; }

  if (x == 0) { x = 3; }
  else x = 4;

  if (x == 0) ;
  else ;

  return x;
}
