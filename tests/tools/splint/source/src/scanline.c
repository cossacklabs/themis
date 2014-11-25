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
** scanline.c
**
** Scan one line of Larch SL input at a time.
**
**	The input is source text, line at a time.   The output is a sequence
**	of tokens, reported by call-out LSLScanFreshToken.
**
**	This organization allows implementation of line-at-a-time
**	incremental scanning.  The incremental mechanism is in the
**	driving module scan.c, which can save and replay tokens for
**	unchanged lines.  This module works either way.
**
**	The main loop of the scanner keys on the leading character.
**	Actions within the loop collect the token starting with the
**	and determine its kind.
*/

# include "splintMacros.nf"
# include "basic.h"
# include "signature.h"
# include "signature2.h"
# include "scan.h"
# include "scanline.h"
# include "tokentable.h"
# include "syntable.h"

/*@notfunction@*/
# define MOVECHAR()   do { *bufptr++ = c; c = *currentLine++; colNumber++; } while (FALSE) 

/* evans 2003-04-21: changed name to avoid conflict with MS VC++ */
/*@constant static int SCANMAXCHAR;@*/ 
# define SCANMAXCHAR 512      

/*@constant static int TABSIZE;@*/
# define TABSIZE 8	

static void LocalUserError (/*@temp@*/ char *p_msg);

static charClassData charClass[LASTCHAR + 1];

static int colNumber; 
static int startCol;	
static bool reportEOL;		
static bool reportComments;	

static char tokenBuffer[SCANMAXCHAR];

