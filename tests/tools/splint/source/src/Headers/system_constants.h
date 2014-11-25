/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
# ifndef SYSTEM_CONSTANTS_H
# define SYSTEM_CONSTANTS_H

# include "lcl_constants.h"

/* exit codes */

/*@constant int LLSUCCESS; @*/
# define LLSUCCESS   EXIT_SUCCESS

/*@constant int LLFAILURE; @*/
# define LLFAILURE   EXIT_FAILURE

/*@constant int LLGIVEUP; @*/
# define LLGIVEUP    EXIT_FAILURE

/*@constant int LLINTERRUPT; @*/
# define LLINTERRUPT EXIT_FAILURE

/*@constant int DEFAULTMAXMODS; @*/
# define DEFAULTMAXMODS 32

/*
** number of parse errors before quiting
*/

/*@constant int GIVEUPPARSE; @*/
# define GIVEUPPARSE 5

/*
** how many recursive searches to do before bailing!
*/

/*@constant int MAXDEPTH; @*/
# define MAXDEPTH 10

/*
** This limits the number of recursive calls to aliasTable_canAliasAux.
** It is necessary in the presence on recursive nested data structures.
** Note that we don't want to simply detect recursion and quit, since
** aliasing may be apparent only at the deeper level.  It would be better
** to do something in the alias table to confirm that there are no aliases
** recorded below a given depth, but in practice this is much easier!
*/

/*@constant int ALIASSEARCHLIMIT; @*/
# define ALIASSEARCHLIMIT 8

/*
** This one's compilcated.  You probably don't want to change
** it.
**
** lclint maintains a table of ctype's (its representation of
** C types) and represents types internally as an index into
** this table.  This greatly improves performance of type
** checking and saves memory.  For simple derived types, it
** is easy to find if the type is already in the table.  For
** more complicated types (e.g., functions and structures),
** we check if a type is already in the table by looking 
** backwards for matches.  The last DEFAULT_OPTLEVEL types
** entered into the table are checked.  Increasing this value
** may improve memory performance, but cost in execution time.
**
*/

/*@constant int DEFAULT_OPTLEVEL; @*/
# define DEFAULT_OPTLEVEL 15

/*
** There are many *List and *Set types used by lclint (all
** generated automatically using a template and script).  
** For each, the BASESIZE is the number of entries the
** structure grows by each time it runs out of space.
** Depending on expected usage, each List/Set defines
** its BASESIZE to one of these three values.
*/

/*@constant int SMALLBASESIZE; @*/
# define SMALLBASESIZE   4

/*@constant int MIDBASESIZE; @*/
# define MIDBASESIZE     8

/*@constant int LARGEBASESIZE; @*/
# define LARGEBASESIZE  16

/*@constant int BIGBASESIZE; @*/
# define BIGBASESIZE   256

/*@constant int HUGEBASESIZE; @*/
# define HUGEBASESIZE  512

/*
** Size of hash table for file names.
*/

/*@constant unsigned long FTHASHSIZE; @*/
# define FTHASHSIZE 273

/*
** BASESIZE for C environment frames.
*/

/*@constant int CBASESIZE; @*/
# define CBASESIZE 4

/*
** The global environment (across files) is stored in
** a hash table.
*/

/*@constant int CGLOBBASESIZE; @*/
# define CGLOBBASESIZE 1024

/*@constant unsigned long CGLOBHASHSIZE; @*/
# define CGLOBHASHSIZE 1795

/*@constant int LLHASHSIZE; @*/
# define LLHASHSIZE 1279   

# else
# error "Multiple include"
# endif






