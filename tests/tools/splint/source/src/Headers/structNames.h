# ifndef STRUCTNAMES
# define STRUCTNAMES

extern /*@only@*/ cstring makeStruct(cstring p_s);
extern /*@only@*/ cstring makeUnion(cstring p_s);
extern /*@only@*/ cstring makeEnum(cstring p_s);
extern /*@only@*/ cstring makeParam (cstring p_s);

extern void setTagNo (unsigned int p_n);
extern bool isFakeTag (cstring p_s) /*@*/ ;

extern /*@only@*/ cstring fakeTag (void);
extern /*@only@*/ cstring fixTagName (cstring p_s);
extern /*@observer@*/ cstring fixParamName (cstring p_s);
extern /*@observer@*/ cstring plainTagName (cstring p_s) /*@*/ ;

# else
# error "Multiple include"
# endif