static const charClassData charClassDef[] =
{
  /* Control characters					 */

  { CHC_NULL, TRUE },      /*  0 NULL		 */
  { SINGLECHAR, FALSE },        /*  1 CTRL-A		 */
  { SINGLECHAR, FALSE },	/*  2 CTRL-B		 */
  { SINGLECHAR, FALSE },	/*  3 CTRL-C		 */
  { SINGLECHAR, FALSE },	/*  4 CTRL-D		 */
  { SINGLECHAR, FALSE },	/*  5 CTRL-E		 */
  { SINGLECHAR, FALSE },	/*  6 CTRL-F		 */
  { SINGLECHAR, FALSE },	/*  7 CTRL-G		 */
  { SINGLECHAR, FALSE },	/*  8 CTRL-H		 */

  /* defined formatting characters			 */

  { WHITECHAR, FALSE },		/*  9 CTRL-I   TAB	 */
  { WHITECHAR, TRUE },		/* 10 CTRL-J   EOL	 */

  /* more control characters				 */

  { SINGLECHAR, FALSE },	/* 11 CTRL-K		 */
  { SINGLECHAR, FALSE },	/* 12 CTRL-L		 */
  { SINGLECHAR, FALSE },	/* 13 CTRL-M		 */
  { SINGLECHAR, FALSE },	/* 14 CTRL-N		 */
  { SINGLECHAR, FALSE },	/* 15 CTRL-O		 */
  { SINGLECHAR, FALSE },	/* 16 CTRL-P		 */
  { SINGLECHAR, FALSE },	/* 17 CTRL-Q		 */
  { SINGLECHAR, FALSE },	/* 18 CTRL-R		 */
  { SINGLECHAR, FALSE },	/* 19 CTRL-S		 */
  { SINGLECHAR, FALSE },	/* 20 CTRL-T		 */
  { SINGLECHAR, FALSE },	/* 21 CTRL-U		 */
  { SINGLECHAR, FALSE },	/* 22 CTRL-V		 */
  { SINGLECHAR, FALSE },	/* 23 CTRL-W		 */
  { SINGLECHAR, FALSE },	/* 24 CTRL-X		 */
  { SINGLECHAR, FALSE },	/* 25 CTRL-Y		 */
  { SINGLECHAR, FALSE },	/* 26 CTRL-Z		 */
  { SINGLECHAR, FALSE },	/* 27 CTRL-[   ESC	 */
  { SINGLECHAR, FALSE },	/* 28 CTRL-slash         */
  { SINGLECHAR, FALSE },	/* 29 CTRL-]   GS	 */
  { SINGLECHAR, FALSE },	/* 30 CTRL-^   RS	 */
  { SINGLECHAR, FALSE },	/* 31 CTRL-_   US	 */

  /* Special printing characters			 */

  { WHITECHAR, FALSE },		/* 32 space		 */
  { SINGLECHAR, FALSE },	/* 33 !			 */
  { SINGLECHAR, FALSE },	/* 34 "			 */
  { SINGLECHAR, FALSE },	/* 35 #			 */
  { SINGLECHAR, FALSE },	/* 36 $			 */
  { SINGLECHAR, FALSE },	/* 37 %			 */
  { SINGLECHAR, FALSE },	/* 38 &			 */
  { SINGLECHAR, FALSE },	/* 39 '			 */

  /* Reserved characters				 */

  { PERMCHAR, FALSE },		/* 40 (			 */
  { PERMCHAR, FALSE },		/* 41 )			 */
  { OPCHAR, FALSE },		/* 42 *			 */
  { OPCHAR, FALSE },		/* 43 +			 */
  { PERMCHAR, FALSE },		/* 44 ,			 */
  { OPCHAR, FALSE },		/* 45 -			 */
  { OPCHAR, FALSE },		/* 46 .			 */
  { SLASHCHAR, FALSE },		/* 47 /			 */

  /* Numbers						 */

  { IDCHAR, FALSE },		/* 48 0			 */
  { IDCHAR, FALSE },		/* 49 1			 */
  { IDCHAR, FALSE },		/* 50 2			 */
  { IDCHAR, FALSE },		/* 51 3			 */
  { IDCHAR, FALSE },		/* 52 4			 */
  { IDCHAR, FALSE },		/* 53 5			 */
  { IDCHAR, FALSE },		/* 54 6			 */
  { IDCHAR, FALSE },		/* 55 7			 */
  { IDCHAR, FALSE },		/* 56 8			 */
  { IDCHAR, FALSE },		/* 57 9			 */

  /* More reserved and special printing characters	 */

  { PERMCHAR, FALSE },		/* 58 :			 */
  { SINGLECHAR, FALSE },	/* 59;			 */
  { OPCHAR, FALSE },		/* 60 <			 */
  { OPCHAR, FALSE },		/* 61 =			 */
  { OPCHAR, FALSE },		/* 62 >			 */
  { SINGLECHAR, FALSE },	/* 63 ?			 */
  { SINGLECHAR, FALSE },	/* 64 @			 */

  /* Uppercase Alphabetics				 */

  { IDCHAR, FALSE },		/* 65 A 		 */
  { IDCHAR, FALSE },		/* 66 B 		 */
  { IDCHAR, FALSE },		/* 67 C 		 */
  { IDCHAR, FALSE },		/* 68 D 	 	 */
  { IDCHAR, FALSE },		/* 69 E 		 */
  { IDCHAR, FALSE },		/* 70 F			 */
  { IDCHAR, FALSE },		/* 71 G			 */
  { IDCHAR, FALSE },		/* 72 H			 */
  { IDCHAR, FALSE },		/* 73 I			 */
  { IDCHAR, FALSE },		/* 74 J			 */
  { IDCHAR, FALSE },		/* 75 K			 */
  { IDCHAR, FALSE },		/* 76 L			 */
  { IDCHAR, FALSE },		/* 77 M			 */
  { IDCHAR, FALSE },		/* 78 N			 */
  { IDCHAR, FALSE },		/* 79 O			 */
  { IDCHAR, FALSE },		/* 80 P			 */
  { IDCHAR, FALSE },		/* 81 Q			 */
  { IDCHAR, FALSE },		/* 82 R			 */
  { IDCHAR, FALSE },		/* 83 S			 */
  { IDCHAR, FALSE },		/* 84 T			 */
  { IDCHAR, FALSE },		/* 85 U			 */
  { IDCHAR, FALSE },		/* 86 V			 */
  { IDCHAR, FALSE },		/* 87 W			 */
  { IDCHAR, FALSE },		/* 88 X			 */
  { IDCHAR, FALSE },		/* 89 Y			 */
  { IDCHAR, FALSE },		/* 90 Z			 */

  /* Still more reserved and special printing characters */

  { SINGLECHAR, FALSE },	/* 91 [			 */
  { CHC_EXTENSION, FALSE },	/* 92 slash		 */
  { SINGLECHAR, FALSE },	/* 93 ]			 */
  { SINGLECHAR, FALSE },	/* 94 ^			 */
  { IDCHAR, FALSE },		/* 95 _			 */
  { SINGLECHAR, FALSE },	/* 96 `			 */
  
  /* Lowercase alphabetics				 */

  { IDCHAR, FALSE },		/* 97 a			 */
  { IDCHAR, FALSE },		/* 98 b			 */
  { IDCHAR, FALSE },		/* 99 c			 */
  { IDCHAR, FALSE },		/* 100 d		 */
  { IDCHAR, FALSE },		/* 101 e		 */
  { IDCHAR, FALSE },		/* 102 f		 */
  { IDCHAR, FALSE },		/* 103 g		 */
  { IDCHAR, FALSE },		/* 104 h		 */
  { IDCHAR, FALSE },		/* 105 i		 */
  { IDCHAR, FALSE },		/* 106 j		 */
  { IDCHAR, FALSE },		/* 107 k		 */
  { IDCHAR, FALSE },		/* 108 l		 */
  { IDCHAR, FALSE },		/* 109 m		 */
  { IDCHAR, FALSE },		/* 110 n		 */
  { IDCHAR, FALSE },		/* 111 o		 */
  { IDCHAR, FALSE },		/* 112 p		 */
  { IDCHAR, FALSE },		/* 113 q		 */
  { IDCHAR, FALSE },		/* 114 r		 */
  { IDCHAR, FALSE },		/* 115 s		 */
  { IDCHAR, FALSE },		/* 116 t		 */
  { IDCHAR, FALSE },		/* 117 u		 */
  { IDCHAR, FALSE },		/* 118 v		 */
  { IDCHAR, FALSE },		/* 119 w		 */
  { IDCHAR, FALSE },		/* 120 x		 */
  { IDCHAR, FALSE },		/* 121 y		 */
  { IDCHAR, FALSE },		/* 122 z		 */

  { SINGLECHAR, FALSE },	/* 123 {		 */
  { SINGLECHAR, FALSE },	/* 124 |		 */
  { SINGLECHAR, FALSE },	/* 125 }		 */
  { SINGLECHAR, FALSE },	/* 126 ~		 */
  { SINGLECHAR, FALSE },	/* 127 DEL		 */

  /* MCS - unused in English				 */

  { SINGLECHAR, FALSE },	/* 128			 */
  { SINGLECHAR, FALSE },	/* 129			 */
  { SINGLECHAR, FALSE },	/* 130			 */
  { SINGLECHAR, FALSE },	/* 131			 */
  { SINGLECHAR, FALSE },	/* 132			 */
  { SINGLECHAR, FALSE },	/* 133			 */
  { SINGLECHAR, FALSE },	/* 134			 */
  { SINGLECHAR, FALSE },	/* 135			 */
  { SINGLECHAR, FALSE },	/* 136			 */
  { SINGLECHAR, FALSE },	/* 137			 */
  { SINGLECHAR, FALSE },	/* 138			 */
  { SINGLECHAR, FALSE },	/* 139			 */
  { SINGLECHAR, FALSE },	/* 140			 */
  { SINGLECHAR, FALSE },	/* 141			 */
  { SINGLECHAR, FALSE },	/* 142			 */
  { SINGLECHAR, FALSE },	/* 143			 */
  { SINGLECHAR, FALSE },	/* 144			 */
  { SINGLECHAR, FALSE },	/* 145			 */
  { SINGLECHAR, FALSE },	/* 146			 */
  { SINGLECHAR, FALSE },	/* 147			 */
  { SINGLECHAR, FALSE },	/* 148			 */
  { SINGLECHAR, FALSE },	/* 149			 */
  { SINGLECHAR, FALSE },	/* 150			 */
  { SINGLECHAR, FALSE },	/* 151			 */
  { SINGLECHAR, FALSE },	/* 152			 */
  { SINGLECHAR, FALSE },	/* 153			 */
  { SINGLECHAR, FALSE },	/* 154			 */
  { SINGLECHAR, FALSE },	/* 155			 */
  { SINGLECHAR, FALSE },	/* 156			 */
  { SINGLECHAR, FALSE },	/* 157			 */
  { SINGLECHAR, FALSE },	/* 158			 */
  { SINGLECHAR, FALSE },	/* 159			 */
  { SINGLECHAR, FALSE },	/* 160			 */
  { SINGLECHAR, FALSE },	/* 161			 */
  { SINGLECHAR, FALSE },	/* 162			 */
  { SINGLECHAR, FALSE },	/* 163			 */
  { SINGLECHAR, FALSE },	/* 164			 */
  { SINGLECHAR, FALSE },	/* 165			 */
  { SINGLECHAR, FALSE },	/* 166			 */
  { SINGLECHAR, FALSE },	/* 167			 */
  { SINGLECHAR, FALSE },	/* 168			 */
  { SINGLECHAR, FALSE },	/* 169			 */
  { SINGLECHAR, FALSE },	/* 170			 */
  { SINGLECHAR, FALSE },	/* 171			 */
  { SINGLECHAR, FALSE },	/* 172			 */
  { SINGLECHAR, FALSE },	/* 173			 */
  { SINGLECHAR, FALSE },	/* 174			 */
  { SINGLECHAR, FALSE },	/* 175			 */
  { SINGLECHAR, FALSE },	/* 176			 */
  { SINGLECHAR, FALSE },	/* 177			 */
  { SINGLECHAR, FALSE },	/* 178			 */
  { SINGLECHAR, FALSE },	/* 179			 */
  { SINGLECHAR, FALSE },	/* 180			 */
  { SINGLECHAR, FALSE },	/* 181			 */
  { SINGLECHAR, FALSE },	/* 182			 */
  { SINGLECHAR, FALSE },	/* 183			 */
  { SINGLECHAR, FALSE },	/* 184			 */
  { SINGLECHAR, FALSE },	/* 185			 */
  { SINGLECHAR, FALSE },	/* 186			 */
  { SINGLECHAR, FALSE },	/* 187			 */
  { SINGLECHAR, FALSE },	/* 188			 */
  { SINGLECHAR, FALSE },	/* 189			 */
  { SINGLECHAR, FALSE },	/* 190			 */
  { SINGLECHAR, FALSE },	/* 191			 */
  { SINGLECHAR, FALSE },	/* 192			 */
  { SINGLECHAR, FALSE },	/* 193			 */
  { SINGLECHAR, FALSE },	/* 194			 */
  { SINGLECHAR, FALSE },	/* 195			 */
  { SINGLECHAR, FALSE },	/* 196			 */
  { SINGLECHAR, FALSE },	/* 197			 */
  { SINGLECHAR, FALSE },	/* 198			 */
  { SINGLECHAR, FALSE },	/* 199			 */
  { SINGLECHAR, FALSE },	/* 200			 */
  { SINGLECHAR, FALSE },	/* 201			 */
  { SINGLECHAR, FALSE },	/* 202			 */
  { SINGLECHAR, FALSE },	/* 203			 */
  { SINGLECHAR, FALSE },	/* 204			 */
  { SINGLECHAR, FALSE },	/* 205			 */
  { SINGLECHAR, FALSE },	/* 206			 */
  { SINGLECHAR, FALSE },	/* 207			 */
  { SINGLECHAR, FALSE },	/* 208			 */
  { SINGLECHAR, FALSE },	/* 209			 */
  { SINGLECHAR, FALSE },	/* 210			 */
  { SINGLECHAR, FALSE },	/* 211			 */
  { SINGLECHAR, FALSE },	/* 212			 */
  { SINGLECHAR, FALSE },	/* 213			 */
  { SINGLECHAR, FALSE },	/* 214			 */
  { SINGLECHAR, FALSE },	/* 215			 */
  { SINGLECHAR, FALSE },	/* 216			 */
  { SINGLECHAR, FALSE },	/* 217			 */
  { SINGLECHAR, FALSE },	/* 218			 */
  { SINGLECHAR, FALSE },	/* 219			 */
  { SINGLECHAR, FALSE },	/* 220			 */
  { SINGLECHAR, FALSE },	/* 221			 */
  { SINGLECHAR, FALSE },	/* 222			 */
  { SINGLECHAR, FALSE },	/* 223			 */
  { SINGLECHAR, FALSE },	/* 224			 */
  { SINGLECHAR, FALSE },	/* 225			 */
  { SINGLECHAR, FALSE },	/* 226			 */
  { SINGLECHAR, FALSE },	/* 227			 */
  { SINGLECHAR, FALSE },	/* 228			 */
  { SINGLECHAR, FALSE },	/* 229			 */
  { SINGLECHAR, FALSE },	/* 230			 */
  { SINGLECHAR, FALSE },	/* 231			 */
  { SINGLECHAR, FALSE },	/* 232			 */
  { SINGLECHAR, FALSE },	/* 233			 */
  { SINGLECHAR, FALSE },	/* 234			 */
  { SINGLECHAR, FALSE },	/* 235			 */
  { SINGLECHAR, FALSE },	/* 236			 */
  { SINGLECHAR, FALSE },	/* 237			 */
  { SINGLECHAR, FALSE },	/* 238			 */
  { SINGLECHAR, FALSE },	/* 239			 */
  { SINGLECHAR, FALSE },	/* 240			 */
  { SINGLECHAR, FALSE },	/* 241			 */
  { SINGLECHAR, FALSE },	/* 242			 */
  { SINGLECHAR, FALSE },	/* 243			 */
  { SINGLECHAR, FALSE },	/* 244			 */
  { SINGLECHAR, FALSE },	/* 245			 */
  { SINGLECHAR, FALSE },	/* 246			 */
  { SINGLECHAR, FALSE },	/* 247			 */
  { SINGLECHAR, FALSE },	/* 248			 */
  { SINGLECHAR, FALSE },	/* 249			 */
  { SINGLECHAR, FALSE },	/* 250			 */
  { SINGLECHAR, FALSE },	/* 251			 */
  { SINGLECHAR, FALSE },	/* 252			 */
  { SINGLECHAR, FALSE },	/* 253			 */
  { SINGLECHAR, FALSE },	/* 254			 */
  { SINGLECHAR, FALSE }		/* 255			 */
};

