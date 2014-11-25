typedef struct s_st st;

struct s_st {
  int x;
} ;


typedef enum e_tag tag;

enum e_tag r;

enum e_tag {
  E_X, E_Y, E_Z
} ;

int f (tag e)
{
  if (e == E_X)
    {
      return 3;
    }
  else
    {
      enum e_tag h = E_Z;

      if (h == e)
	{
	  return 7;
	}
    }

  return 12;
}

enum e_tag {
  E_M
} ;
