/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
# ifndef GLOBALS_H
# define GLOBALS_H

extern /*@owned@*/ fileloc g_currentloc;

/*
** All the streams are unchecked for now. 
*/

/* stream for warning messages */
/*@unchecked@*/ extern /*@relnull@*/ /*@dependent@*/ /*:open:*/ FILE *g_warningstream; 

/* stream for status messages */
/*@unchecked@*/ extern /*@relnull@*/ /*@dependent@*/ /*:open:*/ FILE *g_messagestream; 

/* stream for error messages */
/*@unchecked@*/ extern /*@relnull@*/ /*@dependent@*/ /*:open:*/ FILE *g_errorstream; 

/* stream for error messages */
/*@unchecked@*/ extern /*@null@*/ /*@dependent@*/ /*:open:*/ FILE *g_csvstream; 

/*@-ansireserved@*/
/* This macro is defined by flex. */
/*@constant external int ECHO@*/
/*@=ansireserved@*/

/*@-redecl@*/
/*@-incondefs@*/ 
/*@-namechecks@*/
/*@-declundef@*/ /* Might not process grammar files */
extern /*@dependent@*/ /*@open@*/ FILE *yyin;
extern /*@dependent@*/ /*@open@*/ /*@unused@*/ FILE *yyout;
extern /*@unused@*/ int yyleng;
/*@=incondefs@*/ 

extern int yydebug;
/*@=redecl@*/
/*@=declundef@*/
/*@=namechecks@*/

extern /*@observer@*/ cstring g_codeFile;
extern int g_codeLine;

extern /*@observer@*/ cstring g_prevCodeFile;
extern int g_prevCodeLine;

extern /*@observer@*/ char *g_localSpecPath;

extern /*@only@*/ cstring g_currentSpec;
extern /*@null@*/ /*@only@*/ char *g_currentSpecName;

extern void setCodePoint (void);
# define setCodePoint() \
  (g_prevCodeFile = g_codeFile, g_prevCodeLine = g_codeLine, \
   g_codeFile = cstring_makeLiteralTemp (__FILE__), g_codeLine = __LINE__)

extern void printCodePoint (void);

extern fileId currentFile (void) /*@globals g_currentloc; @*/ ;
# define currentFile()               (fileloc_fileId (g_currentloc))

extern int currentColumn (void) /*@globals g_currentloc; @*/ ;
# define currentColumn()             (fileloc_column(g_currentloc))

extern void incColumn (void) 
   /*@globals g_currentloc; @*/ 
   /*@modifies g_currentloc@*/ ;
# define incColumn()                  (fileloc_incColumn(g_currentloc)) 

extern void decColumn (void)
   /*@globals g_currentloc; @*/ 
   /*@modifies g_currentloc@*/ ;
# define decColumn()                  (fileloc_addColumn(g_currentloc, -1))

extern void incLine (void)
   /*@globals g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define incLine()                    (fileloc_nextLine(g_currentloc))

extern void decLine (void)
   /*@globals g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define decLine()                    (fileloc_addLine (g_currentloc, -1))

extern void beginLine (void)
   /*@globals g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define beginLine()                  (fileloc_setColumn(g_currentloc, 1))

extern void addColumn (int p_n)
   /*@globals g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define addColumn(n)                 (fileloc_addColumn(g_currentloc, n))

extern void setLine (int p_n)
   /*@globals fileloc g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define setLine(n)                   (fileloc_setLineno(g_currentloc, n))

extern void setColumn (int p_n)
   /*@globals fileloc g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define setColumn(n)                 (fileloc_setColumn(g_currentloc, n))

extern void setSpecFileId (fileId p_s)
   /*@globals fileloc g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define setSpecFileId(s) \
  (fileloc_reallyFree (g_currentloc), g_currentloc = fileloc_createSpec (s, 1, 1))

extern void setFileLine (fileId p_s, int p_line)
   /*@globals fileloc g_currentloc; @*/ 
   /*@modifies g_currentloc; @*/ ;
# define setFileLine(s, line) \
  (context_setFilename(s, line))

# else
# error "Multiple include"
# endif






