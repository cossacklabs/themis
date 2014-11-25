typedef int abst1, abst2; /* 1, 2. Mutable abstract type abst1 declared without pointer ... */
typedef int abst3; /* 3. Mutable abstract type abst3 declared without pointer ... */

int main (void)
{
  int i = 1;
  abst1 a1; abst2 a2; abst3 a3;

   /*@access abst1,abst3*/
/* 10 */ a1 = (abst1)i; /* ok */
         a2 = (abst2)i; /* 4. Cast to abstract type abst2: (abst2)i */
         a3 = (abst3)i; /* ok */

   /*@noaccess abst1,abst3*/

/* 16 */ a1 = (abst1)i; /* 5. Cast to abstract type abst1: (abst1)i */
         a2 = (abst2)i; /* 6. Cast to abstract type abst2: (abst2)i */
         a3 = (abst3)i; /* 7. Cast to abstract type abst3: (abst3)i */

   /*@access abst2*/

/* 22 */ a1 = (abst1)i; /* 8. Cast to abstract type abst1: (abst1)i */
         a2 = (abst2)i; /* ok */
         a3 = (abst3)i; /* 9. Cast to abstract type abst3: (abst3)i */

   /*@access abst1,abst3*/

/* 28 */ a1 = (abst1)i; /* ok */
         a2 = (abst2)i; /* ok */ 
         a3 = (abst3)i; /* ok */

   /*@noaccess abst1*/

/* 34 */ a1 = (abst1)i; /* 10. Cast to abstract type abst1: (abst1)i */
         a2 = (abst2)i; /* ok */
         a3 = (abst3)i; /* ok */

   /*@noaccess abst2*/

/* 40 */ a1 = (abst1)i; /* 11. Cast to abstract type abst1: (abst1)i */
         a2 = (abst2)i; /* 12. Cast to abstract type abst2: (abst2)i */
         a3 = (abst3)i; /* ok */

   /*@noaccess abst3*/

/* 46 */ a1 = (abst1)i; /* 13. Cast to abstract type abst1: (abst1)i */
         a2 = (abst2)i; /* 14. Cast to abstract type abst2: (abst2)i */
         a3 = (abst3)i; /* 15. Cast to abstract type abst3: (abst3)i */

  return 3;
}
