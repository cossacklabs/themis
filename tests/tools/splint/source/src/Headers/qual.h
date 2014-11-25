/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** qual.h
**
** representation of type qualifiers
*/

# ifndef QUALH
# define QUALH

/*
** just use tokens to represent type qualifiers 
** (tokens are defined in cgrammar.y) 
*/

typedef enum { 
  QU_UNKNOWN = 0,
  QU_CONST, QU_VOLATILE, QU_RESTRICT,
  QU_INLINE, QU_EXTERN, QU_STATIC, 
  QU_AUTO, QU_REGISTER, 
  QU_SHORT, QU_LONG, QU_SIGNED, QU_UNSIGNED, 
  QU_OUT, QU_IN,
  QU_ONLY, QU_IMPONLY,
  QU_TEMP, QU_SHARED, QU_KEEP, QU_KEPT, QU_PARTIAL, QU_SPECIAL,
  QU_NULL, QU_RELNULL, QU_ISNULL,
  QU_NULLTERMINATED,
  QU_SETBUFFERSIZE,
  QU_EXPOSED, QU_RETURNED, QU_OBSERVER, QU_UNIQUE,
  QU_OWNED, QU_DEPENDENT, QU_RELDEF,
  QU_YIELD, 
  QU_NEVEREXIT, QU_EXITS, QU_MAYEXIT, QU_TRUEEXIT, QU_FALSEEXIT,
  QU_UNUSED, QU_EXTERNAL, QU_SEF,
  QU_NOTNULL,
  QU_ABSTRACT, QU_NUMABSTRACT, QU_CONCRETE, 
  QU_MUTABLE, QU_IMMUTABLE,
  QU_REFCOUNTED, QU_REFS, QU_NEWREF, QU_KILLREF, QU_TEMPREF,
  QU_TRUENULL, QU_FALSENULL, 
  QU_CHECKED, QU_UNCHECKED, QU_CHECKEDSTRICT, QU_CHECKMOD,
  QU_UNDEF, QU_KILLED,
  QU_PRINTFLIKE, QU_SCANFLIKE, QU_MESSAGELIKE,
  QU_USERANNOT,
  QU_LAST
} quenum;

immut_typedef struct {
  quenum kind;
  /*@observer@*/ /*@null@*/ annotationInfo info; /* for QU_USERANNOT only */
} *qual;

extern /*@only@*/ cstring qual_dump (qual) ;
extern qual qual_undump (char **p_s) /*@modifies *p_s@*/ ;


/*
** qualifiers correspond to tokens in the grammar:
**    
**    standard C qualifiers:    
**        QCONST QVOLATILE QINLINE QEXTERN QSTATIC QAUTO QREGISTER
**        QSHORT QLONG QSIGNED QUNSIGNED
**    augmented LCL qualifiers: 
**        QOUT QONLY QTEMP QSHARED QUNIQUE QYIELD
**        QEXITS QMAYEXIT QNULLTERMINATED
*/

extern qual qual_fromInt (int p_q) /*@*/ ;
extern /*@observer@*/ cstring qual_unparse (qual p_q) /*@*/ ;

extern bool qual_match (qual p_q1, qual p_q2) /*@*/ ;

