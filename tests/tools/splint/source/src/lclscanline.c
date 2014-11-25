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
**  MODULE DESCRIPTION:
**
**	This module scans one line of Larch C Interface Language (LCL) input at
**	a time.
**
**	The input is source text, line at a time.   The output is a sequence
**	of tokens, reported by call-out LSLScanFreshToken.
**
**	This organization allows implementation of line-at-a-time incremental
**	scanning.  The incremental mechanism is in the driving module scan.c.
**
**	The main loop of the scanner keys on the leading character.
**	Within the loop are actions which collect the rest of the
**	token starting with the character.  Various careful hacks
**	show up to disambiguate tokens that break the general pattern
**	(Examples, \/ and /\).  White space is passed and the loop
**	goes once again without calling LSLScanFreshToken ().
**	The line ends with a null.
**
**  AUTHORS:
**
**    JPW, GAF, Yang Meng Tan
*/


# include "splintMacros.nf"
# include "basic.h"
# include "gram.h"
# include "lclscan.h"
# include "scanline.h"
# include "lclscanline.h"
# include "lcltokentable.h"
# include "lclsyntable.h"

/*@constant int CHARSIZE;@*/
# define CHARSIZE    256		/* on an 8-bit machine  */

/*@notfunction@*/
# define LCLMOVECHAR() \
   do { *bufPtr++ = currentChar; currentChar = *currentLine++; \
	colNumber++; } while (FALSE)

/*@notfunction@*/
# define LOOKAHEADCHAR()      (*currentLine)

/*@notfunction@*/
# define LOOKAHEADTWICECHAR() (*(currentLine + 1))

/*@constant static int MAXCHAR;@*/
# define MAXCHAR     512	/* storage for a lexeme     */

/*
** Printname for the TokenCode NOTTOKEN   (also 1st one reserved)
** Printname for the TokenCode BADTOKEN   (also last one reserved)
*/

/*@constant static observer char *FIRSTRESERVEDNAME;@*/
# define FIRSTRESERVEDNAME "?"

/*
** The scanner establishes lexical boundaries by first switching	    
** on the leading character of the pending lexeme.			    
*/

typedef enum
{
  STARTCNUM,			/* First character of a C number. */
  STARTCNUMDOT,			/* "." only starts a C number if digit follows*/
  STARTCSTR,			/* First character of a C string. */
  STARTCCHAR,			/* First character of a C character. */
  STARTWIDE,			/* slash L starts both string and character. */
  STARTSLASH,			/* "/" starts caret, comment comment, operator */
  STARTOTHER			/* Everything else. */
} StartCharType;

static void ScanCComment (void);
static void ScanEscape (void);
static void ScanCString (void);
static void ScanCChar (void);
static void ScanCNumber (void);
static void LocalUserError (/*@temp@*/ char *);

/*
** Array to store character class defintions and record end-of-comment	    
** characters.								    
*/

static charClassData LCLcharClass[LASTCHAR + 1];

/*
** Data shared between routines LCLScanLine, ScanCString, ScanCChar,	    
** ScanCNumber.  LCLScanLine was getting too big for one routine and	    
** passing this data was rather cumbersome.  Making this data global seemed 
** to be the simpliest solution.					    
*/

/* evs - sounds bogus to me! */

static int colNumber;	
static int startCol;	
static char *currentLine;      
static char currentChar;	
static ltokenCode tokenCode;	
static lsymbol tokenSym;	
static char *bufPtr;		

static bool inComment;	
static /*@only@*/ ltoken commentTok;	
static ltokenCode prevTokenCode; /* to disambiguate '        */

static StartCharType startClass[CHARSIZE] =
{
  STARTOTHER,			/*	^@			    00x */
  STARTOTHER,			/*	^a			    01x */
  STARTOTHER,			/*	^b			    02x */
  STARTOTHER,			/*	^c			    03x */
  STARTOTHER,			/*	^d			    04x */
  STARTOTHER,			/*	^e			    05x */
  STARTOTHER,			/*	^f			    06x */
  STARTOTHER,			/*	^g  BELL		    07x */

  STARTOTHER,			/*	^h  BACKSPACE		    08x */
  STARTOTHER,			/*	^i  TAB			    09x */
  STARTOTHER,			/*	^j  NEWLINE		    0Ax */
  STARTOTHER,			/*	^k			    0Bx */
  STARTOTHER,			/*	^l  FORMFEED		    0Cx */
  STARTOTHER,			/*	^m  RETURN		    0Dx */
  STARTOTHER,			/*	^n			    0Ex */
  STARTOTHER,			/*	^o			    0Fx */

  STARTOTHER,			/*	^p			    10x */
  STARTOTHER,			/*	^q			    11x */
  STARTOTHER,			/*	^r			    12x */
  STARTOTHER,			/*	^s			    13x */
  STARTOTHER,			/*	^t			    14x */
  STARTOTHER,			/*	^u			    15x */
  STARTOTHER,			/*	^v			    16x */
  STARTOTHER,			/*	^w			    17x */

  STARTOTHER,			/*	^x			    18x */
  STARTOTHER,			/*	^y			    19x */
  STARTOTHER,			/*	^z			    1Ax */
  STARTOTHER,			/*	^[ ESC			    1Bx */
  STARTOTHER,			/*	^slash			    1Cx */
  STARTOTHER,			/*	^]			    1Dx */
  STARTOTHER,			/*	^^			    1Ex */
  STARTOTHER,			/*	^_			    1Fx */

  STARTOTHER,			/*	BLANK			    20x */
  STARTOTHER,			/*	!			    21x */
  STARTCSTR,			/*	"			    22x */
  STARTOTHER,			/*	#			    23x */
  STARTOTHER,			/*	$ (may be changed in reset) 24x */
  STARTOTHER,			/*	%			    25x */
  STARTOTHER,			/*	&			    26x */
  STARTCCHAR,			/*	'			    27x */

  STARTOTHER,			/*	(			    28x */
  STARTOTHER,			/*	)			    29x */
  STARTOTHER,			/*	*			    2Ax */
  STARTOTHER,			/*	+			    2Bx */
  STARTOTHER,			/*	,			    2Cx */
  STARTOTHER,			/*	-			    2Dx */
  STARTCNUMDOT,			/*	.			    2Ex */
  STARTSLASH,			/*	/			    2Fx */

  STARTCNUM,			/*	0			    30x */
  STARTCNUM,			/*	1			    31x */
  STARTCNUM,			/*	2			    32x */
  STARTCNUM,			/*	3			    33x */
  STARTCNUM,			/*	4			    34x */
  STARTCNUM,			/*	5			    35x */
  STARTCNUM,			/*	6			    36x */
  STARTCNUM,			/*	7			    37x */

  STARTCNUM,			/*	8			    38x */
  STARTCNUM,			/*	9			    39x */
  STARTOTHER,			/*	:			    3Ax */
  STARTOTHER,			/*	;			    3Bx */
  STARTOTHER,			/*	<			    3Cx */
  STARTOTHER,			/*	=			    3Dx */
  STARTOTHER,			/*	>			    3Ex */
  STARTOTHER,			/*	?			    3Fx */

  STARTOTHER,			/*	@			    40x */
  STARTOTHER,			/*	A			    41x */
  STARTOTHER,			/*	B			    42x */
  STARTOTHER,			/*	C			    43x */
  STARTOTHER,			/*	D			    44x */
  STARTOTHER,			/*	E			    45x */
  STARTOTHER,			/*	F			    46x */
  STARTOTHER,			/*	G			    47x */

  STARTOTHER,			/*	H			    48x */
  STARTOTHER,			/*	I			    49x */
  STARTOTHER,			/*	J			    4Ax */
  STARTOTHER,			/*	K			    4Bx */
  STARTOTHER,			/*	L			    4Cx */
  STARTOTHER,			/*	M			    4Dx */
  STARTOTHER,			/*	N			    4Ex */
  STARTOTHER,			/*	O			    4Fx */

  STARTOTHER,			/*	P			    50x */
  STARTOTHER,			/*	Q			    51x */
  STARTOTHER,			/*	R			    52x */
  STARTOTHER,			/*	S			    53x */
  STARTOTHER,			/*	T			    54x */
  STARTOTHER,			/*	U			    55x */
  STARTOTHER,			/*	V			    56x */
  STARTOTHER,			/*	W			    57x */

  STARTOTHER,			/*	X			    58x */
  STARTOTHER,			/*	Y			    59x */
  STARTOTHER,			/*	Z			    5Ax */
  STARTOTHER,			/*	[			    5Bx */
  STARTWIDE,			/*	slash			    5Cx */
  STARTOTHER,			/*	]			    5Dx */
  STARTOTHER,			/*	^			    5Ex */
  STARTOTHER,			/*	_			    5Fx */

  STARTOTHER,			/*	`			    60x */
  STARTOTHER,			/*	a			    61x */
  STARTOTHER,			/*	b			    62x */
  STARTOTHER,			/*	c			    63x */
  STARTOTHER,			/*	d			    64x */
  STARTOTHER,			/*	e			    65x */
  STARTOTHER,			/*	f			    66x */
  STARTOTHER,			/*	g			    67x */

  STARTOTHER,			/*	h			    68x */
  STARTOTHER,			/*	i			    69x */
  STARTOTHER,			/*	j			    6Ax */
  STARTOTHER,			/*	k			    6Bx */
  STARTOTHER,			/*	l			    6Cx */
  STARTOTHER,			/*	m			    6Dx */
  STARTOTHER,			/*	n			    6Ex */
  STARTOTHER,			/*	o			    6Fx */

  STARTOTHER,			/*	p			    70x */
  STARTOTHER,			/*	q			    71x */
  STARTOTHER,			/*	r			    72x */
  STARTOTHER,			/*	s			    73x */
  STARTOTHER,			/*	t			    74x */
  STARTOTHER,			/*	u			    75x */
  STARTOTHER,			/*	v			    76x */
  STARTOTHER,			/*	w			    77x */

  STARTOTHER,			/*	x			    78x */
  STARTOTHER,			/*	y			    79x */
  STARTOTHER,			/*	z			    7Ax */
  STARTOTHER,			/*	{			    7Dx */
  STARTOTHER,			/*	|			    7Cx */
  STARTOTHER,			/*	}			    7Dx */
  STARTOTHER,			/*	~			    7Ex */
  STARTOTHER,
  STARTOTHER			/*	RUBOUT			    7Fx */
};

