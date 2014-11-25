# ifndef EMPLOYEE_H
# define EMPLOYEE_H

# define maxEmployeeName   ((size_t) 20)
# define employeePrintSize (63)

/*@notfunction@*/
# define FORMATEMPLOYEE "%9d  %-20s  %-6s  %-11s  %6d.00"


# include "employee.lh"

/*@-incondefs@*/ /*@-redecl@*/
extern bool employee_setName (/*@special@*/ employee *p_e, char  /* na */[]) /*@sets p_e->name@*/;
/*@=incondefs@*/ /*@=redecl@*/

# define employee_initMod()  bool_initMod()
# endif
