/*
 * file: mp.c
 *
 * DESCRIPTION
 *
 * These functions comprise a multi-precision integer arithmetic
 * and discrete function package.
 */

#include	"../include/genutils.h"

#define	MAXPLEN		384


/*****************************************
** greater - Test if x > y               *
**                                       *
** Returns TRUE (1) if x greater than y, *
** otherwise FALSE (0).                  *
**                                       *
** Parameters:                           *
**                                       *
**  x      Address of array x            *
**  y      Address of array y            *
**  l      Length both x and y in bytes  *
**                                       *
******************************************/ 
int greater(BYTE *x, BYTE *y, int l)
{
	int		i;

	for ( i=0; i<l; i++ )
		if ( x[i] != y[i] )
			break;

	if ( i == l )
		return 0;

	if ( x[i] > y[i] )
		return 1;

	return 0;
}


/*****************************************
** less - Test if x < y                  *
**                                       *
** Returns TRUE (1) if x less than y,    *
** otherwise FALSE (0).                  *
**                                       *
** Parameters:                           *
**                                       *
**  x      Address of array x            *
**  y      Address of array y            *
**  l      Length both x and y in bytes  *
**                                       *
******************************************/ 
int less(BYTE *x, BYTE *y, int l)
{
	int		i;

	for ( i=0; i<l; i++ )
		if ( x[i] != y[i] )
			break;

	if ( i == l ) {
		return 0;
	}

	if ( x[i] < y[i] ) {
		return 1;
	}

	return 0;
}


/*****************************************
** bshl - shifts array left              *
**                  by one bit.          *
**                                       *	
** x = x * 2                             *
**                                       *
** Parameters:                           *	
**                                       *
**  x      Address of array x            *
**  l      Length array x in bytes       *
**                                       *
******************************************/ 
BYTE bshl(BYTE *x, int l)
{
	BYTE	*p;
	int		c1, c2;

	p = x + l - 1;
	c1 = 0;
	c2 = 0;
	while ( p != x ) {
		if ( *p & 0x80 )
			c2 = 1;
		*p <<= 1;  /* shift the word left once (ls bit = 0) */
		if ( c1 )
			*p |= 1;
		c1 = c2;
		c2 = 0;
		p--;
	}

	if ( *p & 0x80 )
		c2 = 1;
	*p <<= 1;  /* shift the word left once (ls bit = 0) */
	if ( c1 )
		*p |= (DIGIT)1;

	return (BYTE)c2;
}


/*****************************************
** bshr - shifts array right             *
**                   by one bit.         *
**                                       *	
** x = x / 2                             *
**                                       *
** Parameters:                           *	
**                                       *
**  x      Address of array x            *
**  l      Length array x in bytes       *	
**                                       *
******************************************/
void bshr(BYTE *x, int l)	
{
	BYTE	*p;
	int		c1,c2;

	p = x;
	c1 = 0;
	c2 = 0;
	while ( p != x+l-1 ) {
		if ( *p & 0x01 )
			c2 = 1;
		*p >>= 1;  /* shift the word right once (ms bit = 0) */
		if ( c1 )
			*p |= 0x80;
		c1 = c2;
		c2 = 0;
		p++;
	}

	*p >>= 1;  /* shift the word right once (ms bit = 0) */
	if ( c1 )
		*p |= 0x80;
}


/*****************************************
** Mult - Multiply two integers          *
**                                       *
** A = B * C                             *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of the result         *
**  B      Address of the multiplier     *
**  C      Address of the multiplicand   *
**  LB      Length of B in bytes         *
**  LC      Length of C in bytes         *
**                                       *
**  NOTE:  A MUST be LB+LC in length     *
**                                       *
******************************************/
int Mult(BYTE *A, BYTE *B, int LB, BYTE *C, int LC)
{
	int		i, j, k, LA;
	DIGIT	result;

	LA = LB + LC;

	for ( i=LB-1; i>=0; i-- ) {
		result = 0;
		for ( j=LC-1; j>=0; j-- ) {
			k = i+j+1;
			result = (DIGIT)A[k] + ((DIGIT)(B[i] * C[j])) + (result >> 8);
			A[k] = (BYTE)result;
			}
		A[--k] = (BYTE)(result >> 8);
	}

	return 0;
}


