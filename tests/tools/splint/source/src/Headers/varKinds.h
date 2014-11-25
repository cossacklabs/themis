/*
** varKinds.h
*/

# ifndef VARKINDSH
# define VARKINDSH

/*
** states of storage
*/

typedef enum { 
  SS_UNKNOWN, 
  SS_UNUSEABLE, /* cannot use (unallocated pointer reference) */
  SS_UNDEFINED, /* cannot use value (e.g., unbound variable) */
  SS_MUNDEFINED,/* maybe undefined */
  SS_ALLOCATED, /* allocated pointer */
  SS_PDEFINED,  /* partially defined: must check reachable values */
  SS_DEFINED,   /* completely defined: can use value and all reachable values */
  SS_PARTIAL,   /* defined, but can be bashed without error (partial structure returned) */
  SS_DEAD,      /* cannot use (after giving aways as only) 
		 * same meaning as UNUSEABLE, except that we 
		 * want to generate better error messages. */
  SS_HOFFA,     /* "probably dead" */
  SS_FIXED,     /* cannot modify */
  SS_RELDEF,    /* a structure field that may or may not be defined (up to
                 * programmer to worry about it. */

  SS_UNDEFGLOB,   /* global only: undefined before call */ 
  SS_KILLED,      /* global only: undefined after call */
  SS_UNDEFKILLED, /* global only: undefined before and after call */

  SS_SPECIAL,     /* marker for uses/defines/allocates/releases-specified */

  SS_LAST
} sstate;

extern bool sstate_isValid (/*@sef@*/ sstate) /*@*/ ;
# define sstate_isValid(s) ((s) >= SS_UNKNOWN && (s) <= SS_LAST)

typedef enum { 
  SCNONE, 
  SCEXTERN, 
  SCSTATIC 
} storageClassCode ;

typedef enum {
  NS_ERROR = -1,     /* error (suppress messages) */
  NS_UNKNOWN = 0,    /* no annotation */
  NS_NOTNULL,        /* definitely not null */
  NS_MNOTNULL,       /* marked with notnull */

                     /* perhaps null */
  NS_RELNULL,        /* relaxed null --- okay null for set, not null for checking */
  NS_CONSTNULL,      /* null constant (for abstract types) */
  
  /* don't change the order! */             /* possibly null */
  NS_POSNULL,        /* possibly null */
  NS_DEFNULL,        /* definitely null */
  NS_ABSNULL         /* null from abstract type definition (concrete type unknown) */
} nstate;

extern bool nstate_isKnown (nstate p_n) /*@*/ ;
# define nstate_isKnown(n) ((n) != NS_UNKNOWN)

extern bool nstate_isValid (/*@sef@*/ nstate p_n) /*@*/ ;
# define nstate_isValid(n) ((n) >= NS_ERROR && (n) <= NS_ABSNULL)

/*
** aliasing states
*/

/*
** HEY STUPID!  Don't switch around the order!
**
** These numbers are fixed into the library format.  If the order
** changes, remember to remake all libraries!
*/

typedef enum { 
  AK_UNKNOWN = 0, /* AK_UNKNOWN must be first */
  AK_ERROR,
  AK_ONLY,        
  AK_IMPONLY,
  AK_KEEP,
  AK_KEPT,
  AK_TEMP,  /* 6 */
  AK_IMPTEMP,
  AK_SHARED, 
  AK_UNIQUE,
  AK_RETURNED, /* 10 */
  AK_FRESH,        /* local only storage (may be shared) */
  AK_STACK,        /* allocated on local stack */
  AK_REFCOUNTED,
  AK_REFS,
  AK_KILLREF,
  AK_NEWREF,
  AK_OWNED,
  AK_DEPENDENT, /* 18 */
  AK_IMPDEPENDENT,
  AK_STATIC,
  AK_LOCAL        /* AK_LOCAL must be last */
} alkind;

typedef enum {
  XO_UNKNOWN,
  XO_NORMAL,
  XO_EXPOSED,
  XO_OBSERVER
} exkind;

extern bool sstate_isKnown (sstate p_s) /*@*/ ;
# define sstate_isKnown(s)      ((s) != SS_UNKNOWN)

extern bool sstate_isUnknown (sstate p_s) /*@*/ ;
# define sstate_isUnknown(s)      ((s) == SS_UNKNOWN)

