/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** scanline.h
*/

# ifndef SCANLINE_H
# define SCANLINE_H

/*@constant int LASTCHAR; @*/
# define LASTCHAR 255

/*@constant char CHAREXTENDER; @*/
# define CHAREXTENDER '\\'

typedef enum {
    CHC_NULL,
    IDCHAR,
    OPCHAR,
    SLASHCHAR,
    WHITECHAR,
    CHC_EXTENSION,
    SINGLECHAR,
    PERMCHAR
} charCode;

typedef struct {
  charCode code;
  bool endCommentChar;
} charClassData;

extern void lscanLine (char *);
extern ltoken LSLScanEofToken (void);
extern void LSLReportEolTokens (bool p_setting);

extern void lscanLineInit (void);
extern void lscanLineReset (void);
extern void lscanLineCleanup (void);

/* 
** The following function prototypes are for use by lslinit.c, which
** customizes subsequent actions of scanline.c.
*/

extern charCode lscanCharClass (char p_c);
extern bool LSLIsEndComment (char p_c);
extern void lsetCharClass (char p_c, charCode p_cod);
extern void lsetEndCommentChar (char p_c, bool p_flag);

/*@constant int MAXLINE;@*/
# define MAXLINE 1000		

# else
# error "Multiple include"
# endif
