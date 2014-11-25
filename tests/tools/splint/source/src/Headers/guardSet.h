/*
** guardSet.h
*/

# ifndef GUARDSET_H
# define GUARDSET_H

/*
** guardSet is usually empty, so allow NULL to represent this.
*/

/* in forwardTypes: typedef struct _guardSet *guardSet; */

/*@null@*/ struct s_guardSet
{
  /*@only@*/ sRefSet tguard; /* guarded on true branch */
  /*@only@*/ sRefSet fguard; /* guarded on false branch */
} ;

/*@constant null guardSet guardSet_undefined;@*/
# define guardSet_undefined     ((guardSet)NULL)

extern /*@falsewhennull@*/ /*@unused@*/ bool 
  guardSet_isDefined (guardSet p_g) /*@*/ ;
# define guardSet_isDefined(g)  ((g) != guardSet_undefined)

extern /*@falsewhennull@*/ bool guardSet_isEmpty (guardSet p_g);

extern /*@only@*/ guardSet guardSet_new (void);
extern guardSet guardSet_addTrueGuard (/*@returned@*/ guardSet p_g, /*@exposed@*/ sRef p_s);
extern guardSet guardSet_addFalseGuard (/*@returned@*/ guardSet p_g, /*@exposed@*/ sRef p_s);
extern guardSet guardSet_or (/*@returned@*/ /*@unique@*/ guardSet p_s, guardSet p_t);
extern guardSet guardSet_and (/*@returned@*/ /*@unique@*/ guardSet p_s, guardSet p_t);
extern void guardSet_delete (guardSet p_g, sRef p_s) /*@modifies p_g@*/ ;
extern /*@only@*/ cstring guardSet_unparse (guardSet p_g);
extern void guardSet_free (/*@only@*/ /*@only@*/ guardSet p_g);
extern /*@dependent@*/ /*@exposed@*/ sRefSet 
  guardSet_getTrueGuards (guardSet p_g) /*@*/ ;
extern /*@dependent@*/ /*@exposed@*/ sRefSet
  guardSet_getFalseGuards (guardSet p_g) /*@*/ ;
extern guardSet guardSet_union (/*@only@*/ guardSet p_s, guardSet p_t) 
   /*@modifies p_s@*/ ;
extern /*@only@*/ guardSet guardSet_invert (/*@temp@*/ guardSet p_g) /*@*/ ;
extern /*@only@*/ guardSet guardSet_copy (/*@temp@*/ guardSet p_g) /*@*/ ;
extern bool guardSet_isGuarded (guardSet p_g, sRef p_s) /*@*/ ;
extern bool guardSet_mustBeNull (guardSet p_g, sRef p_s) /*@*/ ;

extern guardSet
  guardSet_levelUnion (/*@only@*/ guardSet p_s, 
		       guardSet p_t, int p_lexlevel)
  /*@modifies p_s@*/ ;
extern guardSet
  guardSet_levelUnionFree (/*@returned@*/ /*@unique@*/ guardSet p_s, 
			   /*@only@*/ guardSet p_t, int p_lexlevel)
  /*@modifies p_t, p_s@*/ ;

extern void guardSet_flip (guardSet p_g);

# else
# error "Multiple include"
# endif
