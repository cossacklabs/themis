/* Define short names for the unsigned types. */
typedef unsigned char byte;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

/* Since sys/types.h often defines one or more of these (depending on */
/* the platform), we have to take steps to prevent name clashes. */
/*** NOTE: This requires that you include std.h *before* any other ***/
/*** header file that includes sys/types.h. ***/
//#define bool bool_          / * (maybe not needed) * /
#define uchar uchar_
#define uint uint_
#define ushort ushort_
#define ulong ulong_
#include <sys/types.h>
#undef bool
#undef uchar
#undef uint
#undef ushort
#undef ulong

void f (unsigned int width)
{
  uint src_bytes = width >> 3;
}