void ModSqr(BYTE *A, BYTE *B, int LB, BYTE *M, int LM)
{

	Square(A, B, LB);
	Mod(A, 2*LB, M, LM);
}

void ModMult(BYTE *A, BYTE *B, int LB, BYTE *C, int LC, BYTE *M, int LM)
{
	Mult(A, B, LB, C, LC);
	Mod(A, (LB+LC), M, LM);
}


/*****************************************
** smult - Multiply array by a scalar.   *
**                                       *
** A = b * C                             *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of the result         *
**  b      Scalar (1 BYTE)               *
**  C      Address of the multiplicand   *
**  L      Length of C in bytes          *
**                                       *
**  NOTE:  A MUST be L+1 in length       *
**                                       *
******************************************/
void smult(BYTE *A, BYTE b, BYTE *C, int L)
{
	int		i;
	DIGIT	result;

	result = 0;
	for ( i=L-1; i>0; i-- ) {
		result = A[i] + ((DIGIT)b * C[i]) + (result >> 8);
		A[i] = (BYTE)(result & 0xff);
		A[i-1] = (BYTE)(result >> 8);
	}
}

/*****************************************
** Square() - Square an integer          *
**                                       *
** A = B^2                               *
**                                       *
** Parameters:                           *
**                                       *
**  A      Address of the result         *
**  B      Address of the operand        *
**  L      Length of B in bytes          *
**                                       *
**  NOTE:  A MUST be 2*L in length       *
**                                       *
******************************************/
void Square(BYTE *A, BYTE *B, int L)
{
	Mult(A, B, L, B, L);
}

/*****************************************
** ModExp - Modular Exponentiation       *
**                                       *
** A = B ** C (MOD M)                    *
**                                       *	
** Parameters:                           *	
**                                       *
**  A      Address of result             *
**  B      Address of mantissa           *
**  C      Address of exponent           *
**  M      Address of modulus            *
**  LB     Length of B in bytes          *
**  LC     Length of C in bytes          *
**  LM     Length of M in bytes          *
**                                       *
**  NOTE: The integer B must be less     *
**        than the modulus M.      	 *
**  NOTE: A must be at least 3*LM        *
**        bytes long.  However, the      *
**        result stored in A will be     *
**        only LM bytes long.            *
******************************************/
void ModExp(BYTE *A, BYTE *B, int LB, BYTE *C, int LC, BYTE *M, int LM)
{
	BYTE	wmask;
	int		bits;

	bits = LC*8;
	wmask = 0x80;

	A[LM-1] = 1;

	while ( !sniff_bit(C,wmask) ) {
		wmask >>= 1;
		bits--;
		if ( !wmask ) {
			wmask = 0x80;
			C++;
		}
	}

	while ( bits-- ) {
		memset(A+LM, 0x00, LM*2);

		/* temp = A*A (MOD M) */
		ModSqr(A+LM, A,LM,  M,LM);

		/* A = lower L bytes of temp */
		memcpy(A, A+LM*2, LM);
		memset(A+LM, 0x00, 2*LM);

		if ( sniff_bit(C,wmask) ) {
			memset(A+LM, 0x00, (LM+LB));
			ModMult(A+LM, B,LB, A,LM,  M,LM);       /* temp = B * A (MOD M) */
			memcpy(A, A+LM+(LM+LB)-LM, LM);  /* A = lower LM bytes of temp */
			memset(A+LM, 0x00, 2*LM);
		}
 
		wmask >>= 1;
		if ( !wmask ) {
			wmask = 0x80;
			C++;
		}
	}
}


/* DivMod:
 *
 *   computes:
 *         quot = x / n
 *         rem = x % n
 *   returns:
 *         length of "quot"
 *
 *  len of rem is lenx+1
 */