/*
** Given a character code, its status as part of an decimal escape sequence
** can be derived from this table.  Digits 0-9 allowed.
*/

static bool isDigit[CHARSIZE] =
{
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};

/*
 * Given a character code, its status as part of an octal escape sequence
 * can be derived from this table.  Digits 0-7 allowed.
 */

static bool isOigit[CHARSIZE] =
{
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};

/*
 * Given a character code, its status as part of a hex escape sequence
 * can be derived from this table.  Digits, a-f, A-F allowed.
 */

static bool isXigit[CHARSIZE] =
{
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,

  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,

  FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,

  FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};


/*
 * Given a character code, its status as part of a C string
 * can be derived from this table.  Everything but quotes and newline
 * are allowed.
 */

static bool isStrChar[CHARSIZE] =
{
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE
};

/*
 * Given a character code, its status as part of a C Character
 * can be derived from this table.  Everything but quotes and newline
 * are allowed.
 */

static bool isCharChar[CHARSIZE] =
{
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE
};

/*
** Given a character code, its status as part of a string or character
** simple escape sequence ('slash'', 'slash"', 'slash?', 'slashslash', 
** 'slasha', 'slashb', 'slashf', 'slashn', 'slasht', and 'slashv')
** can be derived from this table.  ''', '"', '?', 'slash', 'a',
** 'b', 'f', 'n', 't', and 'v' are allowed.
*/

static bool isSimpleEscape[CHARSIZE] =
{
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE,
  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE,
  FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};

static bool reportEOL;		
static bool reportComments;	
static lsymbol firstReserved;  

static char tokenBuffer[MAXCHAR];