extern bool qual_isUnknown (qual) /*@*/ ;
extern bool qual_isTrueNull (qual) /*@*/ ;
extern bool qual_isFalseNull (qual) /*@*/ ;
extern bool qual_isOwned (qual) /*@*/ ;
extern bool qual_isDependent (qual) /*@*/ ;
extern bool qual_isRefCounted (qual) /*@*/ ;
extern bool qual_isRefs (qual) /*@*/ ;
extern bool qual_isNewRef (qual) /*@*/ ;
extern bool qual_isKillRef (qual) /*@*/ ;
extern bool qual_isTempRef (qual) /*@*/ ;
extern bool qual_isLong (qual) /*@*/ ;
extern bool qual_isShort (qual) /*@*/ ;
extern bool qual_isSigned (qual) /*@*/ ;
extern bool qual_isUnsigned (qual) /*@*/ ;
extern bool qual_isUnique (qual) /*@*/ ;
extern bool qual_isExits (qual) /*@*/ ;
extern bool qual_isMayExit (qual) /*@*/ ;
extern bool qual_isNeverExit (qual) /*@*/ ;
extern bool qual_isTrueExit (qual) /*@*/ ;
extern bool qual_isFalseExit (qual) /*@*/ ;
extern bool qual_isConst (qual) /*@*/ ;
extern bool qual_isRestrict (qual) /*@*/ ;
extern bool qual_isVolatile (qual) /*@*/ ;
extern bool qual_isInline (qual) /*@*/ ;
extern bool qual_isExtern (qual) /*@*/ ;
extern bool qual_isStatic (qual) /*@*/ ;
extern bool qual_isAuto (qual) /*@*/ ;
extern bool qual_isRegister (qual) /*@*/ ;
extern bool qual_isOut (qual) /*@*/ ;
extern bool qual_isIn (qual) /*@*/ ;
extern bool qual_isYield (qual) /*@*/ ;
extern bool qual_isOnly (qual) /*@*/ ;
extern bool qual_isImpOnly (qual) /*@*/ ;
extern bool qual_isPartial (qual) /*@*/ ;
extern bool qual_isSpecial (qual) /*@*/ ;
extern bool qual_isKeep (qual) /*@*/ ;
extern bool qual_isKept (qual) /*@*/ ;
extern bool qual_isTemp (qual) /*@*/ ;
extern bool qual_isShared (qual) /*@*/ ;
extern bool qual_isRelDef (qual) /*@*/ ;
extern bool qual_isChecked (qual) /*@*/ ;
extern bool qual_isCheckMod (qual) /*@*/ ;
extern bool qual_isCheckedStrict (qual) /*@*/ ;
extern bool qual_isUnchecked (qual) /*@*/ ;
extern bool qual_isNull (qual) /*@*/ ;
extern bool qual_isIsNull (qual) /*@*/ ;
extern bool qual_isRelNull (qual) /*@*/ ;
extern bool qual_isNotNull (qual) /*@*/ ;
extern bool qual_isReturned (qual) /*@*/ ;
extern bool qual_isExposed (qual) /*@*/ ;
extern bool qual_isObserver (qual) /*@*/ ;
extern bool qual_isUnused (qual) /*@*/ ;
extern bool qual_isExternal (qual) /*@*/ ;
extern bool qual_isSef (qual) /*@*/ ;
extern bool qual_isAbstract (qual) /*@*/ ;
extern bool qual_isNumAbstract (qual) /*@*/ ;
extern bool qual_isEitherAbstract (/*@sef@*/ qual) /*@*/ ;
extern bool qual_isConcrete (qual) /*@*/ ;
extern bool qual_isMutable (qual) /*@*/ ;
extern bool qual_isImmutable (qual) /*@*/ ;
extern bool qual_isNullPred (/*@sef@*/ qual) /*@*/ ;   
extern bool qual_isRefQual (/*@sef@*/ qual) /*@*/ ;
extern bool qual_isUndef (/*@sef@*/ qual) /*@*/ ;
extern bool qual_isKilled (/*@sef@*/ qual) /*@*/ ;
extern /*@unused@*/ bool qual_isTypeQual (/*@sef@*/ qual) /*@*/ ;     
extern /*@unused@*/ bool qual_isControlQual (/*@sef@*/ qual) /*@*/ ; 
extern /*@unused@*/ bool qual_isStorageClass (/*@sef@*/ qual) /*@*/ ;
extern bool qual_isCQual (/*@sef@*/ qual) /*@*/ ;	
extern bool qual_isAllocQual (/*@sef@*/ qual) /*@*/ ;  
extern bool qual_isGlobalQual (/*@sef@*/ qual) /*@*/ ;  
extern bool qual_isImplied (/*@sef@*/ qual) /*@*/ ;   
extern bool qual_isExQual (/*@sef@*/ qual) /*@*/ ;	
extern bool qual_isAliasQual (/*@sef@*/ qual) /*@*/ ; 
extern bool qual_isPrintfLike (/*@sef@*/ qual) /*@*/ ; 
extern bool qual_isScanfLike (/*@sef@*/ qual) /*@*/ ; 
extern bool qual_isMessageLike (/*@sef@*/ qual) /*@*/ ; 
extern bool qual_isMetaState (/*@sef@*/ qual) /*@*/ ;
extern bool qual_isNullTerminated( /*@sef@*/ qual ) /*@*/ ;

extern bool qual_isMemoryAllocation (/*@sef@*/ qual) /*@*/ ;
# define qual_isMemoryAllocation(q) \
   (qual_isOnly (q) || qual_isShared (q) || \
    qual_isDependent (q) || qual_isOwned (q))

extern bool qual_isSharing (/*@sef@*/ qual) /*@*/ ;
# define qual_isSharing(q) \
   (qual_isObserver (q) || qual_isExposed (q))

