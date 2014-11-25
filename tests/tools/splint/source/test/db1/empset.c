# include "empset.h"

static bool initDone = FALSE;

eref _empset_get (employee e, erc s) 
{
  eref er;
  ercIter it;
  employee e1;

  for_ercElems (er, it, s) 
    {
      e1 = eref_get (er);
      if (employee_equal (&e1, &e))
	erc_iterReturn (it, er);
    }
  
  return erefNIL;
}

void empset_clear (empset s) 
{
  erc_clear (s);
}

bool empset_insert (empset s, employee e) 
{
  eref er; 
  
  if (!eref_equal (_empset_get (e, s), erefNIL)) 
    {
      return FALSE;
    }
  
  empset_insertUnique (s, e);
  return TRUE;
}

void empset_insertUnique (empset s, employee e) 
{
  eref er;

  er = ereftab_lookup (e, known);

  if (eref_equal (er, erefNIL)) 
    {
      er = eref_alloc ( );
      eref_assign (er,e);
      ereftab_insert (known, e, er);
    }
  
  erc_insert (s, er);
}

bool empset_delete (empset s, employee e) 
{
  eref er;

  er = _empset_get (e, s);

  if (eref_equal (er, erefNIL)) 
    {
      return FALSE;
    }
  return erc_delete (s, er);
}

empset empset_disjointUnion (empset s1, empset s2) 
{
  erc result;
  ercIter it;
  eref er;
  empset tmp;
  
  result = erc_create ( );

  if (erc_size (s1) > erc_size (s2)) 
    {
      tmp = s1;
      s1 = s2;
      s2 = tmp;
    }
  
  erc_join (result, s1);
  for_ercElems (er, it, s2)
    empset_insertUnique (result, eref_get (er));

  return result;
}

empset empset_union (empset s1, empset s2) 
{
  eref er;
  ercIter it;
  erc result;
  empset tmp;

  result = erc_create ();

  if (erc_size (s1) > erc_size (s2)) 
    {
      tmp = s1;
      s1 = s2;
      s2 = tmp;
    }
  erc_join (result, s2);

  for_ercElems (er, it, s1) 
    if (!empset_member (eref_get (er), s2))
      erc_insert (result, er);

  return result;
}

void empset_intersect (empset s1, empset s2) 
{
  eref er;
  ercIter it;
  erc toDelete;

  toDelete = erc_create ();

  for_ercElems (er, it, s1)
    if (!empset_member (eref_get (er), s2))
      erc_insert (toDelete, er);
  
  for_ercElems (er, it, toDelete)
    erc_delete (s1, er);

  erc_final (toDelete);
}

bool empset_subset (empset s1, empset s2) 
{
  employee e; 
  eref er;
  ercIter it;
  
  for_ercElems (er, it, s1) 
    if (!empset_member (eref_get (er), s2))
      erc_iterReturn (it, FALSE);
  
  return TRUE;
}

void empset_initMod (void) 
{
  if (initDone) return;

  bool_initMod ();
  employee_initMod ();
  eref_initMod ();
  erc_initMod ();
  ereftab_initMod ();
  known = ereftab_create ();
  initDone = TRUE;
}
