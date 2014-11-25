/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*       
** lctype.h
**
** renamed from ctype.h to avoid confusion with standard
** header file
*/      
        
# ifndef CTYPE_H
# define CTYPE_H

/*
** since this is the header file for the ctype module, we need
** to use an access comment to provide access to the representation.
*/

/* in forwardTypes: typedef int ctype; */
       
typedef enum 
{ 
  CT_UNKNOWN, 
  CT_PRIM, 
  CT_USER, 
  CT_ABST,
  CT_NUMABST, 
  CT_ENUM, 
  CT_PTR, 
  CT_ARRAY, 
  CT_FIXEDARRAY, 
  CT_FCN, 
  CT_STRUCT, 
  CT_UNION,
  CT_ENUMLIST, /* bogus type for keeping unnamed enums around */
  CT_BOOL,     /* since booleans aren't really prim's but we don't
		  want them to be user or abst, they are a separate type */
  CT_CONJ,     /* conjunction */
  CT_EXPFCN
  } ctuid ;

/*@access ctype@*/

/* hack alert! */

/*@constant int CTK_ELIPS;@*/          
# define CTK_ELIPS -23         

/*@constant int CTK_MISSINGPARAMS;@*/          
# define CTK_MISSINGPARAMS -37

/*@constant int CT_FIRST;@*/          
# define CT_FIRST 0

/*@constant int CTK_PREDEFINED;@*/       
# define CTK_PREDEFINED   (CTX_LAST + 1)

/*@constant int CTK_PREDEFINED2;@*/       
# define CTK_PREDEFINED2  (CTK_PREDEFINED + CTK_PREDEFINED)

/*@constant int LAST_PREDEFINED;@*/       
# define LAST_PREDEFINED (CTK_PREDEFINED + CTK_PREDEFINED2)

/*@constant int CTP_VOID;@*/       
# define CTP_VOID (CTX_VOID + CTK_PREDEFINED)

/*@constant int CTP_CHAR;@*/       
# define CTP_CHAR (CTX_CHAR + CTK_PREDEFINED)
         
/*@constant int CTK_BASESIZE;@*/                
# define CTK_BASESIZE BIGBASESIZE
       
typedef enum 
{ 
  CTK_UNKNOWN = -3,
  CTK_INVALID = -2, 
  CTK_DNE = -1,
  CTK_PLAIN = 0, /* these must be in order */
  CTK_PTR = 1,
  CTK_ARRAY = 2,
  CTK_COMPLEX = 3 
  } ctkind ;

extern int ctkind_toInt (ctkind p_x);
# define ctkind_toInt(x) ((int)(x))

/*
** ctypes are integer indexes into ctype table
**
** the most common ctypes are hard wired (see cprim.h)
*/

/*@constant ctype ctype_undefined; @*/
# define ctype_undefined  ((ctype) CTK_INVALID)

/*@constant ctype ctype_dne; @*/
# define ctype_dne        ((ctype) CTK_DNE)

/*@constant ctype ctype_unknown; @*/
# define ctype_unknown    CTX_UNKNOWN

/*@constant ctype ctype_void; @*/
# define ctype_void       CTX_VOID

/*@constant ctype ctype_char; @*/
# define ctype_char       CTX_CHAR

/*@constant ctype ctype_uchar; @*/
# define ctype_uchar      CTX_UCHAR

/*@constant ctype ctype_double; @*/
# define ctype_double     CTX_DOUBLE

/*@constant ctype ctype_ldouble; @*/
# define ctype_ldouble    CTX_LDOUBLE

/*@constant ctype ctype_float; @*/
# define ctype_float      CTX_FLOAT

/*@constant ctype ctype_int; @*/
# define ctype_int        CTX_INT

/*@constant ctype ctype_uint; @*/
# define ctype_uint       CTX_UINT

/*@constant ctype ctype_sint; @*/
# define ctype_sint       CTX_SINT

/*@constant ctype ctype_lint; @*/
# define ctype_lint       CTX_LINT

/*@constant ctype ctype_usint; @*/
# define ctype_usint      CTX_USINT

/*@constant ctype ctype_ulint; @*/
# define ctype_ulint      CTX_ULINT

/*@constant ctype ctype_llint; @*/
# define ctype_llint      CTX_LLINT

/*@constant ctype ctype_ullint; @*/
# define ctype_ullint     CTX_ULLINT

/*@constant ctype ctype_bool; @*/
# define ctype_bool       CTX_BOOL

/*@constant ctype ctype_string; @*/
# define ctype_string     CTP_CHAR

/*@constant ctype ctype_anyintegral @*/
# define ctype_anyintegral CTX_ANYINTEGRAL