extern /*@observer@*/ annotationInfo qual_getAnnotationInfo (qual) /*@*/ ;

extern void qual_initMod (void) /*@modifies internalState@*/ ;
extern void qual_destroyMod (void) /*@modifies internalState@*/ ;

# define qual_isUnknown(q)        ((q)->kind == QU_UNKNOWN)
# define qual_isTrueNull(q)       ((q)->kind == QU_TRUENULL)
# define qual_isFalseNull(q)      ((q)->kind == QU_FALSENULL)
# define qual_isOwned(q)          ((q)->kind == QU_OWNED)
# define qual_isDependent(q)      ((q)->kind == QU_DEPENDENT)
# define qual_isRefCounted(q)     ((q)->kind == QU_REFCOUNTED)
# define qual_isRefs(q)           ((q)->kind == QU_REFS)
# define qual_isNewRef(q)         ((q)->kind == QU_NEWREF)
# define qual_isKillRef(q)        ((q)->kind == QU_KILLREF)
# define qual_isTempRef(q)        ((q)->kind == QU_TEMPREF)
# define qual_isLong(q)           ((q)->kind == QU_LONG)
# define qual_isShort(q)          ((q)->kind == QU_SHORT)
# define qual_isSigned(q)         ((q)->kind == QU_SIGNED)
# define qual_isUnsigned(q)       ((q)->kind == QU_UNSIGNED)
# define qual_isUnique(q)         ((q)->kind == QU_UNIQUE)
# define qual_isExits(q)          ((q)->kind == QU_EXITS)
# define qual_isMayExit(q)        ((q)->kind == QU_MAYEXIT)
# define qual_isNeverExit(q)      ((q)->kind == QU_NEVEREXIT)
# define qual_isTrueExit(q)       ((q)->kind == QU_TRUEEXIT)
# define qual_isFalseExit(q)      ((q)->kind == QU_FALSEEXIT)
# define qual_isConst(q)          ((q)->kind == QU_CONST)
# define qual_isRestrict(q)       ((q)->kind == QU_RESTRICT)
# define qual_isVolatile(q)       ((q)->kind == QU_VOLATILE)
# define qual_isInline(q)         ((q)->kind == QU_INLINE)
# define qual_isExtern(q)         ((q)->kind == QU_EXTERN)
# define qual_isStatic(q)         ((q)->kind == QU_STATIC)
# define qual_isAuto(q)           ((q)->kind == QU_AUTO)
# define qual_isRegister(q)       ((q)->kind == QU_REGISTER)
# define qual_isOut(q)            ((q)->kind == QU_OUT)
# define qual_isIn(q)             ((q)->kind == QU_IN)
# define qual_isYield(q)          ((q)->kind == QU_YIELD)
# define qual_isOnly(q)           ((q)->kind == QU_ONLY)
# define qual_isImpOnly(q)        ((q)->kind == QU_IMPONLY)
# define qual_isPartial(q)        ((q)->kind == QU_PARTIAL)
# define qual_isSpecial(q)        ((q)->kind == QU_SPECIAL)
# define qual_isKeep(q)           ((q)->kind == QU_KEEP)
# define qual_isKept(q)           ((q)->kind == QU_KEPT)
# define qual_isTemp(q)           ((q)->kind == QU_TEMP)
# define qual_isShared(q)         ((q)->kind == QU_SHARED)
# define qual_isRelDef(q)         ((q)->kind == QU_RELDEF)
# define qual_isNull(q)           ((q)->kind == QU_NULL)
# define qual_isIsNull(q)         ((q)->kind == QU_ISNULL)
# define qual_isRelNull(q)        ((q)->kind == QU_RELNULL)
# define qual_isNotNull(q)        ((q)->kind == QU_NOTNULL)
# define qual_isReturned(q)       ((q)->kind == QU_RETURNED)
# define qual_isExposed(q)        ((q)->kind == QU_EXPOSED)
# define qual_isObserver(q)       ((q)->kind == QU_OBSERVER)
# define qual_isUnused(q)         ((q)->kind == QU_UNUSED)
# define qual_isExternal(q)       ((q)->kind == QU_EXTERNAL)
# define qual_isSef(q)            ((q)->kind == QU_SEF)
# define qual_isAbstract(q)       ((q)->kind == QU_ABSTRACT)
# define qual_isNumAbstract(q)    ((q)->kind == QU_NUMABSTRACT)
# define qual_isConcrete(q)       ((q)->kind == QU_CONCRETE)
# define qual_isMutable(q)        ((q)->kind == QU_MUTABLE)
# define qual_isImmutable(q)      ((q)->kind == QU_IMMUTABLE)
# define qual_isChecked(q)        ((q)->kind == QU_CHECKED)
# define qual_isCheckMod(q)       ((q)->kind == QU_CHECKMOD)
# define qual_isCheckedStrict(q)  ((q)->kind == QU_CHECKEDSTRICT)
# define qual_isUnchecked(q)      ((q)->kind == QU_UNCHECKED)
# define qual_isUndef(q)          ((q)->kind == QU_UNDEF)
# define qual_isKilled(q)         ((q)->kind == QU_KILLED)
# define qual_isPrintfLike(q)     ((q)->kind == QU_PRINTFLIKE)
# define qual_isScanfLike(q)      ((q)->kind == QU_SCANFLIKE)
# define qual_isMessageLike(q)    ((q)->kind == QU_MESSAGELIKE)
# define qual_isMetaState(q)      ((q)->kind == QU_USERANNOT)
# define qual_isNullTerminated(q) ((q)->kind == QU_NULLTERMINATED)

