typedef int t_u_byte;

#define TESTVALUE  (t_u_byte) 7
typedef enum e_test
{
e_c1,
e_cl = (e_c1 + (((t_u_byte) 7) - 1)),
} t_test;
