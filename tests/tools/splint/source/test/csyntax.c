enum opcode { ONE } ;
struct sopcode { int x; } ;
union  uopcode { int x; } ;

int f()
{
  int opcode = 1;
  int sopcode = 2;
  int uopcode = 3; 

  return opcode + sopcode + uopcode;
}

struct stat_desc 
{
  short opcode;
  enum opcode x;
} ;

int main(register int x)
{
  return x;
}

extern unsigned x();
extern unsigned int y;

typedef char *cstring;
extern cstring add_extension(cstring s, const cstring suffix);

typedef int filename;
extern cstring filename;

extern int parseOpLine (cstring filename, cstring line);