extern qual qual_createPlain (quenum) /*@*/ ;
extern qual qual_createMetaState (/*@observer@*/ annotationInfo) /*@*/ ;
extern qual qual_createTrueNull (void) /*@*/ ;   
extern qual qual_createFalseNull (void) /*@*/ ;  
extern qual qual_createRefCounted (void) /*@*/ ; 
extern qual qual_createRefs (void) /*@*/ ;       
extern qual qual_createNewRef (void) /*@*/ ;     
extern qual qual_createKillRef (void) /*@*/ ;    
extern qual qual_createTempRef (void) /*@*/ ;    
extern qual qual_createNotNull (void) /*@*/ ;    
extern qual qual_createAbstract (void) /*@*/ ;  
extern qual qual_createNumAbstract (void) /*@*/ ;    
extern qual qual_createConcrete (void) /*@*/ ;   
extern qual qual_createMutable (void) /*@*/ ;    
extern qual qual_createImmutable (void) /*@*/ ;  
extern qual qual_createShort (void) /*@*/ ;      
extern qual qual_createLong (void) /*@*/ ;       
extern qual qual_createSigned (void) /*@*/ ;     
extern qual qual_createUnsigned (void) /*@*/ ;   
extern qual qual_createUnique (void) /*@*/ ;     
extern qual qual_createMayExit (void) /*@*/ ;    
extern qual qual_createExits (void) /*@*/ ;      
extern qual qual_createNeverExit (void) /*@*/ ;    
extern qual qual_createFalseExit (void) /*@*/ ;    
extern qual qual_createTrueExit (void) /*@*/ ;    
extern qual qual_createConst (void) /*@*/ ;      
extern qual qual_createRestrict (void) /*@*/ ;      
extern qual qual_createVolatile (void) /*@*/ ;   
extern qual qual_createInline (void) /*@*/ ;     
extern qual qual_createExtern (void) /*@*/ ;     
extern qual qual_createStatic (void) /*@*/ ;     
extern qual qual_createAuto (void) /*@*/ ;       
extern qual qual_createRegister (void) /*@*/ ;   
extern qual qual_createOut (void) /*@*/ ;        
extern qual qual_createIn (void) /*@*/ ;        
extern qual qual_createYield (void) /*@*/ ;      
extern qual qual_createOnly (void) /*@*/ ;       
extern qual qual_createOwned (void) /*@*/ ;      
extern qual qual_createDependent (void) /*@*/ ;  
extern qual qual_createRelDef (void) /*@*/ ;     
extern qual qual_createImpOnly (void) /*@*/ ;    
extern qual qual_createPartial (void) /*@*/ ;    
extern qual qual_createSpecial (void) /*@*/ ;    
extern qual qual_createKeep (void) /*@*/ ;       
extern qual qual_createKept (void) /*@*/ ;       
extern qual qual_createTemp (void) /*@*/ ;       
extern qual qual_createShared (void) /*@*/ ;     
extern qual qual_createNull (void) /*@*/ ;
extern qual qual_createIsNull (void) /*@*/ ;       
extern qual qual_createRelNull (void) /*@*/ ;    
extern qual qual_createReturned (void) /*@*/ ;   
extern qual qual_createExposed (void) /*@*/ ;    
extern qual qual_createObserver (void) /*@*/ ;   
extern qual qual_createUnused (void) /*@*/ ;     
extern qual qual_createExternal (void) /*@*/ ;     
extern qual qual_createSef (void) /*@*/ ;     
extern qual qual_createChecked (void) /*@*/ ;     
extern qual qual_createCheckMod (void) /*@*/ ;     
extern qual qual_createUnchecked (void) /*@*/ ;     
extern qual qual_createCheckedStrict (void) /*@*/ ;     
extern qual qual_createUndef (void) /*@*/ ;
extern qual qual_createKilled (void) /*@*/ ;
extern qual qual_createNullTerminated (void) /*@*/ ;

