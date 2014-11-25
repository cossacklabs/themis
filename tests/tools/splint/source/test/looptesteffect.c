
typedef struct elem_ elem;

struct elem_ {
  /*@null@*/ elem *next;
  int val;
};

void foo(/*@null@*/ elem *e)
{
  if ((e != NULL) && (e->val > 2))
    {
      ;
    }

  for (; ((e != NULL) && (e->val > 2)); e=e->next) ;

  for (; ((e == NULL) || (e->val > 2)); e=e->next) ;
}