/*@constant ctype ctype_unsignedintegral @*/
# define ctype_unsignedintegral CTX_UNSIGNEDINTEGRAL

/*@constant ctype ctype_signedintegral @*/
# define ctype_signedintegral CTX_SIGNEDINTEGRAL

/*@constant ctype ctype_voidPointer; @*/
# define ctype_voidPointer CTP_VOID

extern ctype ctype_forceRealType (ctype p_c);       
extern bool ctype_forceMatch (ctype p_c1, ctype p_c2);
extern bool ctype_genMatch (ctype p_c1, ctype p_c2, bool p_force, bool p_arg, bool p_def, bool p_deep);

extern bool ctype_isSimple (ctype p_c) /*@*/ ;
extern bool ctype_isAbstract (ctype p_c) /*@*/ ;
extern bool ctype_isNumAbstract (ctype p_c) /*@*/ ;
extern bool ctype_isArray (ctype p_c) /*@*/ ;
extern bool ctype_isFixedArray (ctype p_c) /*@*/ ;
extern bool ctype_isIncompleteArray (ctype p_c) /*@*/ ;
extern bool ctype_isArrayPtr (ctype p_c) /*@*/ ;
extern bool ctype_isBool (ctype p_c) /*@*/ ;  /* matches type bool */
extern bool ctype_isManifestBool (ctype p_c) /*@*/ ; /* declared as type bool */
extern bool ctype_isChar (ctype p_c) /*@*/ ;
extern bool ctype_isUnsignedChar (ctype p_c) /*@*/ ;
extern bool ctype_isSignedChar (ctype p_c) /*@*/ ;
extern bool ctype_isString (ctype p_c) /*@*/ ;
extern bool ctype_isConj (ctype p_c) /*@*/ ;
extern bool ctype_isDirectBool (ctype p_c) /*@*/ ;
extern bool ctype_isDirectInt(ctype p_c) /*@*/ ;
extern bool ctype_isEnum(ctype p_c) /*@*/ ;
extern bool ctype_isExpFcn (ctype p_c) /*@*/ ;
extern bool ctype_isFirstVoid (ctype p_c) /*@*/ ;
extern bool ctype_isForceRealBool (ctype *p_c) /*@*/ ;
extern bool ctype_isForceRealInt (ctype *p_c) /*@*/ ;
extern bool ctype_isForceRealNumeric (ctype *p_c) /*@*/ ;
extern bool ctype_isFunction (ctype p_c) /*@*/ ;
extern bool ctype_isArbitraryIntegral (ctype p_c) /*@*/ ;
extern bool ctype_isUnsignedIntegral (ctype p_c) /*@*/ ;
extern bool ctype_isSignedIntegral (ctype p_c) /*@*/ ;
extern bool ctype_isInt (ctype p_c) /*@*/ ;
extern bool ctype_isRegularInt (ctype p_c) /*@*/ ;
extern bool ctype_isMutable (ctype p_t) /*@*/ ;
extern bool ctype_isImmutableAbstract (ctype p_t) /*@*/ ;
extern bool ctype_isNumeric (ctype p_c) /*@*/ ;
extern bool ctype_isPointer (ctype p_c) /*@*/ ;
extern bool ctype_isReal (ctype p_c) /*@*/ ;
extern /*@unused@*/ bool ctype_isFloat (ctype p_c) /*@*/ ;
extern bool ctype_isDouble (ctype p_c) /*@*/ ;
extern bool ctype_isSigned (ctype p_c) /*@*/ ;
extern bool ctype_isUnsigned (ctype p_c) /*@*/ ;
extern bool ctype_isRealAP (ctype p_c) /*@*/ ;
extern bool ctype_isRealAbstract(ctype p_c) /*@*/ ;
extern bool ctype_isRealNumAbstract(ctype p_c) /*@*/ ;
extern bool ctype_isRealArray (ctype p_c) /*@*/ ;
extern bool ctype_isRealBool (ctype p_c) /*@*/ ;
extern bool ctype_isRealFunction (ctype p_c) /*@*/ ;
extern bool ctype_isRealInt (ctype p_c) /*@*/ ;
extern bool ctype_isRealNumeric (ctype p_c) /*@*/ ;
extern bool ctype_isRealPointer (ctype p_c) /*@*/ ;
extern bool ctype_isRealSU (ctype p_c) /*@*/ ;
extern bool ctype_isRealVoid (ctype p_c) /*@*/ ;
extern bool ctype_isStruct (ctype p_c) /*@*/ ;
extern bool ctype_isStructorUnion(ctype p_c) /*@*/ ;
extern bool ctype_isUA (ctype p_c) /*@*/ ;
extern bool ctype_isUnion (ctype p_c) /*@*/ ;
extern bool ctype_isVoid (ctype p_c) /*@*/ ;
extern bool ctype_isVoidPointer (ctype p_c) /*@*/ ;