static const charClassData charClassDef[] =
{
  /* Control characters	*/

  { SINGLECHAR, FALSE },		/*	 0 NULL			    */
  { SINGLECHAR, FALSE },		/*	 1 CTRL-A		    */
  { SINGLECHAR, FALSE },		/*	 2 CTRL-B		    */
  { SINGLECHAR, FALSE },		/*	 3 CTRL-C		    */
  { SINGLECHAR, FALSE },		/*	 4 CTRL-D		    */
  { SINGLECHAR, FALSE },		/*	 5 CTRL-E		    */
  { SINGLECHAR, FALSE },		/*	 6 CTRL-F		    */
  { SINGLECHAR, FALSE },		/*	 7 CTRL-G		    */
  { SINGLECHAR, FALSE },		/*	 8 CTRL-H		    */

  /* defined formatting characters */
  { WHITECHAR, FALSE },		        /*	 9 CTRL-I   TAB		    */
  { CHC_NULL, TRUE },		/*	10 CTRL-J   EOL		    */

  /* more control characters */
  { SINGLECHAR, FALSE },		/*	11 CTRL-K		    */
  { WHITECHAR, FALSE },		        /*	12 CTRL-L		    */
  { SINGLECHAR, FALSE },		/*	13 CTRL-M		    */
  { SINGLECHAR, FALSE },		/*	14 CTRL-N		    */
  { SINGLECHAR, FALSE },		/*	15 CTRL-O		    */
  { SINGLECHAR, FALSE },		/*	16 CTRL-P		    */
  { SINGLECHAR, FALSE },		/*	17 CTRL-Q		    */
  { SINGLECHAR, FALSE },		/*	18 CTRL-R		    */
  { SINGLECHAR, FALSE },		/*	19 CTRL-S		    */
  { SINGLECHAR, FALSE },		/*	20 CTRL-T		    */
  { SINGLECHAR, FALSE },		/*	21 CTRL-U		    */
  { SINGLECHAR, FALSE },		/*	22 CTRL-V		    */
  { SINGLECHAR, FALSE },		/*	23 CTRL-W		    */
  { SINGLECHAR, FALSE },		/*	24 CTRL-X		    */
  { SINGLECHAR, FALSE },		/*	25 CTRL-Y		    */
  { SINGLECHAR, FALSE },		/*	26 CTRL-Z		    */
  { SINGLECHAR, FALSE },		/*	27 CTRL-[   ESC		    */
  { SINGLECHAR, FALSE },		/*	28 CTRL-slash   FS    	    */
  { SINGLECHAR, FALSE },		/*	29 CTRL-]   GS		    */
  { SINGLECHAR, FALSE },		/*	30 CTRL-^   RS		    */
  { SINGLECHAR, FALSE },		/*	31 CTRL-_   US		    */

  /* Special printing characters */
  { WHITECHAR, FALSE },		        /*	32 space		    */
  { SINGLECHAR, FALSE },		/*	33 !			    */
  { SINGLECHAR, FALSE },		/*	34 "			    */
  { SINGLECHAR, FALSE },		/*	35 #			    */
  { SINGLECHAR, FALSE },		/*	36 $			    */
  { SINGLECHAR, FALSE },		/*	37 %			    */
  { SINGLECHAR, FALSE },		/*	38 &			    */
  { SINGLECHAR, FALSE },		/*	39 '			    */

  /* Reserved characters */
  { PERMCHAR, FALSE },		        /*	40 (			    */
  { PERMCHAR, FALSE },		        /*	41 )			    */
  { PERMCHAR, FALSE },		        /*	42 *			    */
  { OPCHAR, FALSE },		        /*	43 +			    */
  { PERMCHAR, FALSE },		        /*	44 ,			    */
  { OPCHAR, FALSE },		        /*	45 -			    */
  { OPCHAR, FALSE },		        /*	46 .			    */
  { OPCHAR, FALSE },		        /*	47 /			    */

/* Numbers							    */
  { IDCHAR, FALSE },		/*	48 0			    */
  { IDCHAR, FALSE },		/*	49 1			    */
  { IDCHAR, FALSE },		/*	50 2			    */
  { IDCHAR, FALSE },		/*	51 3			    */
  { IDCHAR, FALSE },		/*	52 4			    */
  { IDCHAR, FALSE },		/*	53 5			    */
  { IDCHAR, FALSE },		/*	54 6			    */
  { IDCHAR, FALSE },		/*	55 7			    */
  { IDCHAR, FALSE },		/*	56 8			    */
  { IDCHAR, FALSE },		/*	57 9			    */

  /* More reserved and special printing characters		    */
  { PERMCHAR, FALSE },		/*	58 :			    */
  { PERMCHAR, FALSE },		/*	59;			    */
  { OPCHAR, FALSE },		/*	60 <			    */
  { OPCHAR, FALSE },		/*	61 =			    */
  { OPCHAR, FALSE },		/*	62 >			    */
  { SINGLECHAR, FALSE },	/*	63 ?			    */
  { SINGLECHAR, FALSE },	/*	64 @			    */

  /* Uppercase Alphabetics					    */
  { IDCHAR, FALSE },		/*      65 A			    */
  { IDCHAR, FALSE },		/*      66 B			    */
  { IDCHAR, FALSE },		/*      67 C			    */
  { IDCHAR, FALSE },		/*      68 D			    */
  { IDCHAR, FALSE },		/*      69 E			    */
  { IDCHAR, FALSE },		/*	70 F			    */
  { IDCHAR, FALSE },		/*	71 G			    */
  { IDCHAR, FALSE },		/*	72 H			    */
  { IDCHAR, FALSE },		/*	73 I			    */
  { IDCHAR, FALSE },		/*	74 J			    */
  { IDCHAR, FALSE },		/*	75 K			    */
  { IDCHAR, FALSE },		/*	76 L			    */
  { IDCHAR, FALSE },		/*	77 M			    */
  { IDCHAR, FALSE },		/*	78 N			    */
  { IDCHAR, FALSE },		/*	79 O			    */
  { IDCHAR, FALSE },		/*	80 P			    */
  { IDCHAR, FALSE },		/*	81 Q			    */
  { IDCHAR, FALSE },		/*	82 R			    */
  { IDCHAR, FALSE },		/*	83 S			    */
  { IDCHAR, FALSE },		/*	84 T			    */
  { IDCHAR, FALSE },		/*	85 U			    */
  { IDCHAR, FALSE },		/*	86 V			    */
  { IDCHAR, FALSE },		/*	87 W			    */
  { IDCHAR, FALSE },		/*	88 X			    */
  { IDCHAR, FALSE },		/*	89 Y			    */
  { IDCHAR, FALSE },		/*	90 Z			    */

  /* Still more reserved and special printing characters	    */
  { PERMCHAR, FALSE },		/*	91 [			    */
  { CHC_EXTENSION, FALSE },	/*	92 slash	            */
  { PERMCHAR, FALSE },		/*	93 ]			    */
  { SINGLECHAR, FALSE },	/*	94 ^			    */
  { IDCHAR, FALSE },		/*	95 _			    */
  { SINGLECHAR, FALSE },	/*	96 `			    */

  /* Lowercase alphabetics					    */
  { IDCHAR, FALSE },		/*	97 a			    */
  { IDCHAR, FALSE },		/*	98 b			    */
  { IDCHAR, FALSE },		/*	99 c			    */
  { IDCHAR, FALSE },		/*	100 d			    */
  { IDCHAR, FALSE },		/*	101 e			    */
  { IDCHAR, FALSE },		/*	102 f			    */
  { IDCHAR, FALSE },		/*	103 g			    */
  { IDCHAR, FALSE },		/*	104 h			    */
  { IDCHAR, FALSE },		/*	105 i			    */
  { IDCHAR, FALSE },		/*	106 j			    */
  { IDCHAR, FALSE },		/*	107 k			    */
  { IDCHAR, FALSE },		/*	108 l			    */
  { IDCHAR, FALSE },		/*	109 m			    */
  { IDCHAR, FALSE },		/*	110 n			    */
  { IDCHAR, FALSE },		/*	111 o			    */
  { IDCHAR, FALSE },		/*	112 p			    */
  { IDCHAR, FALSE },		/*	113 q			    */
  { IDCHAR, FALSE },		/*	114 r			    */
  { IDCHAR, FALSE },		/*	115 s			    */
  { IDCHAR, FALSE },		/*	116 t			    */
  { IDCHAR, FALSE },		/*	117 u			    */
  { IDCHAR, FALSE },		/*	118 v			    */
  { IDCHAR, FALSE },		/*	119 w			    */
  { IDCHAR, FALSE },		/*	120 x			    */
  { IDCHAR, FALSE },		/*	121 y			    */
  { IDCHAR, FALSE },		/*      122 z			    */

  { SINGLECHAR, FALSE },	/*	123 {			    */
  { SINGLECHAR, FALSE },	/*	124 |			    */
  { SINGLECHAR, FALSE },	/*	125 }			    */
  { SINGLECHAR, FALSE },	/*	126 ~			    */
  { SINGLECHAR, FALSE },	/*	127 DEL			    */

  /* MCS - unused in English					    */
  { SINGLECHAR, FALSE },	/*	128			    */
  { SINGLECHAR, FALSE },	/*	129			    */
  { SINGLECHAR, FALSE },	/*	130			    */
  { SINGLECHAR, FALSE },	/*	131			    */
  { SINGLECHAR, FALSE },	/*	132			    */
  { SINGLECHAR, FALSE },	/*	133			    */
  { SINGLECHAR, FALSE },	/*	134			    */
  { SINGLECHAR, FALSE },	/*	135			    */
  { SINGLECHAR, FALSE },	/*	136			    */
  { SINGLECHAR, FALSE },	/*	137			    */
  { SINGLECHAR, FALSE },	/*	138			    */
  { SINGLECHAR, FALSE },	/*	139			    */
  { SINGLECHAR, FALSE },	/*	140			    */
  { SINGLECHAR, FALSE },	/*	141			    */
  { SINGLECHAR, FALSE },	/*	142			    */
  { SINGLECHAR, FALSE },	/*	143			    */
  { SINGLECHAR, FALSE },	/*	144			    */
  { SINGLECHAR, FALSE },	/*	145			    */
  { SINGLECHAR, FALSE },	/*	146			    */
  { SINGLECHAR, FALSE },	/*	147			    */
  { SINGLECHAR, FALSE },	/*	148			    */
  { SINGLECHAR, FALSE },	/*	149			    */
  { SINGLECHAR, FALSE },	/*	150			    */
  { SINGLECHAR, FALSE },	/*	151			    */
  { SINGLECHAR, FALSE },	/*	152			    */
  { SINGLECHAR, FALSE },	/*	153			    */
  { SINGLECHAR, FALSE },	/*	154			    */
  { SINGLECHAR, FALSE },	/*	155			    */
  { SINGLECHAR, FALSE },	/*	156			    */
  { SINGLECHAR, FALSE },	/*	157			    */
  { SINGLECHAR, FALSE },	/*	158			    */
  { SINGLECHAR, FALSE },	/*	159			    */
  { SINGLECHAR, FALSE },	/*	160			    */
  { SINGLECHAR, FALSE },	/*	161			    */
  { SINGLECHAR, FALSE },	/*	162			    */
  { SINGLECHAR, FALSE },	/*	163			    */
  { SINGLECHAR, FALSE },	/*	164			    */
  { SINGLECHAR, FALSE },	/*	165			    */
  { SINGLECHAR, FALSE },	/*	166			    */
  { SINGLECHAR, FALSE },	/*	167			    */
  { SINGLECHAR, FALSE },	/*	168			    */
  { SINGLECHAR, FALSE },	/*	169			    */
  { SINGLECHAR, FALSE },	/*	170			    */
  { SINGLECHAR, FALSE },	/*	171			    */
  { SINGLECHAR, FALSE },	/*	172			    */
  { SINGLECHAR, FALSE },	/*	173			    */
  { SINGLECHAR, FALSE },	/*	174			    */
  { SINGLECHAR, FALSE },	/*	175			    */
  { SINGLECHAR, FALSE },	/*	176			    */
  { SINGLECHAR, FALSE },	/*	177			    */
  { SINGLECHAR, FALSE },	/*	178			    */
  { SINGLECHAR, FALSE },	/*	179			    */
  { SINGLECHAR, FALSE },	/*	180			    */
  { SINGLECHAR, FALSE },	/*	181			    */
  { SINGLECHAR, FALSE },	/*	182			    */
  { SINGLECHAR, FALSE },	/*	183			    */
  { SINGLECHAR, FALSE },	/*	184			    */
  { SINGLECHAR, FALSE },	/*	185			    */
  { SINGLECHAR, FALSE },	/*	186			    */
  { SINGLECHAR, FALSE },	/*	187			    */
  { SINGLECHAR, FALSE },	/*	188			    */
  { SINGLECHAR, FALSE },	/*	189			    */
  { SINGLECHAR, FALSE },	/*	190			    */
  { SINGLECHAR, FALSE },	/*	191			    */
  { SINGLECHAR, FALSE },	/*	192			    */
  { SINGLECHAR, FALSE },	/*	193			    */
  { SINGLECHAR, FALSE },	/*	194			    */
  { SINGLECHAR, FALSE },	/*	195			    */
  { SINGLECHAR, FALSE },	/*	196			    */
  { SINGLECHAR, FALSE },	/*	197			    */
  { SINGLECHAR, FALSE },	/*	198			    */
  { SINGLECHAR, FALSE },	/*	199			    */
  { SINGLECHAR, FALSE },	/*	200			    */
  { SINGLECHAR, FALSE },	/*	201			    */
  { SINGLECHAR, FALSE },	/*	202			    */
  { SINGLECHAR, FALSE },	/*	203			    */
  { SINGLECHAR, FALSE },	/*	204			    */
  { SINGLECHAR, FALSE },	/*	205			    */
  { SINGLECHAR, FALSE },	/*	206			    */
  { SINGLECHAR, FALSE },	/*	207			    */
  { SINGLECHAR, FALSE },	/*	208			    */
  { SINGLECHAR, FALSE },	/*	209			    */
  { SINGLECHAR, FALSE },	/*	210			    */
  { SINGLECHAR, FALSE },	/*	211			    */
  { SINGLECHAR, FALSE },	/*	212			    */
  { SINGLECHAR, FALSE },	/*	213			    */
  { SINGLECHAR, FALSE },	/*	214			    */
  { SINGLECHAR, FALSE },	/*	215			    */
  { SINGLECHAR, FALSE },	/*	216			    */
  { SINGLECHAR, FALSE },	/*	217			    */
  { SINGLECHAR, FALSE },	/*	218			    */
  { SINGLECHAR, FALSE },	/*	219			    */
  { SINGLECHAR, FALSE },	/*	220			    */
  { SINGLECHAR, FALSE },	/*	221			    */
  { SINGLECHAR, FALSE },	/*	222			    */
  { SINGLECHAR, FALSE },	/*	223			    */
  { SINGLECHAR, FALSE },	/*	224			    */
  { SINGLECHAR, FALSE },	/*	225			    */
  { SINGLECHAR, FALSE },	/*	226			    */
  { SINGLECHAR, FALSE },	/*	227			    */
  { SINGLECHAR, FALSE },	/*	228			    */
  { SINGLECHAR, FALSE },	/*	229			    */
  { SINGLECHAR, FALSE },	/*	230			    */
  { SINGLECHAR, FALSE },	/*	231			    */
  { SINGLECHAR, FALSE },	/*	232			    */
  { SINGLECHAR, FALSE },	/*	233			    */
  { SINGLECHAR, FALSE },	/*	234			    */
  { SINGLECHAR, FALSE },	/*	235			    */
  { SINGLECHAR, FALSE },	/*	236			    */
  { SINGLECHAR, FALSE },	/*	237			    */
  { SINGLECHAR, FALSE },	/*	238			    */
  { SINGLECHAR, FALSE },	/*	239			    */
  { SINGLECHAR, FALSE },	/*	240			    */
  { SINGLECHAR, FALSE },	/*	241			    */
  { SINGLECHAR, FALSE },	/*	242			    */
  { SINGLECHAR, FALSE },	/*	243			    */
  { SINGLECHAR, FALSE },	/*	244			    */
  { SINGLECHAR, FALSE },	/*	245			    */
  { SINGLECHAR, FALSE },	/*	246			    */
  { SINGLECHAR, FALSE },	/*	247			    */
  { SINGLECHAR, FALSE },	/*	248			    */
  { SINGLECHAR, FALSE },	/*	249			    */
  { SINGLECHAR, FALSE },	/*	250			    */
  { SINGLECHAR, FALSE },	/*	251			    */
  { SINGLECHAR, FALSE },	/*	252			    */
  { SINGLECHAR, FALSE },	/*	253			    */
  { SINGLECHAR, FALSE },	/*	254			    */
  { SINGLECHAR, FALSE }		/*	255			    */
};