extern bool exkind_isUnknown (exkind p_e) /*@*/ ;
# define exkind_isUnknown(e)    ((e) == XO_UNKNOWN)

extern bool exkind_isKnown (/*@sef@*/ exkind p_e) /*@*/ ;
# define exkind_isKnown(e)      ((e) != XO_UNKNOWN && (e) != XO_NORMAL)

extern bool exkind_isValid (/*@sef@*/ exkind p_e) /*@*/ ;
# define exkind_isValid(e)      ((e) >= XO_UNKNOWN && (e) <= XO_OBSERVER)

extern bool alkind_isValid (/*@sef@*/ alkind p_a) /*@*/ ;
# define alkind_isValid(a) ((a) >= AK_UNKNOWN && (a) <= AK_LOCAL)

extern bool alkind_isImplicit (/*@sef@*/ alkind p_a) /*@*/ ;
extern bool alkind_isDependent (/*@sef@*/ alkind p_a) /*@*/ ; 
extern bool alkind_isOnly (/*@sef@*/ alkind p_a) /*@*/ ;       
extern bool alkind_isTemp (/*@sef@*/ alkind p_a) /*@*/ ;       

# define alkind_isImplicit(a)   (((a) == AK_IMPONLY || (a) == AK_IMPDEPENDENT \
				 || (a) == AK_IMPTEMP))
# define alkind_isDependent(a)  (((a) == AK_DEPENDENT || (a) == AK_IMPDEPENDENT))
# define alkind_isOnly(a)       ((a) == AK_ONLY || (a) == AK_IMPONLY)
# define alkind_isTemp(a)       (((a) == AK_TEMP || (a) == AK_IMPTEMP))

extern bool alkind_equal (alkind p_a1, alkind p_a2) /*@*/ ;

extern bool alkind_isOwned (alkind p_a) /*@*/ ;      
extern bool alkind_isStack (alkind p_a) /*@*/ ;      
extern bool alkind_isStatic (alkind p_a) /*@*/ ;     
extern bool alkind_isKeep (alkind p_a) /*@*/ ;       
extern bool alkind_isKept (alkind p_a) /*@*/ ;       
extern bool alkind_isUnique (alkind p_a) /*@*/ ;     
extern bool alkind_isError (alkind p_a) /*@*/ ;      
extern bool alkind_isFresh (alkind p_a) /*@*/ ;      
extern bool alkind_isShared (alkind p_a) /*@*/ ;     
extern bool alkind_isLocal (alkind p_a) /*@*/ ;      
extern bool alkind_isKnown (alkind p_a) /*@*/ ;      
extern bool alkind_isUnknown (alkind p_a) /*@*/ ;    
extern bool alkind_isRefCounted (alkind p_a) /*@*/ ; 
extern /*@unused@*/ bool alkind_isRefs (alkind p_a) /*@*/ ;       
extern bool alkind_isNewRef (alkind p_a) /*@*/ ;     
extern bool alkind_isKillRef (alkind p_a) /*@*/ ;    

extern alkind alkind_resolve (alkind p_a1, alkind p_a2) /*@*/ ;

# define alkind_isOwned(a)      ((a) == AK_OWNED)
# define alkind_isStack(a)      ((a) == AK_STACK)
# define alkind_isStatic(a)     ((a) == AK_STATIC)
# define alkind_isKeep(a)       ((a) == AK_KEEP)
# define alkind_isKept(a)       ((a) == AK_KEPT)
# define alkind_isUnique(a)     ((a) == AK_UNIQUE)
# define alkind_isError(a)      ((a) == AK_ERROR)
# define alkind_isFresh(a)      ((a) == AK_FRESH)
# define alkind_isShared(a)     ((a) == AK_SHARED)
# define alkind_isLocal(a)      ((a) == AK_LOCAL)
# define alkind_isKnown(a)      ((a) != AK_UNKNOWN)
# define alkind_isUnknown(a)    ((a) == AK_UNKNOWN)
# define alkind_isRefCounted(a) ((a) == AK_REFCOUNTED)
# define alkind_isRefs(a)       ((a) == AK_REFS)
# define alkind_isNewRef(a)     ((a) == AK_NEWREF)
# define alkind_isKillRef(a)    ((a) == AK_KILLREF)

extern /*@observer@*/ cstring sstate_unparse (sstate p_s) /*@*/ ;