extern bool ctype_isVisiblySharable (ctype p_t) /*@*/ ;
extern bool ctype_match (ctype p_c1, ctype p_c2) /*@*/ ;
extern bool ctype_matchArg (ctype p_c1, ctype p_c2) /*@*/ ;
extern bool ctype_sameName (ctype p_c1, ctype p_c2) /*@*/ ;         

extern /*@only@*/ cstring ctype_dump (ctype p_c) /*@*/ ;
extern /*@observer@*/ cstring ctype_enumTag (ctype p_c) /*@*/ ;           
extern /*@observer@*/ cstring ctype_unparse (ctype p_c) /*@*/ ;
extern /*@observer@*/ cstring ctype_unparseDeep (ctype p_c) /*@*/ ;       
extern /*@unused@*/ /*@observer@*/ cstring ctype_unparseSafe (ctype p_c) /*@*/ ;

extern ctkind ctkind_fromInt (int p_i) /*@*/ ;

extern bool ctype_matchDef (ctype p_c1, ctype p_c2) /*@*/ ;
extern ctype ctype_undump (char **p_c);
extern ctype ctype_adjustPointers (pointers p_p, ctype p_c);
extern ctype ctype_baseArrayPtr (ctype p_c) /*@*/ ;
extern ctype ctype_combine (ctype p_dominant, ctype p_modifier) ;

extern ctype ctype_createAbstract (typeId p_u)  /*@*/ ;
extern ctype ctype_createNumAbstract (typeId p_u)  /*@*/ ;

extern ctype ctype_createForwardStruct (/*@only@*/ cstring p_n)  /*@*/ ;
extern ctype ctype_createForwardUnion (/*@only@*/ cstring p_n)  /*@*/ ;
extern ctype ctype_createForwardEnum (/*@only@*/ cstring p_n) /*@*/ ;

extern ctype ctype_createStruct (/*@only@*/ cstring p_n, /*@only@*/ uentryList p_f);
extern ctype ctype_createUnion (/*@only@*/ cstring p_n, /*@only@*/ uentryList p_f);
extern ctype ctype_createEnum (/*@keep@*/ cstring p_tag, /*@keep@*/ enumNameList p_el);

extern ctype ctype_createUnnamedStruct (/*@only@*/ uentryList p_f) ;
extern ctype ctype_createUnnamedUnion (/*@only@*/ uentryList p_f) ;
extern ctype ctype_createUser (typeId p_u) ;

extern bool ctype_isUnnamedSU (ctype p_c) /*@*/ ;
extern bool ctype_isUser (ctype p_c) /*@*/ ;

extern int ctype_getSize (ctype p_c) 
  /* EFFECTS: Returns the expected size of type p_c.  Various flags to control? 
  **          Returns -1 if the size is unknown (or should not be guessed). 
  */
 /*@*/ ;

extern ctype ctype_biggerType (ctype p_c1, ctype p_c2) 
  /* EFFECTS: returns whichever of c1 or c2 is bigger (storage requirements).
        If they are equal, returns c1. */
  /*@*/ ;

extern ctype ctype_expectFunction(ctype p_c) ;
extern ctype ctype_dontExpectFunction (ctype p_c) ;

extern ctype ctype_fixArrayPtr (ctype p_c);

extern ctype ctype_getBaseType (ctype p_c) /*@*/ ;

extern ctype ctype_makeAnytype (void) /*@*/ ;
extern bool ctype_isAnytype (ctype) /*@*/ ;

extern ctype ctype_makeArray (ctype p_c);
extern ctype ctype_makeFixedArray (ctype p_c, size_t p_size);
extern ctype ctype_makeInnerFixedArray (ctype p_c, size_t p_size);
extern ctype ctype_makeInnerArray (ctype p_c);
extern ctype ctype_makeConj (ctype p_c1, ctype p_c2) /*@modifies internalState@*/ ;
extern ctype ctype_makeParamsFunction (ctype p_base, /*@only@*/ uentryList p_p);
extern ctype ctype_makeFunction (ctype p_base, /*@only@*/ uentryList p_p) /*@*/ ;
extern ctype ctype_makeNFParamsFunction (ctype p_base, /*@only@*/ uentryList p_p) /*@*/ ;
extern ctype ctype_makePointer (ctype p_c);
extern ctype ctype_makeRawFunction (ctype p_base, /*@only@*/ uentryList p_p);
extern ctype ctype_makeWideString (void) /*@modifies internalState@*/ ;
extern bool ctype_isWideString (ctype p_c) /*@*/ ;

