# include <stdio.h>
# include <string.h>
# include "employee.h"

bool employee_setName (employee *e, char na []) 
{
  size_t i;
  
  for (i = 0; na[i] != '\0'; i++)
    {
      if (i == maxEmployeeName) {
	e->name[0] = '\0';
	return FALSE;
      }
    }

  strcpy (e->name, na);
  return TRUE;
}

bool employee_equal (employee * e1, employee * e2) 
{
  return ((e1->ssNum == e2->ssNum)
	  && (e1->salary == e2->salary)
	  && (e1->gen == e2->gen)
	  && (e1->j == e2->j)
	  && (strncmp (e1->name, e2->name, maxEmployeeName) == 0));
}

void employee_sprint (char s[], employee e) 
{
  static char *gender[] ={ "male", "female", "?" };
  static char *jobs[] = { "manager", "non-manager", "?" };
  
  (void) sprintf (s, employeeFormat, e.ssNum, e.name,
		  gender[e.gen], jobs[e.j], e.salary);
}

