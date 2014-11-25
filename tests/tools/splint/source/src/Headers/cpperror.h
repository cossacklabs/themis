# ifndef CPPERROR_H
# define CPPERROR_H

/* doesn't exit! */
extern void cppReader_fatalError (cppReader *p_pfile, /*@only@*/ cstring p_str);

extern /*@noreturn@*/ void cppReader_pfatalWithName (cppReader *p_pfile, 
						  cstring p_name);

extern void cppReader_errorLit (cppReader *p_pfile, /*@observer@*/ cstring p_msg);

extern void
cppReader_pedwarnWithLine (cppReader *p_pfile, int p_line, int p_column,
			   /*@only@*/ cstring p_msg);

extern void cppReader_error (cppReader *p_pfile, /*@only@*/ cstring p_msg);
extern void cppReader_warning (cppReader *p_pfile, /*@only@*/ cstring p_msg);

extern void cppReader_printContainingFiles (cppReader *p_pfile) /*@modifies g_warningstream, p_pfile@*/ ;
extern void cppReader_warningLit (cppReader *p_pfile, /*@observer@*/ cstring p_msg);

extern void cppReader_pedwarn (cppReader *p_pfile, /*@only@*/ cstring p_msg);

extern void cppReader_pedwarnLit (cppReader *p_pfile, /*@observer@*/ cstring p_msg);

extern void cppReader_errorWithLine (cppReader *p_pfile,
				     int p_line, int p_column, 
				     /*@only@*/ cstring p_msg);

extern void cppReader_perrorWithName (cppReader *p_pfile, cstring p_name);

# else
# error "Multiple include"
# endif


