/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/

# ifndef READER_H
# define READER_H

extern /*@null@*/ char *reader_readLine (FILE *p_f, /*@returned@*/ /*@out@*/ char *p_s, int p_max) 
   /*@modifies *p_f, p_s@*/ ;

extern int reader_getInt (char **p_s) /*@modifies *p_s@*/ ;
extern char reader_loadChar (char **p_s) /*@modifies *p_s@*/ ; 
extern double reader_getDouble (char **p_s) /*@modifies *p_s@*/ ;

extern void reader_doCheckChar (char ** p_s, char p_c, char *p_file, int p_line)
   /*@modifies *p_s;@*/;

extern bool reader_optCheckChar (char **p_s, char p_c) /*@modifies *p_s;@*/ ;
extern void reader_checkChar (char **p_s, char p_c) /*@modifies *p_s;@*/ ;
# define reader_checkChar(s,c)  (reader_doCheckChar (s, c, __FILE__, __LINE__))
extern /*@only@*/ /*@null@*/ char *reader_getWord (char **p_s) /*@modifies *p_s@*/ ;

extern cstring reader_getStringWord (char **p_s) /*@modifies *p_s@*/ ;
# define reader_getStringWord(s)     (cstring_fromChars(reader_getWord(s)))

extern cstring reader_readUntil (char **p_s, char p_x) /*@modifies *p_s@*/ ;
extern cstring reader_readUntilOne (char **p_s, char *p_x) /*@modifies *p_s@*/ ;

extern void reader_checkUngetc (int p_c, FILE *p_f) /*@modifies *p_f@*/ ;

# else
# error "Multiple include"
# endif