extern ctype ctype_newBase (ctype p_c, ctype p_p) /*@*/ ;
extern ctype ctype_realType (ctype p_c) /*@*/ ;
extern ctype ctype_realishType (ctype p_c) /*@*/ ;
extern ctype ctype_removePointers (ctype p_c) /*@*/ ;
extern ctype ctype_resolve (ctype p_c) /*@*/ ;
extern ctype ctype_resolveNumerics (ctype p_c1, ctype p_c2) /*@*/ ;
extern ctype ctype_getReturnType (ctype p_c) /*@*/ ;

extern bool ctype_isRefCounted (ctype p_t) /*@*/ ;
extern /*@observer@*/ uentryList ctype_argsFunction (ctype p_c) /*@*/ ;
extern /*@observer@*/ uentryList ctype_getParams (ctype p_c) /*@*/ ;
# define ctype_getParams(c) ctype_argsFunction(c)

extern /*@observer@*/ enumNameList ctype_elist (ctype p_c) /*@*/ ;
extern /*@observer@*/ uentryList ctype_getFields (ctype p_c) /*@*/ ;

/*
** could do some run-time checks...
*/

extern cprim ctype_toCprim (ctype p_c) /*@*/ ;
/*@access cprim@*/
# define ctype_toCprim(c) ((cprim) (c))
/*@noaccess cprim@*/

extern int ctype_compare (ctype p_c1, ctype p_c2) /*@*/ ;
extern /*@unused@*/ int ctype_count (void);

extern ctype ctype_makeExplicitConj (ctype p_c1, ctype p_c2);

extern typeId ctype_typeId (ctype p_c);

extern ctype ctype_fromQual (qual p_q);
extern bool ctype_isAnyFloat (ctype p_c);
extern bool ctype_isStackAllocated (ctype p_c);

/*@constant ctype ctype_missingParamsMarker; @*/
# define ctype_missingParamsMarker ((ctype)CTK_MISSINGPARAMS)

extern bool ctype_isMissingParamsMarker (ctype p_ct) /*@*/ ;
# define ctype_isMissingParamsMarker(ct) ((ct) == ctype_missingParamsMarker)

extern bool ctype_equal (ctype p_c1, ctype p_c2);
# define ctype_equal(c1,c2) ((c1) == (c2))

extern bool ctype_almostEqual (ctype p_c1, ctype p_c2);

/*@constant ctype ctype_elipsMarker; @*/
# define ctype_elipsMarker ((ctype) CTK_ELIPS)

extern bool ctype_isElips (ctype p_ct) /*@*/;
# define ctype_isElips(ct) ((ct) == ctype_elipsMarker)

extern bool ctype_isAP (ctype p_c) /*@*/ ;
# define ctype_isAP(c) ctype_isArrayPtr(c)       

extern bool ctype_isDefined (ctype p_c) /*@*/ ;
# define ctype_isDefined(c)       ((c) != ctype_undefined)

extern bool ctype_isKnown (ctype p_c) /*@*/ ;
# define ctype_isKnown(c)         ((c) != CTX_UNKNOWN)

extern bool ctype_isSU (ctype p_c) /*@*/ ;
# define ctype_isSU(c)            ctype_isStructorUnion(c)

extern bool ctype_isUndefined (ctype p_c) /*@*/ ;
# define ctype_isUndefined(c)     ((c) == ctype_undefined)

extern bool ctype_isUnknown (ctype p_c) /*@*/;
# define ctype_isUnknown(c)       ((c) == CTX_UNKNOWN)

extern bool ctype_isBogus (/*@sef@*/ ctype p_c) /*@*/ ;
# define ctype_isBogus(c)         (ctype_isUndefined(c) || ctype_isUnknown(c))

extern cstring 
  ctype_unparseDeclaration (ctype p_c, /*@only@*/ cstring p_name);

extern bool ctype_sameAltTypes (ctype p_c1, ctype p_c2) /*@*/ ;

extern void ctype_dumpTable (FILE *p_f);
extern void ctype_loadTable (FILE *p_f);
extern void ctype_destroyMod (void);
extern void ctype_initTable (void);
extern /*@only@*/ cstring ctype_unparseTable (void);
extern /*@unused@*/ void ctype_printTable (void);

extern ctype ctype_widest (ctype, ctype) /*@*/ ;

/* drl modified */
extern size_t ctype_getArraySize (ctype p_c);
/*end drl add functions */

/* Should only be used in uentry.c */
extern bool ctype_isUserBool (ctype p_ct) /*@*/ ;

# else
# error "Multiple include"
# endif