/*
**++
**  FUNCTION NAME:
**
**      lscanline ()
**
**  FORMAL PARAMETERS:
**
**      None
**
**  RETURN VALUE:
**
**      None
**
**  INVARIANTS:
**
**      [@description or none@]
**
**  DESCRIPTION:
**
**	One line of text is processed.
**	Tokens are delivered via the call LSLScanFreshToken ().
**
**  EXCEPTIONS:
**
**--
*/

void
lscanLine (char *currentLine)
{
  ltokenCode cod;	 
  lsymbol sym;		
  register char c;	
  register char *bufptr;
  ltoken newToken;	

  c = *currentLine++;	
  colNumber = 0;	

  for (;;)
    {			
      bufptr = &tokenBuffer[0];	
      startCol = colNumber;	

      /*@-loopswitchbreak@*/
      switch (lscanCharClass (c))
	{			

	case CHC_NULL:	
	  sym = lsymbol_fromChars ("E O L");
	  cod = LST_EOL;
	  break;
	  
	 /* Identifiers */

	case IDCHAR:

	  while (lscanCharClass (c) == IDCHAR)
	    {			
	      MOVECHAR ();
	    }

	  *bufptr = '\0';	
	  sym = lsymbol_fromChars (&tokenBuffer[0]);
	  cod = LST_SIMPLEID;
	  break;

	 /* One-character tokens */

	case SINGLECHAR:
	case PERMCHAR:		
	  MOVECHAR ();
	  *bufptr = '\0';	
	  sym = lsymbol_fromChars (&tokenBuffer[0]);
	  cod = LST_SIMPLEOP;
	  break;

	case SLASHCHAR:
	  if (*currentLine == '\\')
	    {
	      MOVECHAR ();
	      MOVECHAR ();
	      *bufptr = '\0';
	      sym = lsymbol_fromChars (&tokenBuffer[0]);
	      cod = LST_SIMPLEOP;
	      break;
	    }
	  MOVECHAR ();
	  /* We fall through to next case if we have / followed  */
	  /* by anything else.					 */
	  /*@fallthrough@*/
	case OPCHAR: 

	 /* Operator symbols */

	 /* possible multi character */
	  while (lscanCharClass (c) == OPCHAR)
	    {			
	      MOVECHAR ();
	    }

	  *bufptr = '\0';	/* null terminate in buffer */
	  sym = lsymbol_fromChars (&tokenBuffer[0]);
	  cod = LST_SIMPLEOP;
	  break;

	 /* White space */
	case WHITECHAR:
	  /*@-switchswitchbreak@*/
	  switch (c)
	    {
	    case '\t':
	      MOVECHAR ();
	      colNumber--;
	      colNumber += TABSIZE;
	      colNumber -= (colNumber % TABSIZE);
	      break;

	    case '\v':
	    case '\f':
	      MOVECHAR ();
	      colNumber--;
	      break;

	    default:
	      MOVECHAR ();
	      break;
	    }
	  *bufptr = '\0';	
	  sym = lsymbol_fromChars (&tokenBuffer[0]);
	  cod = LST_WHITESPACE;
	  break;

	case CHC_EXTENSION:	
	  MOVECHAR ();

	  switch (c)
	    {			

	     /* open and close */
	    case '(':		
	      MOVECHAR ();
	      while (lscanCharClass (c) == IDCHAR)
		{		
		  MOVECHAR ();
		}
	      *bufptr = '\0';	
	      sym = lsymbol_fromChars (&tokenBuffer[0]);
	      cod = LST_OPENSYM;
	      break;

	    case ')':		
	      MOVECHAR ();
	      while (lscanCharClass (c) == IDCHAR)
		{		
		  MOVECHAR ();
		}
	      *bufptr = '\0';	
	      sym = lsymbol_fromChars (&tokenBuffer[0]);
	      cod = LST_CLOSESYM;
	      break;

	     /* separator */
	    case ',':		
	      MOVECHAR ();
	      while (lscanCharClass (c) == IDCHAR)
		{		
		  MOVECHAR ();
		}
	      *bufptr = '\0';	
	      sym = lsymbol_fromChars (&tokenBuffer[0]);
	      cod = LST_SEPSYM;
	      break;

	      /* simpleid */
	    case ':':		
	      MOVECHAR ();
	      while (lscanCharClass (c) == IDCHAR)
		{		
		  MOVECHAR ();
		}
	      *bufptr = '\0';	
	      sym = lsymbol_fromChars (&tokenBuffer[0]);
	      cod = LST_SIMPLEID;
	      break;

	    default:		
	      if (lscanCharClass (c) == IDCHAR)
		{
		  do
		    {
		      MOVECHAR ();
		    }
		  while (lscanCharClass (c) == IDCHAR);
		  *bufptr = '\0';
		  sym = lsymbol_fromChars (&tokenBuffer[0]);
		  cod = LST_SIMPLEOP;
		}
	      else
		{
		  /*
                  ** Meets none of the above.  Take the extension	    
		  ** character and the character following and treat	    
		  ** together as a SINGLECHAR.  SINGLECHARs tranlate into 
		  ** SIMPLEOPs.					    
		  */

		  MOVECHAR ();
		  *bufptr = '\0';	
		  sym = lsymbol_fromChars (&tokenBuffer[0]);
		  cod = LST_SIMPLEOP;
		}
	      break;
	    }
	  /*@switchbreak@*/ break;	
	  /*@=switchswitchbreak@*/
	default:

	  LocalUserError ("unexpected character in input");
	  return;
	}
      /*@=loopswitchbreak@*/

      /*
      ** Above code only "guessed" at token type.  Insert it into the    
      ** TokenTable.  If the token already exists, it is returned as
      ** previously defined.  If it does not exist, it is inserted as the 
      ** token code computed above.
      */

      newToken = LSLInsertToken (cod, sym, 0, FALSE);

      if (LSLIsSyn (ltoken_getText (newToken)))
	{
	  /* 
	  ** Token is a synonym.  Get the actual token and set the raw    
	  ** text to the synonym name.				    
	  */

	  newToken = LSLGetTokenForSyn (ltoken_getText (newToken));
	  ltoken_setRawText (newToken, sym);
	}

      ltoken_setCol (newToken, startCol);
      ltoken_setLine (newToken, inputStream_thisLineNumber (LSLScanSource ()));
      ltoken_setFileName (newToken, inputStream_fileName (LSLScanSource ()));
      
      if (ltoken_getCode (newToken) == LST_COMMENTSYM)
	{
	  bufptr = &tokenBuffer[0];

	  while (!LSLIsEndComment (c))
	    {
	      MOVECHAR ();
	    }
	  if (lscanCharClass (c) != CHC_NULL)
	    {
	      MOVECHAR ();
	    }
	  if (reportComments)
	    {
	      *bufptr = '\0';
	      ltoken_setRawText (newToken, lsymbol_fromChars (&tokenBuffer[0]));
	      LSLScanFreshToken (newToken);
	    }
	}
      else if (ltoken_getCode (newToken) == LST_EOL)
	{
	  if (reportEOL)
	    {
	      LSLScanFreshToken (newToken);
	    }
	  return;
	}
      else 
	{
	  if (cod != LST_WHITESPACE)
	    {
	      LSLScanFreshToken (newToken);
	    }
	}
    }
}

