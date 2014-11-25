typedef int abst1, abst2; /* 1, 2. declaration errors (mutable types */
typedef char *FILE;

int main (void) {
  abst1 a;    abst1 *ap;
  abst2 b = 3;  /* 3. wrong type */
  abst2 *bp = (abst2 *) malloc(sizeof(abst2)); /* 4. cast to underlying abstract */
  FILE *f;
  int  *ip = (int *)malloc(sizeof(int));

        *ip = 3;           /* 5. possible null deref */
        *bp = 5;           /* 6, 7. possible null deref, assignment of int to abst2 */
/* 13 */ a = (abst1)ip;    /* 8. cast to abstract */
/* 14 */ ap = (abst1 *)ip; /* 9. cast to underlying abstract */
         f = (FILE *)ip;   /* 10. cast to underlying abstract */
         a = (abst1)a;     /* 11. redundant abstract cast */
/* 17 */ ip = (int *)ap;   /* 12, 13. cast from underlying abstract, allocated ip not released */
         ip = (int *)f;    /* 13. cast from underlying abstract */
/* 19 */ ip = (int *)a;    /* 14. cast from underlying abstract */
/* 20 */ a = (abst1)b;     /* 15, 16. cast from abst2, cast to abst1 */
/* 21 */ ap = (abst1 *)bp; /* 17, 18. cast to abst1 * , cast from abst2 * */
         return 0;         /* 19, 20. ip and bp not released */
}
