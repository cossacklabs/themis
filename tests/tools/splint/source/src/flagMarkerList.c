/*
** Splint - annotation-assisted static program checker
** Copyright (C) 1994-2003 University of Virginia,
**         Massachusetts Institute of Technology
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
** 
** The GNU General Public License is available from http://www.gnu.org/ or
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA.
**
** For information on splint: info@splint.org
** To report a bug: splint-bug@splint.org
** For more information: http://www.splint.org
*/
/*
** flagMarkerList.c
**
** based on list_template.c
**
** where T has T_equal (or change this) and T_unparse
**
** invariant: flagMarker's are listed in order
*/

# include "splintMacros.nf"
# include "basic.h"

static int flagMarkerList_lastBeforeLoc (flagMarkerList p_s, fileloc p_loc) /*@*/ ;

static bool
flagMarkerList_contains (flagMarkerList p_s, flagMarker p_fm) /*@*/ ;

flagMarkerList
  flagMarkerList_new ()
{
  flagMarkerList s = (flagMarkerList) dmalloc (sizeof (*s));
  
  s->nelements = 0;
  s->nspace = flagMarkerListBASESIZE; 
  s->elements = (flagMarker *)
    dmalloc (sizeof (*s->elements) * flagMarkerListBASESIZE);

  return (s);
}

static void
flagMarkerList_grow (flagMarkerList s)
{
  int i;
  flagMarker *newelements;
  
  s->nspace += flagMarkerListBASESIZE; 

  newelements = (flagMarker *) dmalloc (sizeof (*newelements) 
					* (s->nelements + s->nspace));

  for (i = 0; i < s->nelements; i++)
    {
      newelements[i] = s->elements[i];
    }
  
  sfree (s->elements);
  s->elements = newelements;
}

bool flagMarkerList_add (flagMarkerList s, flagMarker fm)
{
  int i = s->nelements - 1;
  int lastloc;

  DPRINTF (("Add: %s", flagMarker_unparse (fm)));

  if (flagMarkerList_contains (s, fm))
    {
      flagMarker_free (fm);
      DPRINTF (("List contains: %s", flagMarkerList_unparse (s)));
      return FALSE;
    }

  if (i > 0)
    {
      flagMarker last = s->elements[i];

      if (flagMarker_isIgnoreCount (last))
	{
	  if (!flagMarker_isIgnoreOff (fm))
	    {
	      if (flagMarker_isLocalSet (fm))
		{
		  if (llforceerror 
		      (FLG_WARNFLAGS,
		       cstring_makeLiteral ("Cannot set flag inside ignore "
					    "count region."),
		       flagMarker_getLoc (fm)))
		    {
		      llgenindentmsg 
			(cstring_makeLiteral ("Ignore count region starts"),
			 flagMarker_getLoc (last));
		    }
		}
	      else 
		{
		  if (flagMarker_isIgnoreOn (fm)) 
		    {
		      if (llforceerror 
			  (FLG_WARNFLAGS,
			   cstring_makeLiteral ("Cannot nest ignore regions."),
			   flagMarker_getLoc (fm)))
			{
			  llgenindentmsg 
			    (cstring_makeLiteral ("Previous ignore region starts"),
			     flagMarker_getLoc (last));
			}
		    }
		}

	      flagMarker_free (fm);
	      return FALSE;
	    }
	}
      else 
	{
	  if (flagMarker_isIgnoreOff (last))
	    {
	      flagMarker nlast = s->elements [i - 1];

	      if (flagMarker_isIgnoreCount (nlast))
		{
		  if (fileloc_sameFileAndLine (flagMarker_getLoc (fm),
					       flagMarker_getLoc (nlast)))
		    {
		      if (flagMarker_isLocalSet (fm))
			{
			  if (llforceerror 
			      (FLG_WARNFLAGS,
			       cstring_makeLiteral
			       ("Cannot set flag inside ignore "
				"count region."),
			       flagMarker_getLoc (fm))) 
			    {
			      llgenindentmsg 
				(cstring_makeLiteral 
				 ("Ignore count region starts"),
				 flagMarker_getLoc (nlast));
			      DPRINTF (("Last: %s / %s",
					fileloc_unparse (flagMarker_getLoc (last)),
					fileloc_unparse (flagMarker_getLoc (fm))));
			      
			    }
			}
		      else 
			{
			  if (flagMarker_isIgnoreOn (fm)) 
			    {
			      if (llforceerror 
				  (FLG_WARNFLAGS,
				   cstring_makeLiteral ("Cannot nest ignore regions."),
				   flagMarker_getLoc (fm))) 
				{
				  llgenindentmsg 
				    (cstring_makeLiteral ("Previous ignore region starts"),
				     flagMarker_getLoc (nlast));
				}
			    }
			}
		      
		      flagMarker_free (fm);
		      return FALSE;
		    }
		}
	    }
	}
    }


  /*
  ** Need to insert this flag in the right place (after the last before loc flag)
  */

  lastloc = flagMarkerList_lastBeforeLoc (s, flagMarker_getLoc (fm));

  if (s->nspace <= 0)
    {
      flagMarkerList_grow (s);
    }
  
  s->nspace--;

  if (lastloc == -1)
    {
      /* Add it to the end of the list */
      s->elements[s->nelements] = fm;
    }
  else
    {
      DPRINTF (("Inserting: %s in %s", 
		flagMarker_unparse (fm),
		flagMarkerList_unparse (s)));

      /* Insert it at location lastloc + 1, push other flags down */
      for (i = s->nelements; i > lastloc + 1; i--)
	{
	  s->elements [i] = s->elements [i - 1];
	}

      s->elements [lastloc + 1] = fm;

    }

  s->nelements++;
  return TRUE;
}

