/* Contributed by Peteran, 12 Jan 2004 */
/* GCC extension: Conditionals with Omitted Operands
   "x ? : y" is same as x ? x : y
   http://gcc.gnu.org/onlinedocs/gcc-3.3.2/gcc/Conditionals.html#Conditionals
*/

/*@+boolint -exportlocal@*/
void cond1(void *testme)
{
	int i,j;
	i = (testme!=0)? : 0;
	j = (testme!=0)? (testme!=0): 0;
}

int cond2(void *testme)
{
	return (testme!=0)? : 0;
}

int cond3(int testme)
{
	return testme? testme: 0;
}

void test_use_before_definition() {
	int i;
	printf("%d\n", i?:0);
}
