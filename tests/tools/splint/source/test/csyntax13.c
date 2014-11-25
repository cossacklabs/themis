/*@-varuse*/
typedef int isr_t;
typedef int isrs;

int f()
{
  isr_t isrs;
  {
    isr_t isrs[3];
  }
  return 3;
}