void flagMarkerList_checkSuppressCounts (flagMarkerList s)
{
  int nexpected = 0;
  int nsuppressed = 0;
  fileloc loc = fileloc_undefined;
  bool inCount = FALSE;
  int i;
  
  for (i = 0; i < s->nelements; i++)
    {
      flagMarker current = s->elements[i];
      DPRINTF (("flagMarker: %s / %s",
		flagMarker_unparse (current),
		bool_unparse (inCount)));
      
      if (flagMarker_isIgnoreCount (current))
	{
	  llassert (!inCount);
	  inCount = TRUE;
	  nexpected = flagMarker_getCount (current);
	  loc = flagMarker_getLoc (current);
	  nsuppressed = 0;
	}
      else if (flagMarker_isIgnoreOff (current))
	{
	  if (inCount)
	    {
	      inCount = FALSE;
	      llassert (fileloc_isDefined (loc));

	      if (nexpected > 0 && nexpected != nsuppressed)
		{
		  /* Must use forceerror to prevent self-suppression! */
		  llforceerror
		    (FLG_SUPCOUNTS,
		     message 
		     ("Line expects to suppress %d error%&, found %d error%&",
		      nexpected, nsuppressed),
		     loc);
		}
	    }
	}
      else if (flagMarker_isSuppress (current))
	{
	  nsuppressed++;
	}
      else
	{
	  ;
	}
    }

  llassert (!inCount);
}

static void flagMarkerList_splice (flagMarkerList s, 
				   int index,
				   /*@keep@*/ flagMarker fm)
{
  fileloc loc = flagMarker_getLoc (fm);
  fileloc beforeloc, afterloc;
  int i;

  llassert (index >= 0 && (index + 1 < s->nelements));
  
  beforeloc = flagMarker_getLoc (s->elements[index]);
  afterloc = flagMarker_getLoc (s->elements[index + 1]);;
  
  llassert (fileloc_sameFile (beforeloc, loc));
  llassert (fileloc_sameFile (afterloc, loc));

  if (s->nspace <= 0)
    {
      flagMarkerList_grow (s);
    }
  
  for (i = s->nelements; i > index + 1; i--)
    {
      s->elements[i] = s->elements[i - 1];
    }

  s->elements[index + 1] = fm;
  s->nelements++;
  s->nspace--;

  }

/*@only@*/ cstring
flagMarkerList_unparse (flagMarkerList s)
{
   int i;
   cstring st = cstring_makeLiteral ("[");

   for (i = 0; i < s->nelements; i++)
     {
       if (i == 0)
	 {
	   st = message ("%q %q", st, flagMarker_unparse (s->elements[i]));
	 }
       else
	 st = message ("%q, %q", st, flagMarker_unparse (s->elements[i]));
     }
   
   st = message ("%q ]", st);
   return st;
}

void
flagMarkerList_free (flagMarkerList s)
{
  int i;
  for (i = 0; i < s->nelements; i++)
    {
      flagMarker_free (s->elements[i]);
    }
  
  sfree (s->elements); 
  sfree (s);
}

static int
flagMarkerList_lastBeforeLoc (flagMarkerList s, fileloc loc)
{
  int i;

  for (i = s->nelements - 1; i >= 0; i--) 
    {
      flagMarker current = s->elements[i];
      
      if (fileloc_sameFile (current->loc, loc) 
	  && (!flagMarker_beforeMarker (current, loc)))
	{
	  return i;
	}
    }

  return -1;
}

static bool
flagMarkerList_contains (flagMarkerList s, flagMarker fm)
{
  int i;

  for (i = s->nelements - 1; i >= 0; i--) 
    {
      flagMarker current = s->elements[i];
      
      if (flagMarker_equal (current, fm))
	{
	  return TRUE;
	}
    }

  return FALSE;
}