void
ScanCComment (void)
{
  inComment = TRUE;
  for (;;)
    {
      switch (currentChar)
	{
	case '*':
	  LCLMOVECHAR ();
	  if (currentChar == '/')
	    {
	      LCLMOVECHAR ();
	      inComment = FALSE;
	      return;
	    }
	  /*@switchbreak@*/ break;
	case '\n':
	  return;
	default:
	  LCLMOVECHAR ();
	}
    }
}

void
ScanEscape (void)
{
  if (isSimpleEscape[(int)currentChar])
    {
      LCLMOVECHAR ();		/* discard simple escape character. */
    }
  else if (currentChar == 'x')
    {
      LCLMOVECHAR ();		/* discard 'x'. */
      if (!isXigit[(int)currentChar])
	{
	  LocalUserError ("at least one hex digit must follow '\\x'");
	}
      while (isXigit[(int)currentChar])
	{
	  LCLMOVECHAR ();		/* discard hex digits. */
	}
    }
  else if (isOigit[(int)currentChar])
    {
      LCLMOVECHAR ();		/* discard first hex digit. */
      if (isOigit[(int)currentChar])
	{
	  LCLMOVECHAR ();		/* discard second hex digit. */
	}
      if (isOigit[(int)currentChar])
	{
	  LCLMOVECHAR ();		/* discard third hex digit. */
	}
    }
  else
    {
      LocalUserError ("invalid escape sequence in a C string or character");
    }
}

void
ScanCString (void)
{
  if (currentChar == '\\' && LOOKAHEADCHAR () == 'L')
    {
      LCLMOVECHAR ();		/* discard slash */
      LCLMOVECHAR ();		/* discard 'L'. */
    }

  if (currentChar == '\"')
    {
      LCLMOVECHAR ();		/* discard opening quote. */

      while (currentChar != '\"')
	{
	  if (isStrChar[(int)currentChar])
	    {
	      LCLMOVECHAR ();	/* discard string character. */
	    }
	  else if (currentChar == '\\')
	    {
	      LCLMOVECHAR ();	/* discard slash */
	      ScanEscape ();
	    }
	  else if (currentChar == '\n')
	    {
	      LocalUserError ("Unterminated C string");
	    }
	  else
	    {
	      LocalUserError ("Invalid character in C string");
	    }
	}
      LCLMOVECHAR ();		/* discard closing quote */

    }
  else
    {
      LocalUserError ("C string must start with '\"'");
    }


  *bufPtr = '\0';		/* null terminate in buffer */
  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
  tokenCode = LLT_LCSTRING;
}

void
ScanCChar (void)
{
  if (currentChar == '\\' && LOOKAHEADCHAR () == 'L')
    {
      LCLMOVECHAR ();		/* discard slash */
      LCLMOVECHAR ();		/* discard 'L'. */
    }

  if (currentChar == '\'')
    {
      LCLMOVECHAR ();		/* discard opening quote */

      while (currentChar != '\'')
	{
	  if (isCharChar[(int)currentChar])
	    {
	      LCLMOVECHAR ();	/* discard string character. */
	    }
	  else if (currentChar == '\\')
	    {
	      LCLMOVECHAR ();	/* discard slash */
	      ScanEscape ();
	    }
	  else if (currentChar == '\n')
	    {
	      LocalUserError ("Unterminated C character constant");
	    }
	  else
	    {
	      LocalUserError ("Invalid character in C character");
	    }
	}
      LCLMOVECHAR ();		/* discard closing quote */

    }
  else
    {
      LocalUserError ("Invalid C character");
    }


  *bufPtr = '\0';		/* null terminate in buffer */
  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
  tokenCode = LLT_CCHAR;
}

