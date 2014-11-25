/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** ltoken.h
*/

# ifndef LTOKEN_H
# define LTOKEN_H

/* SimpleId codes */
typedef enum {
  SID_VAR,          /* VarId */
  SID_TYPE,         /* TypeId */
  SID_OP,           /* OpId */
  SID_SORT          /* sortId */ 
} SimpleIdCode;

struct s_ltoken {
  ltokenCode code;				
  int col;			
  int line;			
  
  /* if idtype is SID_SORT, then text field keeps the sort */
  lsymbol text;			/* string handle    */
  lsymbol fname;		        /* source handle    */
  lsymbol rawText;			/* original text    */
  bool defined: 1;	        	/* token predefined */
  bool hasSyn: 1;		        /* synonym exists   */
  
  /* just for simpleId: for distinguish simpleId into varId, fcnId, TypeId, ... */
  /*@reldef@*/ SimpleIdCode idtype; 
  
  /* quick and dirty: just for ctypes */
  /*@reldef@*/ unsigned int intfield; 
} ;

typedef /*@only@*/ ltoken o_ltoken;

/*@constant null ltoken ltoken_undefined; @*/
# define ltoken_undefined  ((ltoken)NULL)

extern /*@falsewhennull@*/ bool ltoken_isValid (ltoken p_tok);
# define ltoken_isValid(t) ((t) != ltoken_undefined)

extern /*@nullwhentrue@*/ bool ltoken_isUndefined (ltoken p_tok);
# define ltoken_isUndefined(t) ((t) == ltoken_undefined)

extern bool ltoken_isStateDefined (/*@sef@*/ ltoken p_tok) /*@*/ ;
# define ltoken_isStateDefined(t) \
  (ltoken_isValid (t) && (t)->defined) 

extern void ltoken_setDefined (/*@sef@*/ ltoken p_tok, bool p_def);
# define ltoken_setDefined(t, def) \
  (ltoken_isValid (t) ? (t)->defined = (def) : (def))

extern ltoken 
  ltoken_createType (ltokenCode p_code, SimpleIdCode p_idtype, lsymbol p_text) /*@*/ ;

extern ltoken ltoken_create (ltokenCode p_code, lsymbol p_text) /*@*/ ;

extern void ltoken_setIntField (/*@sef@*/ ltoken p_tok, unsigned int p_i);
# define ltoken_setIntField(tok,i) \
  (ltoken_isValid (tok) ? (tok)->intfield = (i) : (i))
     
extern int ltoken_getLine (/*@sef@*/ ltoken p_tok);
# define ltoken_getLine(tok) \
  (ltoken_isValid (tok) ? (tok)->line : 0)

extern void ltoken_setLine (/*@sef@*/ ltoken p_tok, /*@sef@*/ int p_line);
# define ltoken_setLine(tok, ln) \
  (ltoken_isValid (tok) ? (tok)->line = (ln) : 0)

extern int ltoken_getCol (/*@sef@*/ ltoken p_tok);
# define ltoken_getCol(tok) \
  (ltoken_isValid (tok) ? (tok)->col : 0)

extern void ltoken_setCol (/*@sef@*/ ltoken p_tok, int p_col)
   /*@modifies p_tok@*/ ;
# define ltoken_setCol(tok, c) \
  (ltoken_isValid (tok) ? (tok)->col = (c) : (c))

extern ltokenCode ltoken_getCode (/*@sef@*/ ltoken p_tok) /*@*/ ;
# define ltoken_getCode(tok) \
  (ltoken_isValid (tok) ? (tok)->code : NOTTOKEN)

extern unsigned int ltoken_getIntField (/*@sef@*/ ltoken p_tok) /*@*/ ;
# define ltoken_getIntField(tok) \
  (ltoken_isValid (tok) ? (tok)->intfield : 0)

extern lsymbol ltoken_getText (/*@sef@*/ ltoken p_tok) /*@*/;
# define ltoken_getText(tok) \
  (ltoken_isValid (tok) ? (tok)->text : lsymbol_undefined)

extern /*@exposed@*/ char *ltoken_getTextChars (/*@sef@*/ ltoken p_tok) /*@*/ ;
# define ltoken_getTextChars(tok) \
  (lsymbol_toCharsSafe (ltoken_getText (tok)))

extern bool ltoken_hasSyn (/*@sef@*/ ltoken p_tok) /*@*/ ;
# define ltoken_hasSyn(tok) \
  (ltoken_isValid (tok) ? (tok)->hasSyn : FALSE)


extern bool ltoken_wasSyn (/*@sef@*/ ltoken p_tok);
# define ltoken_wasSyn(tok) \
  (ltoken_isValid (tok) ? lsymbol_isDefined ((tok)->rawText) : FALSE)

