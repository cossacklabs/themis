# include <strings.h>
# include "dbase.h"

# define firstERC mMGRS
# define lastERC fNON
# define numERCS (lastERC - firstERC + 1)

typedef enum
{
  mMGRS, fMGRS, mNON, fNON
} employeeKinds;

erc db[numERCS];

bool initDone = FALSE;

void db_initMod (void)
{
  int i;
  
  if (initDone)
    {
      return;
    }
  
  bool_initMod ();
  employee_initMod ();
  eref_initMod ();
  erc_initMod ();
  empset_initMod ();
  
  for (i = firstERC; i <= lastERC; i++)
    {
      db[i] = erc_create ();
    }
  
  initDone = TRUE;
}

eref _db_ercKeyGet (erc c, int key) 
{
  erc_elements(c, er)
    {
      if (eref_get(er).ssNum == key) return (er);
    } end_erc_elements ;

  return erefNIL;
}

eref _db_keyGet (int key)
{
  int i;
  eref er;
  
  for (i = firstERC; i <= lastERC; i++)
    {
      er = _db_ercKeyGet (db[i], key);
      if (!eref_equal (er, erefNIL))
	{
	  return er;
	}
    }
  
  return erefNIL;
}

int _db_addEmpls (erc c, int l, int h, empset s)
{
  employee e;
  int numAdded;
  numAdded = 0;
  
  erc_elements (c, er) 
    {
      e = eref_get(er);
      if ((e.salary >= l) && (e.salary <= h)) 
	{
	  empset_insert(s, e);
	  numAdded++;
	}
    } end_erc_elements ;

  return numAdded;
}

db_status hire (employee e)
{
  if (e.gen == gender_ANY)
    return genderERR;

  if (e.j == job_ANY)
    return jobERR;

  if (e.salary < 0)
    return salERR;

  if (!eref_equal (_db_keyGet (e.ssNum), erefNIL))
    return duplERR;

  uncheckedHire (e);
  return db_OK;
}

void uncheckedHire (employee e)
{
  eref er;
  
  er = eref_alloc ();
  eref_assign (er, e);
  
  if (e.gen == MALE)
    if (e.j == MGR)
      erc_insert (db[mMGRS], er);
    else
      erc_insert (db[mNON], er);
  else
    if (e.j == MGR)
      erc_insert (db[fMGRS], er);
    else
      erc_insert (db[fNON], er);
}

bool fire (int ssNum)
{
  int i;
  
  for (i = firstERC; i <= lastERC; i++)
    {
      erc_elements(db[i], er)
	{
	  if (eref_get(er).ssNum == ssNum) 
	    {
	      erc_delete(db[i], er);
	      return TRUE;
	    } 
	} end_erc_elements ;
    }
  
  return FALSE;
}

bool promote (int ssNum)
{
  eref er;
  employee e;
  gender g;
  
  g = MALE;
  er = _db_ercKeyGet (db[mNON], ssNum);
  
  if (eref_equal (er, erefNIL))
    {
      er = _db_ercKeyGet (db[fNON], ssNum);
      if (eref_equal (er, erefNIL))
	return FALSE;
      g = FEMALE;
    }
  
  e = eref_get (er);
  e.j = MGR;
  eref_assign (er, e);
  
  if (g == MALE)
    {
      erc_delete (db[mNON], er);
      erc_insert (db[mMGRS], er);
    } 
  else
    {
      erc_delete (db[fNON], er);
      erc_insert (db[fMGRS], er);
    }
  
  return TRUE;
}

db_status setSalary (int ssNum, int sal)
{
  eref er;
  employee e;
  
  if (sal < 0)
    {
      return salERR;
    }

  er = _db_keyGet (ssNum);

  if (eref_equal (er, erefNIL))
    {
      return missERR;
    }

  e = eref_get (er);
  e.salary = sal;
  eref_assign (er, e);

  return db_OK;
}

int query (db_q q, empset s)
{
  int numAdded;
  int l, h;
  int i;

  l = q.l;
  h = q.h;

  switch (q.g)
    {
    case gender_ANY:
      switch (q.j)
	{
	case job_ANY:
	  numAdded = 0;
	  for (i = firstERC; i <= lastERC; i++)
	    numAdded += _db_addEmpls (db[i], l, h, s);
	  return numAdded;
	case MGR:
	  numAdded = _db_addEmpls (db[mMGRS], l, h, s);
	  numAdded += _db_addEmpls (db[fMGRS], l, h, s);
	  return numAdded;
	case NONMGR:
	  numAdded = _db_addEmpls (db[mNON], l, h, s);
	  numAdded += _db_addEmpls (db[fNON], l, h, s);
	  return numAdded;
	}
    case MALE:
      switch (q.j)
	{
	case job_ANY:
	  numAdded = _db_addEmpls (db[mMGRS], l, h, s);
	  numAdded += _db_addEmpls (db[mNON], l, h, s);
	  return numAdded;
	case MGR:
	  return _db_addEmpls (db[mMGRS], l, h, s);
	case NONMGR:
	  return _db_addEmpls (db[mNON], l, h, s);
	}
    case FEMALE:
      switch (q.j)
	{
	case job_ANY:
	  numAdded = _db_addEmpls (db[fMGRS], l, h, s);
	  numAdded += _db_addEmpls (db[fNON], l, h, s);
	  return numAdded;
	case MGR:
	  return _db_addEmpls (db[fMGRS], l, h, s);
	case NONMGR:
	  return _db_addEmpls (db[fNON], l, h, s);
	}
    }
}

void db_print (void)
{
  int i;
  char *printVal;
  
  printf ("Employees:\n");

  for (i = firstERC; i <= lastERC; i++)
    {
      printVal = erc_sprint (db[i]);
      printf ("%s", printVal);
      free (printVal);
    }
}
