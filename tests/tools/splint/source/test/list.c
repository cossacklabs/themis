typedef /*@null@*/ struct _list
{
  /*@only@*/ char *this;
  /*@null@*/ /*@only@*/ struct _list *next;
} *list;

extern /*@out@*/ /*@only@*/ void *
  smalloc (size_t);

void
list_addh (/*@temp@*/ list l, 
	   /*@only@*/ char *e)
{
  if (l != NULL)
    {
      while (l->next != NULL) 
	{
	  l = l->next;
	}
      
      l->next = (list) 
	smalloc (sizeof (*l->next));
      l->next->this = e;
    }
} /* l->next->next not defined */

void
list_addh2 (/*@temp@*/ list l, 
	    /*@only@*/ char *e)
{
  list new;

  assert (l != NULL);
  assert (l->next == NULL);

  new = (list) smalloc (sizeof (*l->next));
  new->this = e;
  l->next = new;
} /* l->next->next not defined */

      
