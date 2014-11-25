void f (int x, int y)
{
  if (x > y)
    { /* depth = 1 */
      { /* still, depth = 1 */
	while (x > 3)
	  /* depth = 2 */
	  if (y == 2) /* depth = 3 */
	    x++; 
	
	if (y == 2)
	  x++; /* depth = 2 */
	else 
	  { /* depth = 2 */
	    if (y == 4) /* depth = 3 */
	      {
		x++; 
	      }
	    else
	      {
		y++; 
	      }
	  }
      }
    }
}
