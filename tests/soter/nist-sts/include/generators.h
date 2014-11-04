

//#include	"../include/sha.h"

void	lcg();
double	lcg_rand(int, double, double*, int);
void	quadRes1();
void	quadRes2();
void	cubicRes();
void	exclusiveOR();
void	modExp();
void	bbs();
void	micali_schnorr();
void	SHA1();


/* The circular shifts. */
#define CS1(x) ((((ULONG)x)<<1)|(((ULONG)x)>>31))
#define CS5(x)  ((((ULONG)x)<<5)|(((ULONG)x)>>27))
#define CS30(x)  ((((ULONG)x)<<30)|(((ULONG)x)>>2))

/* K constants */

#define K0  0x5a827999L
#define K1  0x6ed9eba1L
#define K2  0x8f1bbcdcL
#define K3  0xca62c1d6L

#define f1(x,y,z)   ( (x & (y ^ z)) ^ z )

#define f3(x,y,z)   ( (x & ( y ^ z )) ^ (z & y) )

#define f2(x,y,z)   ( x ^ y ^ z )                           /* Rounds 20-39 */

#define  expand(x)  Wbuff[x%16] = CS1(Wbuff[(x - 3)%16 ] ^ Wbuff[(x - 8)%16 ] ^ Wbuff[(x - 14)%16] ^ Wbuff[x%16])

#define sub1Round1(count)      { \
	 temp = CS5(A) + f1(B, C, D) + E + Wbuff[count] + K0; \
	 E = D; \
	 D = C; \
	 C = CS30( B ); \
	 B = A; \
	 A = temp; \
	 } \

#define sub2Round1(count)   \
	 { \
	 expand(count); \
	 temp = CS5(A) + f1(B, C, D) + E + Wbuff[count%16] + K0; \
	 E = D; \
	 D = C; \
	 C = CS30( B ); \
	 B = A; \
	 A = temp; \
	} \

#define Round2(count)     \
	 { \
	 expand(count); \
	 temp = CS5( A ) + f2( B, C, D ) + E + Wbuff[count%16] + K1;  \
	 E = D; \
	 D = C; \
	 C = CS30( B ); \
	 B = A; \
	 A = temp;  \
	 } \

#define Round3(count)    \
	 { \
	 expand(count); \
	 temp = CS5( A ) + f3( B, C, D ) + E + Wbuff[count%16] + K2; \
	 E = D; \
	 D = C; \
	 C = CS30( B ); \
	 B = A; \
	 A = temp; \
	 }

#define Round4(count)    \
	 { \
	 expand(count); \
	 temp = CS5( A ) + f2( B, C, D ) + E + Wbuff[count%16] + K3; \
	 E = D; \
	 D = C; \
	 C = CS30( B ); \
	 B = A; \
	 A = temp; \
	 }
