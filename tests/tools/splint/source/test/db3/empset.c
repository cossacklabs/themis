# include "empset.h"

static bool initDone = FALSE;
static /*@only@*/ ereftab known;

static eref empset_get (employee e, erc s) /*@*/
{
  erc_elements(s, er)
    {
      employee e1 = eref_get(er);

      if (employee_equal(&e1, &e))
	{
	  return er;
	}
    } end_erc_elements ;
  
  return eref_undefined;
}

bool empset_member (employee e, erc s)
{
  return (eref_isDefined (empset_get (e, s)));
}

void empset_clear (empset s) 
{
  erc_clear (s);
}

bool /*@alt void@*/ empset_insert (empset s, employee e) 
  /*@globals internalState@*/
  /*@modifies internalState@*/
{
  if (eref_isDefined (empset_get (e, s)))
    {
      return FALSE;
    }
  
  empset_insertUnique (s, e);
  return TRUE;
}

void empset_insertUnique (empset s, employee e) 
   /*@globals known@*/
   /*@modifies known@*/
{
  eref er;

  er = ereftab_lookup (e, known);

  if (!eref_isDefined (er))
    {
      er = eref_alloc ( );
      eref_assign (er,e);
      ereftab_insert (known, e, er);
    }
  
  erc_insert (s, er);
}

bool /*@alt void@*/ empset_delete (empset s, employee e) 
{
  eref er;

  er = empset_get (e, s);

  if (!eref_isDefined (er))
    {
      return FALSE;
    }

  return erc_delete (s, er);
}

empset empset_disjointUnion (empset s1, empset s2) 
   /*@globals known@*/
   /*@modifies known@*/
{
  erc result;
  empset tmp;
  
  result = erc_create ( );

  if (erc_size (s1) > erc_size (s2)) 
    {
      tmp = s1;
      s1 = s2;
      s2 = tmp;
    }
  
  erc_join (result, s1);

  empset_elements(s2, emp)
    {
      empset_insertUnique(result, emp);
    } end_empset_elements ;

  return result;
}

empset empset_union (empset s1, empset s2) 
{
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

  empset_elements (s1, emp)
    {
      if (!empset_member(emp, s2))
	{
	  empset_insert(result, emp);
	}
    } end_empset_elements ;

  return result;
}

void empset_intersect (empset s1, empset s2) 
{
  erc toDelete = erc_create();

  empset_elements (s2, emp)
    {
      if (!empset_member(emp, s2))
	{
	  empset_insert(toDelete, emp);
	}
    } end_empset_elements ;

  empset_elements (toDelete, emp)
    {
      empset_delete(s1, emp);
    } end_empset_elements;

  erc_final (toDelete);
}

bool empset_subset (empset s1, empset s2) 
{
  empset_elements(s1, emp)
    {
      if (!empset_member(emp, s2)) 
	{
	  return FALSE;
	}
    } end_empset_elements ;

  return TRUE;
}

void empset_initMod (void) 
  /*@globals initDone, undef known@*/
  /*@modifies initDone, known@*/
{
  if (initDone) 
    {
      /*@-globstate@*/ return; /*@=globstate@*/
    }

  bool_initMod ();
  employee_initMod ();
  eref_initMod ();
  erc_initMod ();
  ereftab_initMod ();
  known = ereftab_create ();
  initDone = TRUE;
}
