void copy ( to, from, count )
     char* to;
     char* from; 
{
  int n=(count+7)/8;
  switch (count%8) do {
  case 0: *to = *from++;
  case 7: *to = *from++;
  case 6: *to = *from++;
  case 5: *to = *from++;
  case 4: *to = *from++;
  case 3: *to = *from++;
  case 2: *to = *from++;
  case 1: *to = *from++;
  } while (--n>0);
}