extern qual qual_createUnknown (void) /*@*/ ;
# define qual_createUnknown()    (qual_createPlain (QU_UNKNOWN))

extern qual qual_createPrintfLike (void) /*@*/ ;
extern qual qual_createScanfLike (void) /*@*/ ;
extern qual qual_createMessageLike (void) /*@*/ ;

# define qual_createPrintfLike()  (qual_createPlain (QU_PRINTFLIKE))
# define qual_createScanfLike()   (qual_createPlain (QU_SCANFLIKE))
# define qual_createMessageLike() (qual_createPlain (QU_MESSAGELIKE))

# define qual_createTrueNull()   qual_createPlain (QU_TRUENULL)
# define qual_createFalseNull()  qual_createPlain (QU_FALSENULL)
# define qual_createRefCounted() qual_createPlain (QU_REFCOUNTED)
# define qual_createRefs()       qual_createPlain (QU_REFS)
# define qual_createNewRef()     qual_createPlain (QU_NEWREF)
# define qual_createKillRef()    qual_createPlain (QU_KILLREF)
# define qual_createTempRef()    qual_createPlain (QU_TEMPREF)
# define qual_createNotNull()    qual_createPlain (QU_NOTNULL)
# define qual_createAbstract()   qual_createPlain (QU_ABSTRACT)
# define qual_createNumAbstract()   qual_createPlain (QU_NUMABSTRACT)
# define qual_createConcrete()   qual_createPlain (QU_CONCRETE)
# define qual_createMutable()    qual_createPlain (QU_MUTABLE)
# define qual_createImmutable()  qual_createPlain (QU_IMMUTABLE)
# define qual_createShort()      qual_createPlain (QU_SHORT)
# define qual_createLong()       qual_createPlain (QU_LONG)
# define qual_createSigned()     qual_createPlain (QU_SIGNED)
# define qual_createUnsigned()   qual_createPlain (QU_UNSIGNED)
# define qual_createUnique()     qual_createPlain (QU_UNIQUE)
# define qual_createMayExit()    qual_createPlain (QU_MAYEXIT)
# define qual_createExits()      qual_createPlain (QU_EXITS)
# define qual_createNeverExit()  qual_createPlain (QU_NEVEREXIT)
# define qual_createTrueExit()   qual_createPlain (QU_TRUEEXIT)
# define qual_createFalseExit()  qual_createPlain (QU_FALSEEXIT)
# define qual_createConst()      qual_createPlain (QU_CONST)
# define qual_createRestrict()   qual_createPlain (QU_RESTRICT)
# define qual_createVolatile()   qual_createPlain (QU_VOLATILE)
# define qual_createInline()     qual_createPlain (QU_INLINE)
# define qual_createExtern()     qual_createPlain (QU_EXTERN)
# define qual_createStatic()     qual_createPlain (QU_STATIC)
# define qual_createAuto()       qual_createPlain (QU_AUTO)
# define qual_createRegister()   qual_createPlain (QU_REGISTER)
# define qual_createOut()        qual_createPlain (QU_OUT)
# define qual_createIn()         qual_createPlain (QU_IN)
# define qual_createYield()      qual_createPlain (QU_YIELD)
# define qual_createOnly()       qual_createPlain (QU_ONLY)
# define qual_createOwned()      qual_createPlain (QU_OWNED)
# define qual_createDependent()  qual_createPlain (QU_DEPENDENT)
# define qual_createRelDef()     qual_createPlain (QU_RELDEF)
# define qual_createImpOnly()    qual_createPlain (QU_IMPONLY)
# define qual_createPartial()    qual_createPlain (QU_PARTIAL)
# define qual_createSpecial()    qual_createPlain (QU_SPECIAL)
# define qual_createKeep()       qual_createPlain (QU_KEEP)
# define qual_createKept()       qual_createPlain (QU_KEPT)
# define qual_createTemp()       qual_createPlain (QU_TEMP)
# define qual_createShared()     qual_createPlain (QU_SHARED)
# define qual_createNull()       qual_createPlain (QU_NULL)
# define qual_createIsNull()     qual_createPlain (QU_ISNULL)
# define qual_createRelNull()    qual_createPlain (QU_RELNULL)
# define qual_createReturned()   qual_createPlain (QU_RETURNED)
# define qual_createExposed()    qual_createPlain (QU_EXPOSED)
# define qual_createObserver()   qual_createPlain (QU_OBSERVER)
# define qual_createUnused()     qual_createPlain (QU_UNUSED)
# define qual_createExternal()   qual_createPlain (QU_EXTERNAL)
# define qual_createSef()        qual_createPlain (QU_SEF)
# define qual_createChecked()    qual_createPlain (QU_CHECKED)
# define qual_createCheckMod()   qual_createPlain (QU_CHECKMOD)
# define qual_createCheckedStrict()  qual_createPlain (QU_CHECKEDSTRICT)
# define qual_createUnchecked()   qual_createPlain (QU_UNCHECKED)
# define qual_createUndef()       qual_createPlain (QU_UNDEF)
# define qual_createKilled()      qual_createPlain (QU_KILLED)