int DivMod(BYTE *x, int lenx, BYTE *n, int lenn, BYTE *quot, BYTE *rem)
{
	BYTE	*tx, *tn, *ttx, *ts, bmult[1];
	int		i, shift, lgth_x, lgth_n, t_len, lenq;
	DIGIT	tMSn, mult;
	ULONG	tMSx;
	int		underflow;

	tx = x;
	tn = n;
	
	/* point to the MSD of n  */
	for ( i=0, lgth_n=lenn; i<lenn; i++, lgth_n-- ) {
		if ( *tn )
			break;
		tn++;
	}
	if ( !lgth_n )
		return 0;
	
	/* point to the MSD of x  */
	for ( i=0, lgth_x=lenx; i<lenx; i++, lgth_x-- ) {
		if ( *tx )
			break;
		tx++;
	}
	if ( !lgth_x )
		return 0;

	if ( lgth_x < lgth_n )
		lenq = 1;
	else
		lenq = lgth_x - lgth_n + 1;
	memset(quot, 0x00, lenq);
	
	/* Loop while x > n,  WATCH OUT if lgth_x == lgth_n */
	while ( (lgth_x > lgth_n) || ((lgth_x == lgth_n) && !less(tx, tn, lgth_n)) ) {
		shift = 1;
		if ( lgth_n == 1 ) {
			if ( *tx < *tn ) {
				tMSx = (DIGIT) (((*tx) << 8) | *(tx+1));
				tMSn = *tn;
				shift = 0;
			}
			else {
				tMSx = *tx;
				tMSn = *tn;
			}
		}
		else if ( lgth_n > 1 ) {
			tMSx = (DIGIT) (((*tx) << 8) | *(tx+1));
			tMSn = (DIGIT) (((*tn) << 8) | *(tn+1));
			if ( (tMSx < tMSn) || ((tMSx == tMSn) && less(tx, tn, lgth_n)) ) {
				tMSx = (tMSx << 8) | *(tx+2);
				shift = 0;
			}
		}
		else {
			tMSx = (DIGIT) (((*tx) << 8) | *(tx+1));
			tMSn = *tn;
			shift = 0;
		}

		mult = (DIGIT) (tMSx / tMSn);
		if ( mult > 0xff )
			mult = 0xff;
		bmult[0] = mult & 0xff;

		ts = rem;
		do {
			memset(ts, 0x00, lgth_x+1);
			Mult(ts, tn, lgth_n, bmult, 1);

			underflow = 0;
			if ( shift ) {
				if ( ts[0] != 0 )
					underflow = 1;
				else {
					for ( i=0; i<lgth_x; i++ )
						ts[i] = ts[i+1];
					ts[lgth_x] = 0x00;
				}
			}
			if ( greater(ts, tx, lgth_x) || underflow ) {
				bmult[0]--;
				underflow = 1;
			}
			else
				underflow = 0;
		} while ( underflow );
		sub(tx, lgth_x, ts, lgth_x);
		if ( shift )
			quot[lenq - (lgth_x - lgth_n) - 1] = bmult[0];
		else
			quot[lenq - (lgth_x - lgth_n)] = bmult[0];
		
		ttx = tx;
		t_len = lgth_x;
		for ( i=0, lgth_x=t_len; i<t_len; i++, lgth_x-- ) {
			if ( *ttx )
				break;
			ttx++;
		}
		tx = ttx;
	}
	memset(rem, 0x00, lenn);
	if ( lgth_x )
		memcpy(rem+lenn-lgth_x, tx, lgth_x);

	return lenq;
}


/* 
 * Mod - Computes an integer modulo another integer
 *
 * x = x (mod n)
 *
 */
void Mod(BYTE *x, int lenx, BYTE *n, int lenn)
{
	BYTE	quot[MAXPLEN+1], rem[2*MAXPLEN+1];

	memset(quot, 0x00, sizeof(quot));
	memset(rem, 0x00, sizeof(rem));
	if ( DivMod(x, lenx, n, lenn, quot, rem) ) {
		memset(x, 0x00, lenx);
		memcpy(x+lenx-lenn, rem, lenn);
	}
}

/* 
 * Div - Computes the integer division of two numbers
 *
 * x = x / n
 *
 */
void Div(BYTE *x, int lenx, BYTE *n, int lenn)
{
	BYTE	quot[MAXPLEN+1], rem[2*MAXPLEN+1];
	int		lenq;

	memset(quot, 0x00, sizeof(quot));
	memset(rem, 0x00, sizeof(rem));
	if ( (lenq = DivMod(x, lenx, n, lenn, quot, rem)) != 0 ) {
		memset(x, 0x00, lenx);
		memcpy(x+lenx-lenq, quot, lenq);
	}
}


