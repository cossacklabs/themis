/*@constant int SHIFT_CONST = 5@*/
# define SHIFT_CONST 5

/*@constant int BAD_CONST = -2@*/
# define BAD_CONST -2

int f (int i1, unsigned int u1, unsigned int u2)
{
  int j1;
  unsigned int v1, v2;

  v1 = u1 << i1; /* Right operand may be negative */
  j1 = i1 << i1; /* Left, right oprands */
  
  v2 = u2 << 5;
  v2 = u2 << SHIFT_CONST;
  v2 = u2 << BAD_CONST; /* Right operand */

  return 3;
}