/*@-namechecks@*/ /* all of these should start with g_ */
extern /*@dependent@*/ ltoken ltoken_forall;
extern /*@dependent@*/ ltoken ltoken_exists;
extern /*@dependent@*/ ltoken ltoken_true;
extern /*@dependent@*/ ltoken ltoken_false;
extern /*@dependent@*/ ltoken ltoken_not;
extern /*@dependent@*/ ltoken ltoken_and;
extern /*@dependent@*/ ltoken ltoken_or;
extern /*@dependent@*/ ltoken ltoken_implies;
extern /*@dependent@*/ ltoken ltoken_eq;
extern /*@dependent@*/ ltoken ltoken_neq;
extern /*@dependent@*/ ltoken ltoken_equals;
extern /*@dependent@*/ ltoken ltoken_eqsep;
extern /*@dependent@*/ ltoken ltoken_select;
extern /*@dependent@*/ ltoken ltoken_open;
extern /*@dependent@*/ ltoken ltoken_sep;
extern /*@dependent@*/ ltoken ltoken_close;
extern /*@dependent@*/ ltoken ltoken_id;
extern /*@dependent@*/ ltoken ltoken_arrow;
extern /*@dependent@*/ ltoken ltoken_marker;
extern /*@dependent@*/ ltoken ltoken_pre;
extern /*@dependent@*/ ltoken ltoken_post;
extern /*@dependent@*/ ltoken ltoken_comment;
extern /*@dependent@*/ ltoken ltoken_compose; 
extern /*@dependent@*/ ltoken ltoken_if;
extern /*@dependent@*/ ltoken ltoken_any;
extern /*@dependent@*/ ltoken ltoken_result;
extern /*@dependent@*/ ltoken ltoken_typename;
extern /*@dependent@*/ ltoken ltoken_bool;
extern /*@dependent@*/ ltoken ltoken_farrow;
extern /*@dependent@*/ ltoken ltoken_lbracked;
extern /*@dependent@*/ ltoken ltoken_rbracket;
/*@=namechecks@*/

extern cstring ltoken_unparseCodeName (ltoken p_tok) /*@*/ ;

extern /*@observer@*/ cstring ltoken_unparse (ltoken p_s);

extern void ltoken_setCode (/*@sef@*/ ltoken p_s, ltokenCode p_code);
# define ltoken_setCode(s,c) (ltoken_isValid (s) ? (s)->code = (c) : (c))

extern void ltoken_setRawText (/*@sef@*/ ltoken p_s, lsymbol p_t);
# define ltoken_setRawText(s,t) (ltoken_isValid (s) ? (s)->rawText = (t) : (t))

extern void ltoken_setIdType (/*@sef@*/ ltoken p_s, SimpleIdCode p_idtype);
# define ltoken_setIdType(s,i) (ltoken_isValid (s) ? (s)->idtype = (i) : (i))

extern void ltoken_setText (/*@sef@*/ ltoken p_s, lsymbol p_text);
# define ltoken_setText(s,t) (ltoken_isValid (s) ? (s)->text = (t) : (t))

extern lsymbol ltoken_getRawText (ltoken) /*@*/ ;   

/* defined in abstract.c */
extern bool ltoken_similar (ltoken p_t1, ltoken p_t2) /*@*/ ;

extern /*@observer@*/ char *ltoken_getRawTextChars (ltoken p_s) /*@*/ ;  
# define ltoken_getRawTextChars(s) \
  (lsymbol_toCharsSafe (ltoken_getRawText (s)))

extern /*@observer@*/ cstring ltoken_getRawString (ltoken p_s) /*@*/ ;
# define ltoken_getRawString(s) \
  (lsymbol_toString (ltoken_getRawText (s)))

extern ltoken ltoken_copy (ltoken) /*@*/ ;

extern /*@observer@*/ cstring ltoken_fileName (/*@sef@*/ ltoken p_s);
# define ltoken_fileName(s) \
  (ltoken_isValid(s) ? lsymbol_toString ((s)->fname) : cstring_undefined)

extern void ltoken_setFileName (/*@sef@*/ ltoken p_tok, /*@sef@*/ cstring p_fname);
# define ltoken_setFileName(tok,f) \
  (ltoken_isValid(tok) ? (tok)->fname = lsymbol_fromString (f) : lsymbol_undefined)

extern bool ltoken_isChar (ltoken p_tok);
# define ltoken_isChar(t) \
  (cstring_length (ltoken_unparse (t)) == 1)

extern void ltoken_setHasSyn (/*@sef@*/ ltoken p_tok, bool p_def);
# define ltoken_setHasSyn(tok, def) \
  (ltoken_isValid(tok) ? (tok)->hasSyn = (def) : (def))

extern void ltoken_free (/*@only@*/ ltoken);

extern ltoken ltoken_createFull (ltokenCode p_code, lsymbol p_text, 
				 cstring p_file, int p_line, int p_col) /*@*/ ;

extern ltoken ltoken_createRaw (ltokenCode p_code, lsymbol p_text) /*@*/ ;
extern cstring ltoken_unparseLoc (ltoken p_t) /*@*/ ;

extern void ltoken_markOwned (/*@owned@*/ ltoken);
extern bool ltoken_isSingleChar (char) /*@*/ ;

# else
# error "Multiple include"
# endif





