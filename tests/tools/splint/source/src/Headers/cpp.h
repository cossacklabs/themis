# ifndef CPP_H
# define CPP_H

typedef struct cppBuffer cppBuffer;
typedef struct cppOptions cppOptions;

enum cpp_token {
  CPP_EOF = -1,
  CPP_OTHER = 0,
  CPP_COMMENT = 1,
  CPP_HSPACE,
  CPP_VSPACE, /* newlines and #line directives */
  CPP_NAME,
  CPP_NUMBER,
  CPP_CHAR,
  CPP_STRING,
  CPP_DIRECTIVE,
  CPP_LPAREN,   /* "(" */
  CPP_RPAREN,   /* ")" */
  CPP_LBRACE,   /* "{" */
  CPP_RBRACE,   /* "}" */
  CPP_COMMA,    /* "," */
  CPP_SEMICOLON,/* ";" */
  CPP_3DOTS,    /* "..." */
#if 0
  CPP_ANDAND, /* "&&" */
  CPP_OROR,   /* "||" */
  CPP_LSH,    /* "<<" */
  CPP_RSH,    /* ">>" */
  CPP_EQL,    /* "==" */
  CPP_NEQ,    /* "!=" */
  CPP_LEQ,    /* "<=" */
  CPP_GEQ,    /* ">=" */
  CPP_PLPL,   /* "++" */
  CPP_MINMIN, /* "--" */
#endif
  /* POP_TOKEN is returned when we've popped a cppBuffer. */
  CPP_POP
};

typedef struct cppReader cppReader;
extern cppReader g_cppState;

extern /*@only@*/ cstring cppReader_getIncludePath (void) ;

extern int cppProcess (/*@dependent@*/ cstring p_infile,
		       /*@dependent@*/ cstring p_outfile);
extern void cppAddIncludeDir (cstring);
extern void cppReader_initMod (void) /*@modifies internalState@*/ ;
extern void cppReader_destroyMod (void) /*@modifies internalState@*/ ;
extern void cppDoDefine (cstring);
extern void cppDoUndefine (cstring);
extern void cppReader_saveDefinitions (void);

extern fileloc cppReader_getLoc (cppReader *);
extern void cppReader_initialize (void);

# else 
# error "Multiple include"
# endif
