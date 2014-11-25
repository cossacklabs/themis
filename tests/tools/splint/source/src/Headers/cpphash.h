# ifndef CPPHASH_H
# define CPPHASH_H

/*@constant int CPP_HASHSIZE@*/
# define CPP_HASHSIZE 1403

/* different kinds of things that can appear in the value field
   of a hash node.  Actually, this may be useless now. */
typedef union u_hashvalue {
  int ival;
  /*@null@*/ /*@owned@*/ char *cpval;
  /*@owned@*/ DEFINITION *defn;
} hashValue;

struct s_hashNode {
  /* double links for easy deletion */
  /*@only@*/ /*@null@*/ hashNode next;
  /*@dependent@*/ /*@null@*/ hashNode prev;

  /*
  ** Also, a back pointer to this node's hash
  ** chain is kept, in case the node is the head
  ** of the chain and gets deleted. 
  */

  /*@null@*/ /*@dependent@*/ hashNode *bucket_hdr;

  enum node_type type;		/* type of special token */
  size_t length;		/* length of token, for quick comparison */
  cstring name;			/* the actual name */
  hashValue value;		/* pointer to expansion, or whatever */
} ;

/* Some definitions for the hash table.  The hash function MUST be
   computed as shown in hashf () below.  That is because the rescan
   loop computes the hash value `on the fly' for most tokens,
   in order to avoid the overhead of a lot of procedure calls to
   the hashf () function.  Hashf () only exists for the sake of
   politeness, for use when speed isn't so important. */

extern void cppReader_deleteMacro (/*@exposed@*/ hashNode p_hp) 
     /*@modifies internalState, p_hp@*/;

extern /*@exposed@*/ hashNode cpphash_install (char *, int, 
						  enum node_type, int, 
						  /*@only@*/ /*@null@*/ char *, int);

extern void cppReader_hashCleanup (void);
extern /*@null@*/ /*@exposed@*/ hashNode cpphash_lookup (char *, int, int); 
extern /*@null@*/ /*@exposed@*/ hashNode cpphash_lookupExpand (char *, int, int,
							       bool p_forceExpand); 

extern int cpphash_hashCode (const char *p_name, size_t p_len, int p_hashsize) /*@*/ ;
extern void cppReader_saveHashtab (void);
extern void cppReader_restoreHashtab (void);

extern /*@exposed@*/ hashNode 
cpphash_installMacro (char *p_name, size_t p_len, 
		      /*@only@*/ struct definition *p_defn, int p_hash);

# else
# error "Multiple include"
# endif