void
ScanCNumber (void)
{
  tokenCode = LLT_CINTEGER;
  
  switch (currentChar)
    {
    case '.':
      LCLMOVECHAR ();		
      tokenCode = LLT_CFLOAT;
      if (!isDigit[(int)currentChar])
	{
	  LocalUserError ("at least one digit must follow '.'");
	}
      while (isDigit[(int)currentChar])
	{
	  LCLMOVECHAR ();
	}
      if (currentChar == 'e' || currentChar == 'E')
	{
	  LCLMOVECHAR ();		/* discard 'e' or 'E'. */
	  if (currentChar == '+' || currentChar == '-')
	    {
	      LCLMOVECHAR ();
	    }
	  if (!isDigit[(int)currentChar])
	    {
	      LocalUserError ("digit must follow exponent");
	    }
	  while (isDigit[(int)currentChar])
	    {
	      LCLMOVECHAR ();
	    }
	}
      if (currentChar == 'f' || currentChar == 'l' ||
	  currentChar == 'F' || currentChar == 'L')
	{
	  LCLMOVECHAR ();
	}
      break;
      
    case '0':
      LCLMOVECHAR ();		/* discard '0'. */
      switch (currentChar)
	{
	case 'x':
	case 'X':
	  LCLMOVECHAR ();
	  if (!isXigit[(int)currentChar])
	    {
	      LocalUserError ("hex digit must follow 'x' or 'X'");
	    }
	  while (isXigit[(int)currentChar])
	    {
	      LCLMOVECHAR ();
	    }
	  /*@switchbreak@*/ break;

	default:
	  /*
	  ** Could either be an octal number or a floating point  
	  ** number.  Scan decimal digits so don't run into	    
	  ** problems if turns out problems if it is an fp	    
	  ** number.  Let converter/parser catch bad octal	    
	  ** numbers.   e.g. 018 not caught by scanner.	    
	  */
	  
	  while (isDigit[(int)currentChar])
	    {
	      LCLMOVECHAR ();
	    }
	  switch (currentChar)
	    {
	    case '.':
	      LCLMOVECHAR ();	/* discard '.'. */
	      tokenCode = LLT_CFLOAT;
	      while (isDigit[(int)currentChar])
		{
		  LCLMOVECHAR ();
		}
	      if (currentChar == 'e' || currentChar == 'E')
		{
		  LCLMOVECHAR ();	/* discard 'e' or 'E'. */
		  if (currentChar == '+' || currentChar == '-')
		    {
		      LCLMOVECHAR ();
		    }
		  if (!isDigit[(int)currentChar])
		    {
		      LocalUserError ("digit must follow exponent");
		    }
		  while (isDigit[(int)currentChar])
		    {
		      LCLMOVECHAR ();
		    }
		}
	      if (currentChar == 'f' ||
		  currentChar == 'l' ||
		  currentChar == 'F' ||
		  currentChar == 'L')
		{
		  LCLMOVECHAR ();
		}
	      /*@switchbreak@*/ break;

	    case 'e':
	    case 'E':
	      LCLMOVECHAR ();	
	      tokenCode = LLT_CFLOAT;
	      if (currentChar == '+' || currentChar == '-')
		{
		  LCLMOVECHAR ();
		}
	      if (!isDigit[(int)currentChar])
		{
		  LocalUserError ("digit must follow exponent");
		}
	      while (isDigit[(int)currentChar])
		{
		  LCLMOVECHAR ();
		}
	      if (currentChar == 'f' ||
		  currentChar == 'l' ||
		  currentChar == 'F' ||
		  currentChar == 'L')
		{
		  LCLMOVECHAR ();
		}
	      /*@switchbreak@*/ break;

	    default:
	      /* Scan integer suffix. */
	      switch (currentChar)
		{
		case 'u':
		case 'U':
		  LCLMOVECHAR ();	
		  if (currentChar == 'l' || currentChar == 'L')
		    {
		      LCLMOVECHAR ();
		    }
		  /*@switchbreak@*/ break;
		case 'l':
		case 'L':
		  LCLMOVECHAR ();	
		  if (currentChar == 'u' || currentChar == 'U')
		    {
		      LCLMOVECHAR ();
		    }
		  
		  /*@switchbreak@*/ break;
		}	
	      /*@switchbreak@*/ break;
	    }		
	}		       
      
      /* Scan integer suffix. */
      switch (currentChar)
	{
	case 'u':
	case 'U':
	  LCLMOVECHAR ();		
	  if (currentChar == 'l' || currentChar == 'L')
	    {
	      LCLMOVECHAR ();	
	    }
	  /*@switchbreak@*/ break;
	case 'l':
	case 'L':
	  LCLMOVECHAR ();		
	  if (currentChar == 'u' || currentChar == 'U')
	    {
	      LCLMOVECHAR ();	
	    }
	  /*@switchbreak@*/ break;
	}		
      break;
      
    default:
      if (isDigit[(int)currentChar])
	{
	  while (isDigit[(int)currentChar])
	    {
	      LCLMOVECHAR ();
	    }
	  switch (currentChar)
	    {
	    case '.':
	      LCLMOVECHAR ();	/* discard '.'. */
	      tokenCode = LLT_CFLOAT;
	      while (isDigit[(int)currentChar])
		{
		  LCLMOVECHAR ();
		}
	      if (currentChar == 'e' || currentChar == 'E')
		{
		  LCLMOVECHAR ();
		  if (currentChar == '+' || currentChar == '-')
		    {
		      LCLMOVECHAR ();
		    }
		  if (!isDigit[(int)currentChar])
		    {
		      LocalUserError ("digit must follow exponent");
		    }
		  while (isDigit[(int)currentChar])
		    {
		      LCLMOVECHAR ();
		    }
		}
	      if (currentChar == 'f' ||
		  currentChar == 'l' ||
		  currentChar == 'F' ||
		  currentChar == 'L')
		{
		  LCLMOVECHAR ();
		}
	      /*@switchbreak@*/ break;

	    case 'e':
	    case 'E':
	      LCLMOVECHAR ();	
	      tokenCode = LLT_CFLOAT;
	      if (currentChar == '+' || currentChar == '-')
		{
		  LCLMOVECHAR ();
		}
	      if (!isDigit[(int)currentChar])
		{
		  LocalUserError ("digit must follow exponent");
		}
	      while (isDigit[(int)currentChar])
		{
		  LCLMOVECHAR ();
		}
	      if (currentChar == 'f' ||
		  currentChar == 'l' ||
		  currentChar == 'F' ||
		  currentChar == 'L')
		{
		  LCLMOVECHAR ();
		}

	      /*@switchbreak@*/ break;
	    default:
	      switch (currentChar)
		{
		case 'u':
		case 'U':
		  LCLMOVECHAR (); 
		  if (currentChar == 'l' || currentChar == 'L')
		    {
		      LCLMOVECHAR ();
		    }
		  /*@switchbreak@*/ break;
		case 'l':
		case 'L':
		  LCLMOVECHAR ();	
		  if (currentChar == 'u' || currentChar == 'U')
		    {
		      LCLMOVECHAR ();
		    }
		  /*@switchbreak@*/ break;
		}		
	      /*@switchbreak@*/ break;
	    }			
	}
      else
	{
	  LocalUserError ("invalid C number");
	}
      break;


    }	

  *bufPtr = '\0';
  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
}

