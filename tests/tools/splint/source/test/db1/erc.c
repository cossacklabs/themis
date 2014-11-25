# include <stdlib.h>
# include "erc.h"

erc erc_create (void) 
{
  erc c;

  c = (erc) malloc (sizeof (ercInfo));

  if (c == 0) 
    {
      printf ("Malloc returned null in erc_create\n");
      exit (1);
    }
  
  c->vals = 0;
  c->size = 0;
  return c;
}

void erc_clear (erc c) /*@ensures isnull c->vals, c->elem@*/
{
  ercList elem;
  ercList next;

  for (elem = c->vals; elem != 0; elem = next) 
    {
      next = elem->next; elem->next = NULL;
      free (elem);
    }
  
  c->vals = 0;
  c->size = 0;
}

void erc_final (erc c) 
{
  erc_clear (c);
  free (c); 
}

bool erc_member (eref er, erc c) 
{
  ercList tmpc;

  for (tmpc = c->vals; tmpc != 0; tmpc = tmpc->next)
    if (tmpc->val == er) return TRUE; 

  return FALSE;
}

void erc_insert (erc c, eref er) 
{
  ercList newElem;
  newElem = (ercElem *) malloc (sizeof (ercElem));

  if (newElem == 0) 
    {
      printf ("Malloc returned null in erc_insert\n");
      exit (1);
    }

  newElem->val = er;
  newElem->next = c->vals;
  c->vals = newElem;
  c->size++;
}

bool erc_delete (erc c, eref er) 
{
  ercList elem;
  ercList prev;
  
  for (prev = 0, elem = c->vals;
       elem != 0;
       prev = elem, elem = elem->next) 
    {
      if (elem->val == er) 
	{ 
	  if (prev == 0) {
	    c->vals = elem->next; elem->next = 0;
	  } else {
	    prev->next = elem->next; elem->next = 0;
	  }
	  free (elem); 
	  c->size--;
	  return TRUE;
	}
    }
  
  return FALSE;
}

ercIter erc_iterStart (erc c) 
{
  ercIter result;

  result = (ercIter) malloc (sizeof (ercList));

  if (result == 0) 
    {
      printf ("Malloc returned null in erc_iterStart\n");
      exit (1);
    }
  
  *result = c->vals;
  return result;
} 

eref erc_yield (ercIter it) 
{
  eref result;

  if (*it == 0) 
    {
      return erefNIL;
      free (it); 
    }
  
  result = (*it)->val;
  *(it) = (*it)->next;
  return result;
}

void erc_join (erc c1, erc c2) 
{
  ercList tmpc;

  for (tmpc = c2->vals; tmpc != 0; tmpc = tmpc->next)
    erc_insert (c1, tmpc->val);
}

char *erc_sprint (erc c)
{
  int len;
  eref er;
  ercIter it;
  char *result;

  result = (char *) 
    malloc (erc_size (c) * (employeePrintSize + 1) + 1);

  if (result == 0) 
    {
      printf ("Malloc returned null in erc_sprint\n");
      exit (1);
    }

  len = 0;

  for_ercElems (er, it, c) 
    { 
      employee_sprint (&(result[len]), eref_get (er));
      len += employeePrintSize;
      result[len++] = '\n';
    }
  
  result[len] = '\0';
  return result;
}