/*****************************************
** sub - Subtract two integers           *
**                                       *
** A = A - B                             *
**                                       *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of subtrahend integer *
**  B      Address of subtractor integer *
**  L      Length of A and B in bytes    *
**                                       *
**  NOTE: In order to save RAM, B is     *
**        two's complemented twice,      *
**        rather than using a copy of B  *
**                                       *
******************************************/
void sub(BYTE *A, int LA, BYTE *B, int LB)
{
	BYTE	*tb;

	tb = (BYTE *)calloc(LA, 1);
	memcpy(tb, B, LB);
	negate(tb, LB);
	add(A, LA, tb, LA);

	FREE(tb);
}


/*****************************************
** negate - Negate an integer            *
**                                       *
** A = -A                                *
**                                       *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of integer to negate  *
**  L      Length of A in bytes          *
**                                       *
******************************************/
int negate(BYTE *A, int L)
{
	int		i, tL;
	DIGIT	accum;

	/* Take one's complement of A */
	for ( i=0; i<L; i++ )
		A[i] = ~(A[i]);

	/* Add one to get two's complement of A */
	accum = 1;
	tL = L-1;
	while ( accum && (tL >= 0) ) {
		accum += A[tL];
		A[tL--] = (BYTE)(accum & 0xff);
		accum = accum >> 8;
	}

	return accum;
}


/*
 * add()
 *
 * A = A + B
 *
 * LB must be <= LA
 *
 */
BYTE add(BYTE *A, int LA, BYTE *B, int LB)
{
	int		i, indexA, indexB;
	DIGIT	accum;

	indexA = LA - 1; 	/* LSD of result */
	indexB = LB - 1; 	/* LSD of B */

	accum = 0;
	for ( i = 0; i < LB; i++ ) {
		accum += A[indexA];
		accum += B[indexB--];
		A[indexA--] = (BYTE)(accum & 0xff);
		accum = accum >> 8;
	}

	if ( LA > LB )
		while ( accum  && (indexA >= 0) ) {
			accum += A[indexA];
			A[indexA--] = (BYTE)(accum & 0xff);
			accum = accum >> 8;
		}

	return (BYTE)accum;
}


void prettyprintBstr(char *S, BYTE *A, int L)
{
	int		i, extra, ctrb, ctrl;

	if ( L == 0 )
		printf("%s <empty>", S);
	else
		printf("%s\n\t", S);
	extra = L % 24;
	if ( extra ) {
		ctrb = 0;
		for ( i=0; i<24-extra; i++ ) {
			printf("  ");
			if ( ++ctrb == 4) {
				printf(" ");
				ctrb = 0;
			}
		}

		for ( i=0; i<extra; i++ ) {
			printf("%02X", A[i]);
			if ( ++ctrb == 4) {
				printf(" ");
				ctrb = 0;
			}
		}
		printf("\n\t");
	}

	ctrb = ctrl = 0;
	for ( i=extra; i<L; i++ ) {
		printf("%02X", A[i]);
		if ( ++ctrb == 4) {
			ctrl++;
			if ( ctrl == 6 ) {
				printf("\n\t");
				ctrl = 0;
			}
			else
				printf(" ");
			ctrb = 0;
		}
	}
	printf("\n\n");
}


/**********************************************************************/
/*  Performs byte reverse for PC based implementation (little endian) */
/**********************************************************************/
void byteReverse(ULONG *buffer, int byteCount)
{
	ULONG value;
	int count;

	byteCount /= sizeof( ULONG );
	for( count = 0; count < byteCount; count++ ) {
		value = ( buffer[ count ] << 16 ) | ( buffer[ count ] >> 16 );
		buffer[ count ] = ( ( value & 0xFF00FF00L ) >> 8 ) | ( ( value & 0x00FF00FFL ) << 8 );
	}
}

void
ahtopb (char *ascii_hex, BYTE *p_binary, int bin_len)
{
	BYTE    nibble;
	int     i; 
	
	for ( i=0; i<bin_len; i++ ) {
        nibble = ascii_hex[i * 2];
	    if ( nibble > 'F' )
	        nibble -= 0x20;   
	    if ( nibble > '9' )
	        nibble -= 7;      
	    nibble -= '0';   
	    p_binary[i] = nibble << 4;
		
	    nibble = ascii_hex[i * 2 + 1];
	    if ( nibble > 'F' )
			nibble -= 0x20;
        if ( nibble > '9' )
            nibble -= 7;   
        nibble -= '0';
		p_binary[i] += nibble;
	}
}