static void ScanOther (void)
{
  switch (LCLScanCharClass (currentChar))
    {			      
    case CHC_NULL:	
      tokenSym = lsymbol_fromChars ("E O L");
      tokenCode = LLT_EOL;
      break;
      
      /* identifiers */
      
    case IDCHAR:
      
      while (LCLScanCharClass (currentChar) == IDCHAR)
	{			/* identifier: find end	    */
	  LCLMOVECHAR ();
	}
      
      *bufPtr = '\0';		/* null terminate in buffer */
      tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
      tokenCode = simpleId;
            break;
      
      /* one-character tokens */
      
    case SINGLECHAR:
    case PERMCHAR:		
      LCLMOVECHAR ();
      *bufPtr = '\0';		
      tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
      tokenCode = simpleOp;
      break;

     /* operator symbols */

    case OPCHAR:

      if (currentChar == '.' &&  LOOKAHEADCHAR () == '.'  &&
	  LOOKAHEADTWICECHAR () == '.')
	{
	  LCLMOVECHAR ();
	  LCLMOVECHAR ();
	  LCLMOVECHAR ();
	  *bufPtr = '\0';
	  tokenSym = lsymbol_fromChars ("...");
	  tokenCode = LLT_TELIPSIS;
	}
      else
	{
	  if (currentChar == '/' && LOOKAHEADCHAR () == '\\')
	    {			
	      LCLMOVECHAR ();
	      LCLMOVECHAR ();
	    }
	  else
	    {			
	      while (LCLScanCharClass (currentChar) == OPCHAR)
		{	
		  LCLMOVECHAR ();
		}
	    }
	  
	  *bufPtr = '\0';
	  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
	  tokenCode = simpleOp;
	}
      break;
      
      /* white space */
    case WHITECHAR:
      /*@-loopswitchbreak@*/
      /*@-switchswitchbreak@*/
      switch (currentChar)
	{
	case '\t':
	  LCLMOVECHAR (); /* tabs only count as one character */
	  break;

	case '\v':
	case '\f':
	  LCLMOVECHAR ();
	  colNumber--;		/* does not change column   */
	  break;

	default:
	  LCLMOVECHAR ();
	  break;
	}
      /*@=switchswitchbreak@*/

      *bufPtr = '\0';	
      tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
      tokenCode = LLT_WHITESPACE;
      break;

     /* symbols */

    case CHC_EXTENSION:	
      LCLMOVECHAR ();

      /*@-switchswitchbreak@*/
      switch (currentChar)
	{		
	 /* open and close */
	case '(':		
	  LCLMOVECHAR ();
	  while (LCLScanCharClass (currentChar) == IDCHAR)
	    {			
	      LCLMOVECHAR ();
	    }
	  *bufPtr = '\0';	
	  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
	  tokenCode = openSym;
	  break;

	case ')':		
	  LCLMOVECHAR ();
	  while (LCLScanCharClass (currentChar) == IDCHAR)
	    {			
	      LCLMOVECHAR ();
	    }
	  *bufPtr = '\0';	
	  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
	  tokenCode = closeSym;
	  break;

	 /* separator */
	case ',':	
	  LCLMOVECHAR ();
	  while (LCLScanCharClass (currentChar) == IDCHAR)
	    {		
	      LCLMOVECHAR ();
	    }
	  *bufPtr = '\0';
	  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
	  tokenCode = sepSym;
	  break;

	 /* simpleid */
	case ':':	
	  LCLMOVECHAR ();
	  while (LCLScanCharClass (currentChar) == IDCHAR)
	    {		
	      LCLMOVECHAR ();
	    }
	  *bufPtr = '\0';
	  tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
	  tokenCode = simpleId;
	  break;

	default:		
	  if (LCLScanCharClass (currentChar) == IDCHAR)
	    {
	      do
		{
		  LCLMOVECHAR ();
		}
	      while (LCLScanCharClass (currentChar) == IDCHAR);
	      *bufPtr = '\0';	
	      tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
	      tokenCode = simpleOp;
	    }
	  else
	    {
	      /*
	      ** Meets none of the above.  Take the extension	    
	      ** character and the character following and treat	   
	      ** together as a SINGLECHAR.  SINGLECHARs tranlate into 
	      ** SIMPLEOPs.					    
	      */

	      LCLMOVECHAR ();
	      *bufPtr = '\0';	
	      tokenSym = lsymbol_fromChars (&tokenBuffer[0]);
	      tokenCode = simpleOp;
	    }
	  break;
	  /*@=switchswitchbreak@*/
	}
      break;			

    default:
      LocalUserError ("unexpected character in input");
      return;
    }
  /*@=loopswitchbreak@*/
}

static bool
nextCanBeCharLiteral (ltokenCode c)
{
  switch (c)
    {
      /* A ' following these tokens starts a C character literal. */
    case logicalOp:		
    case equationSym:		
    case eqSepSym:		
    case openSym:		
    case sepSym:		
    case simpleOp:		
    case LLT_COMMA:		        
    case LLT_EQUALS:		
    case LLT_LBRACE:		
    case LLT_LBRACKET:		
    case LLT_LPAR:			
    case eqOp:			
    case LLT_BE:			
    case LLT_ELSE:			
    case LLT_ENSURES:		
    case LLT_IF:			
    case LLT_CONSTRAINT:		
    case LLT_REQUIRES:		
    case LLT_CHECKS:                
    case LLT_BODY:			
    case LLT_THEN:			
      return (TRUE);

     /* A ' following these tokens means post */
    case selectSym:		
    case closeSym:		
    case simpleId:		
    case preSym:		
    case anySym:		
    case postSym:		
    case LLT_QUOTE:		        
    case LLT_RBRACE:		
    case LLT_RBRACKET:		
    case LLT_RPAR:			
    case LLT_RESULT:		
      return (FALSE);

     /* Neither a C character literal nor post should follow these tokens */
    case quantifierSym:	        
    case mapSym:		
    case markerSym:		
    case LLT_COLON:	        	
    case LLT_SEMI:			
    case LLT_VERTICALBAR:		
    case LLT_MULOP:  		
    case LLT_CCHAR:		        
    case LLT_CFLOAT:		
    case LLT_CINTEGER:		
    case LLT_LCSTRING:		
    case LLT_ALL:			
    case LLT_ANYTHING:		
    case LLT_CONSTANT:		
    case LLT_FOR:			
    case LLT_IMMUTABLE:		
    case LLT_OBJ:			
    case LLT_OUT:			
    case LLT_IMPORTS:		
    case LLT_ISSUB: 		
    case LLT_LET:			
    case LLT_MODIFIES:
    case LLT_CLAIMS:
    case LLT_MUTABLE:		
    case LLT_FRESH:
    case LLT_NOTHING:		
    case LLT_PRIVATE:		
    case LLT_SPEC:			
    case LLT_SIZEOF:		
    case LLT_TAGGEDUNION:		
    case LLT_TYPE:			
    case LLT_UNCHANGED:		
    case LLT_USES:			
    case LLT_CHAR:			
    case LLT_CONST:		        
    case LLT_DOUBLE:		
    case LLT_ENUM:			
    case LLT_FLOAT:		        
    case LLT_INT:			
    case LLT_TYPEDEF_NAME:		
    case LLT_LONG:			
    case LLT_SHORT:		        
    case LLT_STRUCT:		
    case LLT_SIGNED:		
    case LLT_UNKNOWN:		
    case LLT_UNION:		        
    case LLT_UNSIGNED:		
    case LLT_VOID:			
    case LLT_VOLATILE:		
      return (FALSE);

     /* These tokens should have been ignored */
    case NOTTOKEN:
    case commentSym:		
    case LLT_WHITESPACE:
    case LLT_EOL:
    case LEOFTOKEN:
      llcontbuglit ("scanline: nextCanBeChar");
      return FALSE;
    BADDEFAULT;
    }

  BADEXIT;
}

