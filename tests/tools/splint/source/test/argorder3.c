int f (void)
{
  int i = 3;
  int j;
  int a[10];

  i = i++;        /* bad */
  a[i] = i++;     /* bad */
  a[i++] = i;     /* bad */ 
  j = i++ * i;    /* bad */
  j = i * i++;    /* bad */
  j += --i * ++i; /* bad * 2 */
}