ltoken
LSLScanEofToken (void)
{
  ltoken t = ltoken_copy (LSLInsertToken (LEOFTOKEN, 
					  lsymbol_fromChars ("E O F"),
					  0, TRUE));
  ltoken_setCol (t, colNumber);
  ltoken_setLine (t, inputStream_thisLineNumber (LSLScanSource ()));
  ltoken_setFileName (t, inputStream_fileName (LSLScanSource ()));
  return t;
}

void
LSLReportEolTokens (bool setting)
{
  reportEOL = setting;
}

static void
  LocalUserError (char *msg)
{
  inputStream s = LSLScanSource ();
  llfatalerror (message ("%s:%d,%d: %s", 
			 inputStream_fileName (s), 
			 inputStream_thisLineNumber (s), colNumber, 
			 cstring_fromChars (msg)));
}

/*
**++
**  FUNCTION NAME:
**
**      lscanLineInit ()
**
**  FORMAL PARAMETERS:
**
**      None
**
**  RETURN VALUE:
**
**      None
**
**  INVARIANTS:
**
**      [@description or none@]
**
**  DESCRIPTION:
**
**      Initialize this module (should only be called once).
**
**  IMPLICIT INPUTS/OUTPUT:
**
**      GetNextLine - (output) initialized
**	NullToken   - (output) initialized
**      PrintName   - (output) array contents initialized
**
**  EXCEPTIONS:
**
**      None
**--
*/

