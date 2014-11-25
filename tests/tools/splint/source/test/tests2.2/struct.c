extern double read_double (void);

typedef struct
{
   float a;
} some_type;

typedef struct 
{
   double a;
} another_type;

void main (void)
{
  another_type f;
  double x;
  
  x = read_double ();
  f.a = read_double ();
}