void
LCLScanLine (char *line)
{
  ltoken newToken;
  lsymbol CCommentSym = lsymbol_fromChars ("/*");
  size_t linelength = strlen (line);
  static bool inSpecComment = FALSE;

    line[(int)linelength] = '\n';

  currentLine = line;
  currentChar = *currentLine++;	
  context_processedSpecLine ();

  incLine ();
  colNumber = 1;		

  if (inComment)
    {
      ScanCComment ();

      if (reportComments)
	{
	  *bufPtr = '\0';
	  newToken = ltoken_createRaw (simpleId, lsymbol_fromChars (&tokenBuffer[0]));
	  LCLScanFreshToken (newToken);
	}
    }

  if (inSpecComment)
    {
      if (currentChar == '*' &&
	  LOOKAHEADCHAR () == '/')
	{
	  LCLMOVECHAR ();
	  LCLMOVECHAR ();
	  inSpecComment = FALSE;
	}
    }

  /*@+loopexec@*/ 
  for (;;)
    {
      if (inSpecComment && currentChar == '*' && LOOKAHEADCHAR () == '/')
	{
	  LCLMOVECHAR ();
	  LCLMOVECHAR ();
	  inSpecComment = FALSE;
	}

      bufPtr = &tokenBuffer[0];	
      startCol = colNumber;	

      
      /*@-loopswitchbreak@*/
      switch (startClass[(int)currentChar])
	{
	case STARTCNUM:
	  ScanCNumber ();
	  break;

	case STARTCNUMDOT:
	  if (isDigit[(int) LOOKAHEADCHAR ()])
	    {
	      ScanCNumber ();
	    }
	  else
	    {
	      ScanOther ();
	    }
	  break;

	case STARTCSTR:
	  ScanCString ();
	  break;

	case STARTCCHAR:
	  	  if (nextCanBeCharLiteral (prevTokenCode))
	    {
	      ScanCChar ();
	    }
	  else
	    {
	      	      ScanOther ();
	    }
	  break;

	case STARTWIDE:
	  if (LOOKAHEADCHAR () == 'L' && LOOKAHEADTWICECHAR () == '\"')
	    {
	      ScanCString ();
	    }
	  else if (LOOKAHEADCHAR () == 'L' && LOOKAHEADTWICECHAR () == '\'')
	    {
	      ScanCChar ();
	    }
	  else
	    {
	      ScanOther ();
	    }
	  break;

	case STARTSLASH:
	  if (LOOKAHEADCHAR () == '*')
	    {
	      LCLMOVECHAR ();
	      LCLMOVECHAR ();

	      if (currentChar == '@')
		{
		  char *s = mstring_createEmpty ();

		  LCLMOVECHAR ();

		  while (currentChar != '\0' && currentChar != ' ' 
			 && currentChar != '*' && currentChar != '\t' &&
			 currentChar != '\n')
		    {
		      s = mstring_append (s, currentChar);
		      LCLMOVECHAR ();
		    }

		  if (mstring_equal (s, "alt"))
		    {
		      tokenCode = LLT_VERTICALBAR;
		      tokenSym = lsymbol_fromChars ("|");
		      inSpecComment = TRUE;
		    }
		  else
		    {
		      ScanCComment ();
		      tokenCode = commentSym;
		      tokenSym = CCommentSym;
		    }

		  sfree (s);
		  break;
		}
	      else
		{
		  ScanCComment ();
		  tokenCode = commentSym;
		  tokenSym = CCommentSym;
		  break;
		}
	    }
	  else
	    {
	      ScanOther ();
	    } break;

	case STARTOTHER:
	  ScanOther ();
	  break;

	default:
	  llcontbuglit ("LCLScanLine: bad case");
	  break;

	}
      /*@=loopswitchbreak@*/
      
     /*
     ** Above code only "guessed" at token type.  Insert it into the 
     ** TokenTable.  If the token already exists, it is returned as	    
     ** previously defined.  If it does not exist, it is inserted as the 
     ** token code computed above.					    
     */
      
      newToken = LCLInsertToken (tokenCode, tokenSym, lsymbol_undefined, FALSE);
      
                  
      if (LCLIsSyn (ltoken_getText (newToken)))
	{
	  /*
	  ** Token is a synonym.  Get the actual token and set the raw    
	  ** text to the synonym name.				    
	  */

	  newToken = ltoken_copy (LCLGetTokenForSyn (ltoken_getText (newToken)));

	  ltoken_setRawText (newToken, tokenSym);
	}
      else
	{
	  newToken = ltoken_copy (newToken);
	}

      ltoken_setCol (newToken, startCol);
      ltoken_setLine (newToken, inputStream_thisLineNumber (LCLScanSource ()));
      ltoken_setFileName (newToken, inputStream_fileName (LCLScanSource ()));

      if (ltoken_getCode (newToken) == commentSym)
	{
	  if (tokenSym == CCommentSym)
	    { /* C-style comment   */
	      ltoken_free (commentTok);
	      commentTok = ltoken_copy (newToken);

	      if (!inComment && reportComments)
		{
		  *bufPtr = '\0';
		  ltoken_setRawText (newToken, 
				     lsymbol_fromChars (&tokenBuffer[0]));
		  LCLScanFreshToken (newToken);
		}
	      else
		{
		  ltoken_free (newToken); 
		}
	    }
	  else
	    { /* LSL-style comment */
	      bufPtr = &tokenBuffer[0];
	      while (!LCLIsEndComment (currentChar))
		{
		  LCLMOVECHAR ();
		}
	      if (LCLScanCharClass (currentChar) != CHC_NULL)
		{
		  /* Not EOL character.  Toss it out. */
		  LCLMOVECHAR ();
		}

	      if (reportComments)
		{
		  *bufPtr = '\0';
		  ltoken_setRawText (newToken, 
				     lsymbol_fromChars (&tokenBuffer[0]));
		  LCLScanFreshToken (newToken);
		}
	      else
		{
		  ltoken_free (newToken);
		}
	    }
	}
      else if (ltoken_getCode (newToken) == LLT_EOL)
	{
	  if (reportEOL)
	    {
	      LCLScanFreshToken (newToken);
	    }
	  else
	    {
	      ltoken_free (newToken); 
	    }

	  line[(int) linelength] = '\0';
	  return;
	}
      else if (ltoken_getCode (newToken) != LLT_WHITESPACE)
	{
	  prevTokenCode = ltoken_getCode (newToken);
	  LCLScanFreshToken (newToken);
	}
      else
	{
	  ltoken_free (newToken);
	}
    } /*@=loopexec@*/
}

/*@exposed@*/ ltoken
LCLScanEofToken (void)
{
  ltoken t = LCLInsertToken (LEOFTOKEN, lsymbol_fromChars ("E O F"), 0, TRUE);

  if (inComment)
    {
      lclerror (commentTok, cstring_makeLiteral ("Unterminated comment"));
    }

  ltoken_setCol (t, colNumber);
  ltoken_setLine (t, inputStream_thisLineNumber (LCLScanSource ()));
  ltoken_setFileName (t, inputStream_fileName (LCLScanSource ()));

  return t;
}

void
LCLReportEolTokens (bool setting)
{
  reportEOL = setting;
}

static void
LocalUserError (char *msg)
{
  inputStream s = LCLScanSource ();
  llfatalerror (message ("%s:%d,%d: %s", 
			 inputStream_fileName (s), 
			 inputStream_thisLineNumber (s),
			 colNumber,
			 cstring_fromChars (msg)));
}