void
lscanLineInit (void)
{
  int i;

  reportEOL = FALSE;
  reportComments = FALSE;

  for (i = 0; i <= LASTCHAR; i++)
    {
      charClass[i] = charClassDef[i];
    }

 /*
 ** NOTE: The following line ensures that all tokens have nonzero
 ** handles, so that a handle of zero can be used to indicate that a
 ** token does not have a synonym.
 */

  (void) LSLReserveToken (LST_SIMPLEID, "dummy token");

  ltoken_forall = LSLReserveToken (LST_QUANTIFIERSYM, "\\forall");
  ltoken_true = LSLReserveToken (LST_SIMPLEID, "true");
  ltoken_false = LSLReserveToken (LST_SIMPLEID, "false");
  ltoken_not = LSLReserveToken (LST_SIMPLEOP, "\\not");
  ltoken_and = LSLReserveToken (LST_LOGICALOP, "\\and");
  ltoken_or = LSLReserveToken (LST_LOGICALOP, "\\or");
  ltoken_implies = LSLReserveToken (LST_LOGICALOP, "\\implies");

  ltoken_eq = LSLReserveToken (LST_EQOP, "\\eq");
  ltoken_neq = LSLReserveToken (LST_EQOP, "\\neq");

  ltoken_equals = LSLReserveToken (LST_EQUATIONSYM, "\\equals");
  ltoken_eqsep = LSLReserveToken (LST_EQSEPSYM, "\\eqsep");
  ltoken_select = LSLReserveToken (LST_SELECTSYM, "\\select");
  ltoken_open = LSLReserveToken (LST_OPENSYM, "\\open");
  ltoken_sep = LSLReserveToken (LST_SEPSYM, "\\,");
  ltoken_close = LSLReserveToken (LST_CLOSESYM, "\\close");
  ltoken_id = LSLReserveToken (LST_SIMPLEID, "\\:");
  ltoken_arrow = LSLReserveToken (LST_MAPSYM, "\\arrow");
  ltoken_farrow = LSLReserveToken (LST_FIELDMAPSYM, "\\field_arrow");

  ltoken_marker = LSLReserveToken (LST_MARKERSYM, "\\marker");
  ltoken_comment = LSLReserveToken (LST_COMMENTSYM, "\\comment");
  ltoken_compose = LSLReserveToken (LST_COMPOSESYM, "\\composeSort");
  ltoken_if = LSLReserveToken (LST_ifTOKEN, "if");

  (void) LSLReserveToken (LST_LPAR, " (");
  (void) LSLReserveToken (LST_RPAR, ")");
  (void) LSLReserveToken (LST_COMMA, ",");
  (void) LSLReserveToken (LST_COLON, ":");

  (void) LSLReserveToken (LST_LBRACKET, "[");
  (void) LSLReserveToken (LST_RBRACKET, "]");

  (void) LSLReserveToken (LST_WHITESPACE, " ");
  (void) LSLReserveToken (LST_WHITESPACE, "\t");
  (void) LSLReserveToken (LST_WHITESPACE, "\n");

  (void) LSLReserveToken (LEOFTOKEN, "E O F");
  (void) LSLReserveToken (LST_EOL, "E O L");

  (void) LSLReserveToken (LST_assertsTOKEN, "asserts");
  (void) LSLReserveToken (LST_assumesTOKEN, "assumes");
  (void) LSLReserveToken (LST_byTOKEN, "by");
  (void) LSLReserveToken (LST_convertsTOKEN, "converts");
  (void) LSLReserveToken (LST_elseTOKEN, "else");
  (void) LSLReserveToken (LST_enumerationTOKEN, "enumeration");
  (void) LSLReserveToken (LST_equationsTOKEN, "equations");
  (void) LSLReserveToken (LST_exemptingTOKEN, "exempting");
  (void) LSLReserveToken (LST_forTOKEN, "for");
  (void) LSLReserveToken (LST_generatedTOKEN, "generated");
  (void) LSLReserveToken (LST_impliesTOKEN, "implies");
  (void) LSLReserveToken (LST_includesTOKEN, "includes");
  (void) LSLReserveToken (LST_introducesTOKEN, "introduces");
  (void) LSLReserveToken (LST_ofTOKEN, "of");
  (void) LSLReserveToken (LST_partitionedTOKEN, "partitioned");
  (void) LSLReserveToken (LST_thenTOKEN, "then");
  (void) LSLReserveToken (LST_traitTOKEN, "trait");
  (void) LSLReserveToken (LST_tupleTOKEN, "tuple");
  (void) LSLReserveToken (LST_unionTOKEN, "union");
}

void
lscanLineReset (void)
{
}

void
lscanLineCleanup (void)
{
}

charCode lscanCharClass (char c)
{
  return charClass[ (int) (c)].code;
}

bool LSLIsEndComment (char c)
{
  return charClass[ (int) (c)].endCommentChar;
}

void lsetCharClass (char c, charCode cod)
{
  charClass[ (int) (c)].code = cod;
}

void lsetEndCommentChar (char c, bool flag)
{
  charClass[ (int) (c)].endCommentChar = flag;
}
