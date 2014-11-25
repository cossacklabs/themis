/*@only@*/   char *globonly1;
/*@only@*/   char *globonly2;
/*@only@*/   char *globonly3;
/*@only@*/   char *globonly4;
/*@only@*/   char *globonly5;
/*@shared@*/ char *globshared1;
/*@shared@*/ char *globshared2;

extern void free (/*@out@*/ /*@only@*/ void *s);
extern /*@only@*/ char *string_copy (char *s) /*@modifies nothing;@*/ ;

int f(/*@only@*/ char *only1, /*@only@*/ char *only2, /*@only@*/ char *only3, 
      char *temp1, /*@temp@*/ char *temp2, 
      /*@shared@*/ char *shared1, /*@shared@*/ char *shared2)
{
  char *local1;

  globonly1 = only1;   /* 1. Only storage globonly1 not released before assignment */
  free (globonly2);  
  globonly2 = only2; 
  free (globonly3);     /* okay...for now */

  globonly4 = shared1; /* 2. Only storage globonly4 not released before assignment
			  3. Shared storage assigned to only */
  globshared1 = shared2;         
  globshared1 = globshared2;
  globshared1 = globonly5; /* 4. Only storage assigned to shared */

  local1 = globonly1;   
  globshared1 = local1; /* 5. Only storage globonly1 assigned to shared (local1 aliases */
                        /* 6. Kept storage only1 assigned to shared (local1 aliases */
  globshared1 = string_copy (local1); /* 7. Only storage assigned to shared */
  
  globshared2 = temp2; /* 8. Temp storage temp2 assigned to shared */
  globonly4 = temp1;   /* 9. Only storage globonly4 not released before assignment
			 10. Temp storage temp1 assigned to only: globonly4 = temp1 */

  free (shared1); /* 11. Shared storage shared1 passed as only param */
  *only3 = 'a';
  *shared2 = 'c';

  return 3; /* 12. Only storage only3 not released before return */
	    /* 13. Function returns with global globonly3 referencing released */
}