/*
** returns YES iff
**    > in ignore region (there is an ignore ON marker not followed by OFF)
**    > code is OFF (-)
**
** returns NO iff
**    > not in ignore region
**    > code is ON (+)
**
** returns MAYBE iff
**    > not in ignore region
**    > code is unset or =
**
** requires: invariant for flagMarkerList:
**    flagMarker's are sorted by line and col
*/
	  
ynm
flagMarkerList_suppressError (flagMarkerList s, flagcode code, fileloc loc)
{
  int i;
  bool ignoreOff = FALSE;
  bool nameChecksOff = FALSE;
  bool flagOff = FALSE;
  ynm flagSet = MAYBE;
  bool islib = FALSE;
  bool isNameChecksFlag = flagcode_isNameChecksFlag (code);

  if (fileloc_isLib (loc))
    {
      i = s->nelements - 1;
      islib = TRUE;
    }
  else
    {
      i = flagMarkerList_lastBeforeLoc (s, loc);
    }
  
  if (i < 0)
    {
      DPRINTF (("RETURNING!"));
      return MAYBE;
    }
  
  /*
  ** Go backwards through the remaining flagMarkers in this file.
  */

  for (; i >= 0; i--) 
    {
      flagMarker current = s->elements[i];
      
      DPRINTF (("Check current: %s", flagMarker_unparse (current)));

      if (!islib && !flagMarker_sameFile (current, loc))
	{
	  DPRINTF (("Not same file: %s", fileloc_unparse (loc)));
	  break;
	}

      if (flagMarker_isIgnoreOff (current))
	{
	  ignoreOff = TRUE;
	}
      else if (flagMarker_isIgnoreOn (current))
	{
	  if (!ignoreOff)
	    {
	      return YES;
	    }
	}
      else if (flagMarker_isIgnoreCount (current))
	{
	  if (!ignoreOff)
	    {
	      flagMarkerList_splice (s, i,
				     flagMarker_createSuppress (code, loc));
	      return YES;
	    }
	}
      else if (flagMarker_isLocalSet (current))
	{
	  
	  if (!flagOff && flagMarker_getCode (current) == code)
	    {
	      ynm set  = flagMarker_getSet (current);
	      
	      if (ynm_isOff (set))
		{
		  return YES;
		}
	      else
		{
		  if (ynm_isOn (set))
		    {
		      flagOff = TRUE;
		      flagSet = NO;
		    }
		  else
		    {
		      flagOff = TRUE;
		      flagSet = MAYBE;
		    }
		  
		  if (ignoreOff)
		    {
		      if (isNameChecksFlag && !nameChecksOff)
			{
			  ;
			}
		      else
			{
			  return flagSet;
			}
		    }
		}
	    }
	  
	  if (flagMarker_getCode (current) == FLG_NAMECHECKS
	      && !nameChecksOff && isNameChecksFlag)
	    {
	      ynm set  = flagMarker_getSet (current);
	      
	      if (ynm_isOff (set))
		{
		  return YES;
		}
	      else
		{
		  if (ynm_isOn (set))
		    {
		      nameChecksOff = TRUE;
		      flagSet = NO;
		    }
		  else
		    {
		      nameChecksOff = TRUE;
		      flagSet = MAYBE;
		    }
		  
		  if (ignoreOff && flagOff)
		    {
		      return flagSet;
		    }
		}
	    }
	}
      else
	{
	  llassert (flagMarker_isSuppress (current));
	}
    }
  
  return flagSet;
}

bool
flagMarkerList_inIgnore (flagMarkerList s, fileloc loc)
{
  int i;

  if (fileloc_isLib (loc))
    {
      return FALSE;
    }

  i = flagMarkerList_lastBeforeLoc (s, loc);
  
  /*
  ** Go backwards through the remaining flagMarkers in this file.
  */

  for (; i >= 0; i--) 
    {
      flagMarker current = s->elements[i];
      
      if (!flagMarker_sameFile (current, loc))
	{
	  break;
	}

      if (flagMarker_isIgnoreOff (current))
	{
	  return FALSE;;
	}
      else if (flagMarker_isIgnoreOn (current))
	{
	  return TRUE;
	}
      else if (flagMarker_isIgnoreCount (current))
	{
	  flagMarkerList_splice (s, i,
				 flagMarker_createSuppress (SKIP_FLAG, loc));
	  return TRUE;
	}
      else
	{
	  llassert (flagMarker_isLocalSet (current)
		    || flagMarker_isSuppress (current));
	}
    }
  
  return FALSE;
}

