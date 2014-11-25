/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef LCL_CONSTANTS_H
# define LCL_CONSTANTS_H

/* lclsyntable */

/*@constant unsigned long int INITSYNTABLE; @*/
# define INITSYNTABLE   1024

/*@constant double DELTASYNTABLE; @*/
# define DELTASYNTABLE  2.0

/* syntable */

/*@constant int SYNTABLE_BASESIZE; @*/
# define SYNTABLE_BASESIZE 256   

/* lcltokentable */
/*@constant unsigned long int INITTOKENTABLE; @*/
# define INITTOKENTABLE   1024

/*@constant double DELTATOKENTABLE; @*/
# define DELTATOKENTABLE  2.0

/* string2sym */

/*@constant int INITCHARSTRING; @*/
# define INITCHARSTRING    8192 

/*@constant int DELTACHARSTRING; @*/
# define DELTACHARSTRING   2  

/*@constant int INITSTRINGENTRY; @*/
# define INITSTRINGENTRY   1024

/*@constant int DELTASTRINGENTRY; @*/
# define DELTASTRINGENTRY  2  

/*@constant int HASHSIZE; @*/
# define HASHSIZE          1024

/*@constant int HASHMASK; @*/
# define HASHMASK          (HASHSIZE-1)	/* HASHSIZE must be 2^n     */

/*@constant observer char *INITFILENAME; @*/
# define INITFILENAME	"lclinit.lci"

/*@constant observer char *IO_SUFFIX; @*/
# define IO_SUFFIX              ".lcs"

/*@constant observer char *LCLINIT_SUFFIX; @*/
# define LCLINIT_SUFFIX 	".lci"

/*@constant observer char *CTRAITSYMSNAME; @*/
# define CTRAITSYMSNAME         "CTrait.syms"

/*@constant observer char *CTRAITSPECNAME; @*/
# define CTRAITSPECNAME         "CTrait.lsl"

/*@constant observer char *CTRAITFILENAMEN; @*/
# define CTRAITFILENAMEN        "CTrait"


# else
# error "Multiple include"
# endif






