typedef int mint;  /* Concrete in impabstract.lcl */
typedef /*@concrete@*/ int cint; /* Abstract in impabstract.lcl */

int f (mint m, cint c)
{
  if (c > 3)
    {
      return c;
    }

  if (m < 2)
    {
      return m;
    }

  return 3;
}
