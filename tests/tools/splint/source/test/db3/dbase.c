# include <strings.h>
# include "dbase.h"

typedef enum
{
  KND_MMGRS, KND_FMGRS, KND_MNON, KND_FNON
} employeeKinds;

/*@constant static employeeKinds firstERC;@*/
# define firstERC KND_MMGRS

/*@constant static employeeKinds lastERC;@*/
# define lastERC KND_FNON

/*@constant static int numERCS;@*/
# define numERCS (/*@+enumint@*/ (lastERC - firstERC + 1) /*@=enumint@*/)

typedef /*@only@*/ erc o_erc;
static o_erc db[numERCS];

/*@iter employeeKinds_all (yield employeeKinds ek); @*/
# define employeeKinds_all(m_ek) \
  { employeeKinds m_ek; for (m_ek = firstERC; m_ek <= lastERC; m_ek++) {

# define end_employeeKinds_all }}

static bool initDone = FALSE;

void db_initMod (void)
  /*@globals initDone, undef db, internalState@*/
  /*@modifies initDone, db, internalState@*/
{
  if (initDone)
    {
      /*@-compdef@*/ return; /*@=compdef@*/
    }
  
  bool_initMod ();
  employee_initMod ();
  eref_initMod ();
  erc_initMod ();
  empset_initMod ();
  
  employeeKinds_all (ek)
    {
      /*@-mustfree@*/ db[(int)ek] = erc_create (); /*@=mustfree@*/
    } end_employeeKinds_all ;
  
  initDone = TRUE; /*@-compdef@*/ /* db[] is really defined */
} /*@=compdef@*/

static eref db_ercKeyGet(erc c, int key)  /*@*/
{
  erc_elements(c, er)
    {
      if (eref_get(er).ssNum == key) 
	{
	  return (er);
	}
    } end_erc_elements ;

  return eref_undefined;
}

static eref db_keyGet (int key) /*@globals db@*/
{
  eref er;
  
  employeeKinds_all (ek)
    {
      er = db_ercKeyGet (db[(int) ek], key);

      if (eref_isDefined (er))
	{
	  return er;
	}
    } end_employeeKinds_all ;
  
  return eref_undefined;
}

static int db_addEmpls (erc c, int l, int h, empset s)
  /*@globals internalState@*/
  /*@modifies s, internalState@*/
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

db_status db_hire (employee e) /*@globals db@*/
{
  if (e.gen == GENDER_UNKNOWN)
    {
      return DBS_GENDERERR;
    }

  if (e.j == JOB_UNKNOWN)
    {
      return DBS_JOBERR;
    }

  if (e.salary < 0)
    {
      return DBS_SALERR;
    }

  if (eref_isDefined (db_keyGet (e.ssNum)))
    {
      return DBS_DUPLERR;
    }

  db_uncheckedHire (e);
  return DBS_OK;
}

void db_uncheckedHire (employee e)
   /*@globals db@*/ /*@modifies db@*/
{
  eref er;
  
  er = eref_alloc ();
  eref_assign (er, e);
  
  if (e.gen == MALE)
    {
      if (e.j == MGR)
	{
	  erc_insert (db[(int) KND_MMGRS], er);
	}
      else
	{
	  erc_insert (db[(int) KND_MNON], er);
	}
    }
  else
    {
      if (e.j == MGR)
	{
	  erc_insert (db[(int) KND_FMGRS], er);
	}
      else
	{
	  erc_insert (db[(int) KND_FNON], er);
	}
    }
}

bool db_fire (int ssNum)
  /*@globals db@*/ /*@modifies db@*/
{
  employeeKinds_all (ek)
    {
      erc_elements (db[(int) ek], er)
	{
	  if (eref_get(er).ssNum == ssNum) 
	    {
	      erc_delete (db[(int) ek], er);
	      return TRUE;
	    } 
	} end_erc_elements ;
    } end_employeeKinds_all ;
  
  return FALSE;
}

bool db_promote (int ssNum)
  /*@globals db@*/ /*@modifies db@*/
{
  eref er;
  employee e;
  gender g;
  
  g = MALE;
  er = db_ercKeyGet (db[(int) KND_MNON], ssNum);
  
  if (!eref_isDefined (er))
    {
      er = db_ercKeyGet (db[(int) KND_FNON], ssNum);

      if (!eref_isDefined (er))
	{
	  return FALSE;
	}

      g = FEMALE;
    }
  
  e = eref_get (er);
  e.j = MGR;
  eref_assign (er, e);
  
  if (g == MALE)
    {
      erc_delete (db[(int) KND_MNON], er);
      erc_insert (db[(int) KND_MMGRS], er);
    } 
  else
    {
      erc_delete (db[(int) KND_FNON], er);
      erc_insert (db[(int) KND_FMGRS], er);
    }
  
  return TRUE;
}

db_status db_setSalary (int ssNum, int sal) /*@globals db@*/
{
  eref er;
  employee e;
  
  if (sal < 0)
    {
      return DBS_SALERR;
    }

  er = db_keyGet (ssNum);

  if (!eref_isDefined (er))
    {
      return DBS_MISSERR;
    }

  e = eref_get (er);
  e.salary = sal;
  eref_assign (er, e);

  return DBS_OK;
}

int db_query (db_q q, empset s)
  /*@globals db@*/
{
  int numAdded;
  int l, h;

  l = q.l;
  h = q.h;

  switch (q.g)
    {
    case GENDER_UNKNOWN:
      switch (q.j)
	{
	case JOB_UNKNOWN:
	  numAdded = 0;

	  employeeKinds_all (ek)
	    {
	      numAdded += db_addEmpls (db[(int) ek], l, h, s);
	    } end_employeeKinds_all

	  return numAdded;
	case MGR:
	  numAdded = db_addEmpls (db[(int) KND_MMGRS], l, h, s);
	  numAdded += db_addEmpls (db[(int) KND_FMGRS], l, h, s);
	  return numAdded;
	case NONMGR:
	  numAdded = db_addEmpls (db[(int) KND_MNON], l, h, s);
	  numAdded += db_addEmpls (db[(int) KND_FNON], l, h, s);
	  return numAdded;
	}
    case MALE:
      switch (q.j)
	{
	case JOB_UNKNOWN:
	  numAdded = db_addEmpls (db[(int) KND_MMGRS], l, h, s);
	  numAdded += db_addEmpls (db[(int) KND_MNON], l, h, s);
	  return numAdded;
	case MGR:
	  return db_addEmpls (db[(int) KND_MMGRS], l, h, s);
	case NONMGR:
	  return db_addEmpls (db[(int) KND_MNON], l, h, s);
	}
    case FEMALE:
      switch (q.j)
	{
	case JOB_UNKNOWN:
	  numAdded = db_addEmpls (db[(int) KND_FMGRS], l, h, s);
	  numAdded += db_addEmpls (db[(int) KND_FNON], l, h, s);
	  return numAdded;
	case MGR:
	  return db_addEmpls (db[(int) KND_FMGRS], l, h, s);
	case NONMGR:
	  return db_addEmpls (db[(int) KND_FNON], l, h, s);
	}
    }
}

void db_print (void)
  /*@globals db@*/
{
  char *printVal;
  
  printf ("Employees:\n");

  employeeKinds_all (ek)
    {
      printVal = erc_sprint (db[(int) ek]);
      printf ("%s", printVal);
      free (printVal);
    } end_employeeKinds_all ;
}
