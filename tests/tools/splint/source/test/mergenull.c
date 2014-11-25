/*
** Added 2001-12-29
** Test for branchstate bug reported by Jon Wilson
*/

typedef /*@null@*/ struct s_foo *foo;

struct s_foo {
  foo p;
} ;

void f (foo a)
{
  while ( a != NULL ) {
    foo temp = a->p ;
    a->p = NULL ;
    a = temp ;
  }
}
  