extern alkind alkind_fromQual (qual p_q) /*@*/ ;
extern alkind alkind_derive (alkind p_outer, alkind p_inner) /*@*/ ;
extern /*@observer@*/ cstring alkind_unparse (alkind p_a) /*@*/ ;
extern /*@observer@*/ cstring alkind_capName (alkind p_a) /*@*/ ;
extern alkind alkind_fromInt (int p_n) /*@*/ ;
extern nstate nstate_fromInt (int p_n) /*@*/ ;
extern /*@observer@*/ cstring nstate_unparse (nstate p_n) /*@*/ ;
extern int nstate_compare (nstate p_n1, nstate p_n2) /*@*/ ;
extern bool nstate_possiblyNull (nstate p_n) /*@*/ ;
extern bool nstate_perhapsNull (nstate p_n) /*@*/ ;
extern sstate sstate_fromInt (int p_n) /*@*/ ;
extern exkind exkind_fromInt (int p_n) /*@*/ ;
extern exkind exkind_fromQual (qual p_q) /*@*/ ;
extern /*@observer@*/ cstring exkind_unparse (exkind p_a) /*@*/ ;
extern /*@observer@*/ cstring exkind_capName (exkind p_a) /*@*/ ;
extern /*@observer@*/ cstring exkind_unparseError (exkind p_a) /*@*/ ;
extern sstate sstate_fromQual (qual p_q) /*@*/ ;
extern bool alkind_compatible (alkind p_a1, alkind p_a2) /*@*/ ;
extern alkind alkind_fixImplicit (alkind p_a) /*@*/ ;

typedef enum 
{
  XK_ERROR,
  XK_UNKNOWN,      
  XK_NEVERESCAPE,
  XK_GOTO,
  XK_MAYGOTO,
  XK_MAYEXIT,
  XK_MUSTEXIT,
  XK_TRUEEXIT,
  XK_FALSEEXIT,
  XK_MUSTRETURN,
  XK_MAYRETURN,
  XK_MAYRETURNEXIT,
  XK_MUSTRETURNEXIT /* must return or exit */
} exitkind;

/*@constant exitkind XK_LAST; @*/
# define XK_LAST XK_MUSTRETURNEXIT

extern exitkind exitkind_fromQual (qual p_q) /*@*/ ;
extern /*@unused@*/ bool exitkind_isMustExit (exitkind p_e) /*@*/ ;
# define exitkind_isMustExit(e) ((e) == XK_MUSTEXIT)

extern bool exitkind_equal (exitkind p_e1, exitkind p_e2) /*@*/ ;
# define exitkind_equal(e1,e2) ((e1) == (e2))

extern bool exitkind_couldExit (exitkind p_e) /*@*/ ;
extern bool exitkind_couldEscape (exitkind p_e) /*@*/ ;
extern exitkind exitkind_fromInt (int p_x) /*@*/ ;
extern /*@observer@*/ cstring exitkind_unparse (exitkind p_k) /*@*/ ;

extern bool exitkind_isKnown (exitkind p_e) /*@*/ ;
# define exitkind_isKnown(e) ((e) != XK_UNKNOWN)

extern bool exitkind_isTrueExit (exitkind p_e) /*@*/ ;
# define exitkind_isTrueExit(e) \
  ((e) == XK_TRUEEXIT)

extern bool exitkind_isConditionalExit (/*@sef@*/ exitkind p_e) /*@*/ ;
# define exitkind_isConditionalExit(e) \
  ((e) == XK_TRUEEXIT || (e) == XK_FALSEEXIT)

extern bool exitkind_isError (/*@sef@*/ exitkind p_e) /*@*/ ;
# define exitkind_isError(e) ((e) == XK_ERROR)

extern bool exitkind_mustExit (/*@sef@*/ exitkind p_e) /*@*/ ;
# define exitkind_mustExit(e) ((e) == XK_MUSTEXIT)

extern bool exitkind_mustEscape (/*@sef@*/ exitkind p_e) /*@*/ ;
# define exitkind_mustEscape(e) \
  ((e) == XK_MUSTEXIT || (e) == XK_MUSTRETURN \
   || (e) == XK_MUSTRETURNEXIT || (e) == XK_GOTO)

extern exitkind exitkind_makeConditional (exitkind p_k) /*@*/ ;
extern exitkind exitkind_combine (exitkind p_k1, exitkind p_k2) /*@*/ ;

/*
** NOTE: combiners are in sRef
*/

# else
# error "Multiple include"
# endif
