/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef LLMAIN_H
# define LLMAIN_H

extern int main (int p_argc, char *p_argv[])
  /*@globals killed undef g_currentloc,
	     killed undef g_currentSpec,
	     killed undef g_currentSpecName,
	     killed undef g_currentloc,
	     killed undef yyin,
                    undef g_warningstream;
   @*/
  /*@modifies g_currentloc, g_currentSpec, g_currentSpecName,
	      yyin, internalState, fileSystem; 
  @*/ 
  ;

extern /*@noreturn@*/ void llexit (int p_status);      
extern void showHerald (void);

/*@-redecl@*/ /*@-incondefs@*/
/*@-declundef@*/
extern int yyparse (void);
extern void yyrestart (/*@dependent@*/ FILE *);

extern int ylparse (void);
extern int lslparse (void);

extern /*:open:*/ /*@dependent@*/ FILE *yyin;
/*@=incondefs@*/ /*@=redecl@*/
/*@=declundef@*/

# endif


