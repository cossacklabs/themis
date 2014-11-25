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
** valueTable.c
** Based on genericTable.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "randomNumbers.h"

valueTable valueTable_copy (valueTable s)
{
  if (valueTable_size (s) > 0)
    {
      valueTable t;
      t = valueTable_create (valueTable_size (s));
      
      valueTable_elements (s, key, val) 
	{
	  valueTable_insert (t, cstring_copy (key), stateValue_copy (val));
	} end_valueTable_elements ;
	
	llassert (valueTable_size (s) == valueTable_size (t));
	return t;
    }
  else
    {
      return valueTable_undefined;
    }
}

cstring valueTable_unparse (valueTable h)
{
  cstring res = cstring_newEmpty ();

  valueTable_elements (h, key, val) {
    DPRINTF (("Using key: %s", key));
    res = message ("%q%s: %q [%q]; ", res, key, 
		   stateValue_unparseValue (val, context_lookupMetaStateInfo (key)),
		   stateValue_unparse (val));
  } end_valueTable_elements ;
  
  return res;
}

void valueTable_insert (valueTable h, cstring key, stateValue value)
{
  llassert (stateValue_isDefined (value));
  genericTable_insert ((genericTable) (h), key, (void *) (value));
}

void valueTable_update (valueTable h, cstring key, stateValue newval) 
{
  DPRINTF (("Update: %s -> %s", key, stateValue_unparse (newval)));
  llassert (stateValue_isDefined (newval));
  genericTable_update ((genericTable) (h), key, (void *) (newval));
}
