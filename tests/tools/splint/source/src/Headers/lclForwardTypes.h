# ifndef LCL_FORWARDTYPES_H
# define LCL_FORWARDTYPES_H

typedef struct s_opFormNode *opFormNode;
typedef struct s_termNode *termNode;
typedef struct s_abstBodyNode *abstBodyNode;
typedef struct s_lclPredicateNode *lclPredicateNode;
typedef struct s_stmtNode *stmtNode;
abst_typedef struct s_programNodeList *programNodeList;
typedef /*@null@*/ struct s_lclTypeSpecNode *lclTypeSpecNode;
abst_typedef struct s_termNodeList *termNodeList;
typedef unsigned int sort;
typedef long unsigned lsymbol;
typedef struct s_typeExpr *typeExpr;
abst_typedef /*@null@*/ struct s_ltoken *ltoken;
typedef unsigned int ltokenCode;

# else
# error "Multiple include"
# endif
