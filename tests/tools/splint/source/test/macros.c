# include "bool.h"

# define mustard(a,b) ((a) + b + a) /* 1, 2, 3, 4. no proto, 2 no parens, a used twice */
# define ketchup(a)   (a) /* 5. no proto */

# define ok(a, b) ((a) + (b))
#define needparen(a, b) (a + b)           /* 6, 7. no parens a and b */
# define needuse(a, b) (a)                /* 8. b not used */
# define multuse(a, b) ((a) + (a) + (b))  /* 9. a used twice */
# define badassign(a,b) ((a) = (b))       /* 10. assignment to macro */
# define badassign(a,b) ((a)++, ++(b))    /* 11, 12, 13. badassign already defined,
					                 ++ on macro param */

# define predok(a,b) (((a) == 9) ? (b) : -(b)) 
# define predbad(a,b) (((a) == 8) ? (b) : -3)  /* 14. b not used on false */
# define predbad2(a, b) (((a) == 2) ? (a) : (b) + (b)) /* 15, 16, 17. a used twice, 
							  b used twice, not in true */
# define badorder(a,b) ((b)+(a)) /* ok */
# define okorder(a,b,c) (((a) == 2) ? (b) + (c) : (b) - (c))
# define okorder2(a,b,c) (((a) == 2) ? (((b) == 3) ? (c) : -(c)) : (b) - (c))

# define badval(a, b) ((a) == (b)) /* 18. return value */
# define badconst 5                /* 19. type */
# define goodconst ((5) == (3))
# define worseconst(a) ((a) == 7)  /* 20. constant as parameterized macro */

# define var 5                     /* 21. var as macro */
# define var2(x) ((x) + (x))       /* 22, 23. var as parameterized macro, x used twice */
typedef int twerp ;
