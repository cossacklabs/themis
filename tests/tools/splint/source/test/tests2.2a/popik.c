static const dummy[] = 
{
	0x0,
        0x1,
        0x1,
        0x2
};


static const short int dummy2[] = 
{
        0x0,
        0x1,
        0x1,
        0x2
};

static const dummy3[] = 
{
        0,
        1,
        1,
        2
};

/* What this means is that there appear to be some problems with lclint
   1) doesn't detect the missing typing in the first and last examples
          - gcc 2.7.2.f.1 doesn't either it compiles this code without a comment
   2) changing the declaration from 0x0 to 0 has no effect(I expected this),
      but changing 0x1 & 0x2 to 1 & 2 did and I don't have any explanation.

*/



