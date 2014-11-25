char gc; 
int  gi;
char *gs;

int f()
{
  char c;
  unsigned char uc;
  int i;
  long int li = 23;
  short int silly = 3;
  char *s;

  printf("hullo this is a %s !", "test");
  (void) scanf("hullo, welcome to %d", &i); /* defines i */

  printf("even %d %c harder", i, c); /* 1. Variable c used before definition */
  uc = 'a';
  printf("even %d %c harder", li, uc); /* 2. printf format arg 1 (%d) expects int gets long int: li */
  printf("even %ld %d %hd %hd %d harder", i, li, i, silly, silly);  /* 3, 4. [5, 6.]
								     * arg1 (expects long), 
								     arg2 (expects int),
								     arg3 (expects short),
								     * (okay if +relaxquals) arg5 (expects int) */
  
  (void) scanf("%*d okay"); /* [NO! 5. Statement has no effect] */
  printf("%s %s", s, s); /* 5. Variable s used before definition */

  printf("a real %+14.3i", c, i); /* 6, 7. printf format arg 1 (%i) expects int gets char: c, extra arg */
  fprintf(stdout, "a real %+14.33i", c, i); /* 8, 9. fprintf format arg 1 (%i) expects int gets char: c, extra */
  printf("%% %d %f %f", c, i); /* 10, 11, 12. printf format arg 1, arg2, missing arg 3 */

  (void) scanf("hullo, welcome to %d", &i);
  (void) scanf("hullo, welcome to %d", i); /* 13. scanf format arg 1 (%d) expects int * gets int: i */

  /* 3 type errors */
  (void) fscanf(stdin, "hullo, welcome to %d %c %s", i, c, &s); /* 14, 15, 16. arg1, arg2, arg3 */

  /* 3 modification errors */
  (void) fscanf(stdin, "hullo, welcome to %23d %c %s", &gi, &gc, gs); /* 17, 18, 19. modifies g1, gc, gs */
  /* 1 modification error */
  (void) fscanf(stdin, "hullo, welcome to %*23d %*c %s", gs); /* 20. modifies gs */

  return 3;
}


