/*
 * lclfoo.c - test lclint
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   int i = 0, num;
   float pet = 0.0;
   int (*qfun)(float *);

   int (*getbuiltout(int))(float *);

   num = (argc > 1) ? atoi(argv[1]) : 0;

   qfun = getbuiltout(num);

   if (qfun != NULL) {
      i = qfun(&pet);
      printf("number in float %f and returns %d\n", pet, i);
      exit(EXIT_SUCCESS);
   } else exit(EXIT_FAILURE);
}

/*
 * should be a function (of one arg - int) returning
 * a pointer to a function (of one arg - float *) returning int
 * 
 * From: cdecl
 * cdecl> declare getbuiltout as function (verbose as int) returning
 *        pointer to function (pobj as pointer to float) returning int
 * int (*getbuiltout(int verbose))(float *pobj) { }
 */

int (*getbuiltout(int verbose))(float *)
{
   int qpolygon(float *);

   switch(verbose) {
    case 1:
      return qpolygon;
    default:
      return NULL;
   }
}