void
LCLScanLineInit (void)
{
  int i;

  setCodePoint ();
  reportEOL = FALSE;
  reportComments = FALSE;

  for (i = 0; i <= LASTCHAR; i++)
    {
      LCLcharClass[i] = charClassDef[i];
    }

  setCodePoint ();

  /*
  ** Make sure first postion is never used because use the 0th index to   
  ** mean empty. 
  */

  firstReserved = lsymbol_fromChars (FIRSTRESERVEDNAME);
  setCodePoint ();

  /* Predefined LSL Tokens */
  
  ltoken_forall = LCLReserveToken (quantifierSym, "\\forall");
  setCodePoint ();
  ltoken_exists = LCLReserveToken (quantifierSym, "\\exists");
  ltoken_implies = LCLReserveToken (logicalOp, "\\implies");
  ltoken_eqsep = LCLReserveToken (eqSepSym, "\\eqsep");
  ltoken_select = LCLReserveToken (selectSym, "\\select");
  ltoken_open = LCLReserveToken (openSym, "\\open");
  ltoken_sep = LCLReserveToken (sepSym, "\\,");
  ltoken_close = LCLReserveToken (closeSym, "\\close");
  ltoken_id = LCLReserveToken (simpleId, "\\:");
  ltoken_arrow = LCLReserveToken (mapSym, "\\arrow");
  ltoken_marker = LCLReserveToken (markerSym, "\\marker");
  ltoken_pre = LCLReserveToken (preSym, "\\pre");
  ltoken_post = LCLReserveToken (postSym, "\\post");
  ltoken_comment = LCLReserveToken (commentSym, "\\comment");
  ltoken_any = LCLReserveToken (anySym, "\\any");

  ltoken_result = LCLReserveToken (LLT_RESULT, "result");
  ltoken_typename = LCLReserveToken (LLT_TYPEDEF_NAME, "TYPEDEF_NAME");
  ltoken_setIdType (ltoken_typename, SID_TYPE);

  /*
  ** Not context_getBoolName () --- "bool" is built in to LCL.
  ** This is bogus, but necessary for a lot of old lcl files.
  */

  ltoken_bool = LCLReserveToken (LLT_TYPEDEF_NAME, "bool");

  ltoken_lbracked = LCLReserveToken (LLT_LBRACKET, "[");
  ltoken_rbracket = LCLReserveToken (LLT_RBRACKET, "]");

  (void) LCLReserveToken (LLT_COLON, ":");
  (void) LCLReserveToken (LLT_COMMA, ",");

  (void) LCLReserveToken (LLT_EQUALS, "=");
  (void) LCLReserveToken (LLT_LBRACE, "{");
  (void) LCLReserveToken (LLT_LPAR, "(");
  (void) LCLReserveToken (LLT_RBRACE, "}");
  (void) LCLReserveToken (LLT_RPAR, ")");
  (void) LCLReserveToken (LLT_SEMI, ";");
  (void) LCLReserveToken (LLT_VERTICALBAR, "|");

  (void) LCLReserveToken (LLT_MULOP, "*");

  (void) LCLReserveToken (LLT_WHITESPACE, " ");
  (void) LCLReserveToken (LLT_WHITESPACE, "\t");
  (void) LCLReserveToken (LLT_WHITESPACE, "\f");
  (void) LCLReserveToken (LLT_WHITESPACE, "\n");

  (void) LCLReserveToken (LEOFTOKEN, "E O F");
  (void) LCLReserveToken (LLT_EOL, "E O L");

  /* LSL Keywords */
  ltoken_and = LCLReserveToken (logicalOp, "\\and");
  ltoken_or = LCLReserveToken (logicalOp, "\\or");

  ltoken_equals = LCLReserveToken (equationSym, "\\equals");

  ltoken_eq = LCLReserveToken (eqOp, "\\eq");
  ltoken_neq = LCLReserveToken (eqOp, "\\neq");

  ltoken_not = LCLReserveToken (simpleOp, "\\not");
  ltoken_true = LCLReserveToken (simpleId, "true");
  ltoken_false = LCLReserveToken (simpleId, "false");

  /* LCL Keywords */
  (void) LCLReserveToken (LLT_ALL, "all");
  (void) LCLReserveToken (LLT_ANYTHING, "anything");
  (void) LCLReserveToken (LLT_BE, "be");
  (void) LCLReserveToken (LLT_CONSTANT, "constant");
  (void) LCLReserveToken (LLT_CHECKS, "checks");
  (void) LCLReserveToken (LLT_ELSE, "else");
  (void) LCLReserveToken (LLT_ENSURES, "ensures");
  (void) LCLReserveToken (LLT_FOR, "for");
  (void) LCLReserveToken (LLT_IF, "if");
  (void) LCLReserveToken (LLT_IMMUTABLE, "immutable");
  (void) LCLReserveToken (LLT_OBJ, "obj");
  (void) LCLReserveToken (LLT_OUT, "out");
  (void) LCLReserveToken (LLT_ITER, "iter"); 
  (void) LCLReserveToken (LLT_YIELD, "yield"); 
  (void) LCLReserveToken (LLT_PARTIAL, "partial");
  (void) LCLReserveToken (LLT_ONLY, "only");
  (void) LCLReserveToken (LLT_UNDEF, "undef");
  (void) LCLReserveToken (LLT_KILLED, "killed");
  (void) LCLReserveToken (LLT_OWNED, "owned");
  (void) LCLReserveToken (LLT_DEPENDENT, "dependent");
  (void) LCLReserveToken (LLT_PARTIAL, "partial");
  (void) LCLReserveToken (LLT_RELDEF, "reldef");
  (void) LCLReserveToken (LLT_KEEP, "keep");
  (void) LCLReserveToken (LLT_KEPT, "kept");
  (void) LCLReserveToken (LLT_TEMP, "temp");
  (void) LCLReserveToken (LLT_SHARED, "shared");
  (void) LCLReserveToken (LLT_RELNULL, "relnull");
  (void) LCLReserveToken (LLT_RELDEF, "reldef");
  (void) LCLReserveToken (LLT_CHECKED, "checked");
  (void) LCLReserveToken (LLT_UNCHECKED, "unchecked");
  (void) LCLReserveToken (LLT_CHECKEDSTRICT, "checkedstrict");
  (void) LCLReserveToken (LLT_CHECKMOD, "checkmod");
  (void) LCLReserveToken (LLT_TRUENULL, "truenull");
  (void) LCLReserveToken (LLT_FALSENULL, "falsenull");
  (void) LCLReserveToken (LLT_LNULL, "null");
  (void) LCLReserveToken (LLT_LNOTNULL, "notnull");
  (void) LCLReserveToken (LLT_RETURNED, "returned");
  (void) LCLReserveToken (LLT_OBSERVER, "observer");
  (void) LCLReserveToken (LLT_EXPOSED, "exposed");
  (void) LCLReserveToken (LLT_REFCOUNTED, "refcounted");
  (void) LCLReserveToken (LLT_REFS, "refs");
  (void) LCLReserveToken (LLT_NEWREF, "newref");
  (void) LCLReserveToken (LLT_TEMPREF, "tempref");
  (void) LCLReserveToken (LLT_KILLREF, "killref");
  (void) LCLReserveToken (LLT_NULLTERMINATED, "nullterminated");
  (void) LCLReserveToken (LLT_EXITS, "exits");
  (void) LCLReserveToken (LLT_MAYEXIT, "mayexit");
  (void) LCLReserveToken (LLT_TRUEEXIT, "trueexit");
  (void) LCLReserveToken (LLT_FALSEEXIT, "falseexit");
  (void) LCLReserveToken (LLT_NEVEREXIT, "neverexit");
  (void) LCLReserveToken (LLT_SEF, "sef");
  (void) LCLReserveToken (LLT_UNUSED, "unused");
  (void) LCLReserveToken (LLT_UNIQUE, "unique");
  (void) LCLReserveToken (LLT_IMPORTS, "imports");
  (void) LCLReserveToken (LLT_CONSTRAINT, "constraint");
  (void) LCLReserveToken (LLT_LET, "let");
  (void) LCLReserveToken (LLT_MODIFIES, "modifies");
  (void) LCLReserveToken (LLT_CLAIMS, "claims");
  (void) LCLReserveToken (LLT_BODY, "body");
  (void) LCLReserveToken (LLT_MUTABLE, "mutable");
  (void) LCLReserveToken (LLT_FRESH, "fresh");
  (void) LCLReserveToken (LLT_NOTHING, "nothing");
  (void) LCLReserveToken (LLT_INTERNAL, "internalState");
  (void) LCLReserveToken (LLT_FILESYS, "fileSystem");
  (void) LCLReserveToken (LLT_PRIVATE, "private");
  (void) LCLReserveToken (LLT_SPEC, "spec");
  (void) LCLReserveToken (LLT_REQUIRES, "requires");
  (void) LCLReserveToken (LLT_SIZEOF, "sizeof");
  (void) LCLReserveToken (LLT_TAGGEDUNION, "taggedunion");
  (void) LCLReserveToken (LLT_THEN, "then");
  (void) LCLReserveToken (LLT_TYPE, "type");
  (void) LCLReserveToken (LLT_TYPEDEF, "typedef");
  (void) LCLReserveToken (LLT_UNCHANGED, "unchanged");
  (void) LCLReserveToken (LLT_USES, "uses");
  (void) LCLReserveToken (LLT_PRINTFLIKE, "printflike");
  (void) LCLReserveToken (LLT_SCANFLIKE, "scanflike");
  (void) LCLReserveToken (LLT_MESSAGELIKE, "messagelike");

  /* LCL C Keywords */
  (void) LCLReserveToken (LLT_CHAR, "char");
  (void) LCLReserveToken (LLT_CONST, "const");
  (void) LCLReserveToken (LLT_DOUBLE, "double");
  (void) LCLReserveToken (LLT_ENUM, "enum");

  /* comment out so we can add in lclinit.lci: synonym double float */
  /* LCLReserveToken (LLT_FLOAT,		    "float"); */
  /* But we need to make the scanner parse "float" not as a simpleId, but
     as a TYPEDEF_NAME.  This is done later in abstract_init  */
  
  (void) LCLReserveToken (LLT_INT, "int");
  (void) LCLReserveToken (LLT_LONG, "long");
  (void) LCLReserveToken (LLT_SHORT, "short");
  (void) LCLReserveToken (LLT_STRUCT, "struct");
  (void) LCLReserveToken (LLT_SIGNED, "signed");
  (void) LCLReserveToken (LLT_UNION, "union");
  (void) LCLReserveToken (LLT_UNKNOWN, "__unknown");
  (void) LCLReserveToken (LLT_UNSIGNED, "unsigned");
  (void) LCLReserveToken (LLT_VOID, "void");
  (void) LCLReserveToken (LLT_VOLATILE, "volatile");
  setCodePoint ();
}

void
LCLScanLineReset (void)
{
  inComment = FALSE;
  prevTokenCode = LLT_LPAR;		/* Presume first ' starts literal */
}

void
LCLScanLineCleanup (void)
{
}

bool LCLIsEndComment (char c)
{
  return LCLcharClass[(int)(c)].endCommentChar;
}

charCode LCLScanCharClass (char c)
{
  return LCLcharClass[(int)(c)].code;
}

void LCLSetCharClass (char c, charCode cod) 
{
  LCLcharClass[(int)(c)].code = (cod);
}

void LCLSetEndCommentChar (char c, bool flag) 
{
  LCLcharClass[(int)(c)].endCommentChar = flag;
}

