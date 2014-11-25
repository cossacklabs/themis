# ifndef EMPLOYEE_H
# define EMPLOYEE_H

# define maxEmployeeName (20)
# define employeeFormat "%9d  %-20s  %-6s  %-11s  %6d.00"
# define employeePrintSize (63)

# include "employee.lh"

/* Override the definition in employee.lh (can't put this in .lcl) */
/*@-incondefs@*/ /*@-redecl@*/ /*@-protoparammatch@*/
extern bool employee_setName (/*@special@*/ employee *p_e, char  /* na */[]) /*@sets p_e->name@*/;
/*@=incondefs@*/ /*@=redecl@*/ /*@=protoparammatch@*/
# define employee_initMod()  bool_initMod()
# endif
