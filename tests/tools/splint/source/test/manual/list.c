typedef /*@null@*/ struct _list
{
  /*@only@*/ char *this;
  /*@null@*/ /*@only@*/ struct _list *next;
} *list;

extern /*@out@*/ /*@only@*/ void *
  smalloc (size_t);

void
list_addh (list l, /*@only@*/ char *e)
{
  if (l != NULL)
    {
      while (l->next != NULL) 
	{
	  l = l->next;
	}
      
      l->next = (list) smalloc (sizeof (*l->next));
      l->next->this = e;
    }
}

  
      