# define qual_createNullTerminated() qual_createPlain (QU_NULLTERMINATED)

/* start modifications */
/* This is used to check if we the qualifier matches any of the
len/size/nullterminated types */

extern bool qual_isBufQualifier(/*@sef@*/ qual)  /*@*/;

# define qual_isBufQualifier(q) \
  (qual_isNullTerminated(q))

/* end of modification/s */

extern bool qual_isGlobCheck (/*@sef@*/ qual p_q) /*@*/ ;
# define qual_isGlobCheck(q) \
  (qual_isChecked (q) || qual_isCheckMod (q) \
   || qual_isUnchecked (q) || qual_isCheckedStrict (q))

# define qual_isNullPred(q) \
  (qual_isTrueNull (q) || qual_isFalseNull (q))

# define qual_isRefQual(q) \
  (qual_isRefCounted(q) || qual_isRefs(q) || qual_isNewRef (q) || \
   qual_isKillRef (q) || qual_isTempRef (q))

extern bool qual_isNullStateQual (/*@sef@*/ qual) /*@*/ ;
# define qual_isNullStateQual(q) \
  (qual_isNull (q) || qual_isIsNull (q) || qual_isRelNull (q) \
   || qual_isNotNull (q))

# define qual_isTypeQual(q) \
  (qual_isAbstract(q) || qual_isNumAbstract(q) || qual_isConcrete(q) \
   || qual_isMutable(q) || qual_isImmutable(q))

# define qual_isEitherAbstract(q) \
  (qual_isAbstract(q) || qual_isNumAbstract(q))

# define qual_isControlQual(q)  (qual_isExits(q) || qual_isMayExit (q))
# define qual_isStorageClass(q) (qual_isExtern(q) || qual_isStatic(q))
# define qual_isCQual(q) \
  (qual_isAuto(q) || qual_isRegister(q) || qual_isInline(q) \
   || qual_isConst(q) || qual_isVolatile(q) || qual_isShort(q) \
   || qual_isLong (q) || qual_isSigned (q) || qual_isUnsigned (q))

# define qual_isAllocQual(q) \
  (qual_isOut (q) || qual_isIn (q) || qual_isSpecial (q) \
   || qual_isPartial (q) || qual_isRelDef (q))

# define qual_isGlobalQual(q) \
  (qual_isUndef(q) || qual_isKilled (q))

# define qual_isImplied(q)      (qual_isImpOnly(q))
# define qual_isExQual(q)	(qual_isExposed (q) || qual_isObserver (q))

# define qual_isAliasQual(q) \
  (qual_isOnly(q) || qual_isImpOnly(q) || qual_isTemp(q) \
   || qual_isOwned (q) || qual_isShared (q) || qual_isUnique (q) \
   || qual_isDependent (q) || qual_isKeep (q) || qual_isKept (q))

extern bool qual_isExitQual (/*@sef@*/ qual p_q) /*@*/ ;  
# define qual_isExitQual(q) \
  (qual_isExits (q) || qual_isNeverExit (q) || qual_isMayExit (q) \
   || qual_isTrueExit (q) || qual_isFalseExit (q))

extern char qual_abstractCode (qual) /*@*/ ;
extern qual qual_abstractFromCodeChar (char) /*@*/ ;

# else
# error "Multiple include"
# endif





