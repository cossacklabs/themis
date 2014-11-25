/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/
/*
** inputStream.h
*/

# ifndef INPUTSTREAM_H
# define INPUTSTREAM_H

/*@constant int STUBMAXRECORDSIZE; @*/
# define STUBMAXRECORDSIZE 800

struct s_inputStream {
  /*@only@*/ cstring name;
  /*:open:*/ /*@dependent@*/ /*@null@*/ FILE *file;
  char buffer[STUBMAXRECORDSIZE+1];
  int lineNo;
  size_t charNo;
  /*@dependent@*/ /*@null@*/ char *curLine;
  bool echo, fromString;
  /*@owned@*/ cstring  stringSource;
  /*@dependent@*/ cstring stringSourceTail;
} ;

/* in forwardTypes.h: abst_typedef null struct _inputStream *inputStream; */

extern /*@falsewhennull@*/ bool inputStream_isDefined (/*@null@*/ inputStream p_f) /*@*/ ;
extern /*@nullwhentrue@*/ bool inputStream_isUndefined (/*@null@*/ inputStream p_f) /*@*/ ;

/*@constant null inputStream inputStream_undefined; @*/
# define inputStream_undefined      ((inputStream) NULL)
# define inputStream_isDefined(f)   ((f) != inputStream_undefined)
# define inputStream_isUndefined(f) ((f) == inputStream_undefined)

extern void inputStream_free (/*@null@*/ /*@only@*/ inputStream p_s);
extern bool inputStream_close (inputStream p_s) 
   /*@modifies p_s, fileSystem@*/ ; 

extern /*@only@*/ inputStream
  inputStream_create (/*@only@*/ cstring p_name, cstring p_suffix, bool p_echo) /*@*/ ;

extern inputStream inputStream_fromString (cstring p_name, cstring p_str) /*@*/ ;
extern /*@dependent@*/ /*@null@*/ char *inputStream_nextLine(inputStream p_s) 
   /*@modifies p_s@*/ ;

extern int inputStream_nextChar (inputStream p_s) /*@modifies p_s@*/ ;
    /* Returns int for EOF */

extern int inputStream_peekChar (inputStream p_s) /*@modifies p_s@*/ ;
    /* Returns int for EOF */

extern int inputStream_peekNChar (inputStream p_s, int p_n) /*@modifies p_s@*/ ;
    /* Returns int for EOF */

extern bool inputStream_open (inputStream p_s) /*@modifies p_s, fileSystem@*/ ;
extern bool inputStream_getPath (cstring p_path, inputStream p_s)
   /*@modifies p_s@*/ ;

extern /*@observer@*/ cstring inputStream_fileName (inputStream p_s) /*@*/ ;
extern bool inputStream_isOpen (/*@sef@*/ inputStream p_s) /*@*/ ;
extern int inputStream_thisLineNumber(inputStream p_s) /*@*/ ;
extern /*:open:*/ /*@exposed@*/ FILE *inputStream_getFile (inputStream p_s) /*@*/ ;

# else
# error "Multiple include"
# endif





