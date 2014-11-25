typedef enum { one, two, three } etype;
typedef enum { cero, uno, dos, tres } stype;
typedef enum _mtag { threem, four } mtype; /* 4. Enum three declared with members ... */
typedef enum _itag { siete } itype;  /* 5. Enumerator member siete declared with inconsistent type: ... */
typedef struct _st { int a; int b; } st ; 
typedef struct { int a; int b; } st2 ; /* 5. Structure st2 declared with fields ... */
                                       /* (5. in enum.lcl) */
int f (void)
{
  etype x;
  mtype m;
  int y;
  int a[];

  y = a[one]; /* 6. Value a[] used before definition */
  x = two;
  y = one;
  x = three; /* 7. Assignment of enum _mtag { three, four } to etype: x = three */

  switch (x)
    {
    case one: break;
    } /* 8. Missing case in switch: two */

  switch (x)
    {
    case one:
      switch (m)
      {
      case three:
	printf("one!");
	break;
      case four:
	printf("yabba");
	break;
      }
      break;
    case one: /* 9. Duplicate case in switch: one */
      break; 
    case 5:
      break; /* case in switch not in enum */
    default:
      break;
    case two:
      break; /* okay (unreachable case) */
    }
} /* 10. Path with no return in function declared to return int */

/* 11. in enum.lcl */

enum { hasta, pasta, yummy } ;

enum { e1, e2 = e1, e3 = e2 } ;

struct adsf
{
  enum { A, B, C } e;
} ;

void f5 (struct adsf s)
{
  s.e = B;
}

