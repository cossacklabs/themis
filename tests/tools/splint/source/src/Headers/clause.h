
typedef enum {
  NOCLAUSE,
  TRUECLAUSE,
  FALSECLAUSE,
  ANDCLAUSE,
  ORCLAUSE,
  WHILECLAUSE,
  DOWHILECLAUSE,
  FORCLAUSE,
  CASECLAUSE,
  SWITCHCLAUSE,
  CONDCLAUSE,
  ITERCLAUSE,
  TRUEEXITCLAUSE,
  FALSEEXITCLAUSE
} clause;

extern /*@observer@*/ cstring clause_nameAlternate (clause p_cl) /*@*/ ;
extern /*@observer@*/ cstring clause_nameTaken (clause p_cl) /*@*/ ;
extern /*@observer@*/ cstring clause_nameFlip (clause p_cl, bool p_flip) /*@*/ ;

extern bool clause_isConditional (clause p_cl) /*@*/ ;
extern bool clause_isBreakable (clause p_cl) /*@*/ ;
extern bool clause_isLoop (clause p_cl) /*@*/ ;
extern bool clause_isSwitch (clause p_cl) /*@*/ ;
extern bool clause_isCase (clause p_cl) /*@*/ ;
extern bool clause_isNone (clause p_cl) /*@*/ ;

extern /*@observer@*/ cstring clause_unparse (clause p_cl) /*@*/ ;
