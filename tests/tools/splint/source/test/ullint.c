/*
** This test provided by Jim Zelenka
*/

typedef unsigned long       pan_uint32_t;
typedef long long           pan_int64_t;
typedef unsigned long long  pan_uint64_t;

struct t_s {
  pan_uint32_t  w;
  pan_uint32_t  x;
};

int
main (/*@unused@*/ int argc,
      /*@unused@*/ char **argv)
{
  unsigned char c, *src, _src[6];
  pan_uint32_t d, ui32;
  pan_uint64_t e;
  struct t_s st2;
  int i;

  c = 0x01; /* Error without +charintliteral or +charint */

  st2.w = 1;
  st2.x = 2;

  for(i=0;i<sizeof(src);i++) { /* Error for i < sizeof */
    _src[i] = (unsigned char)i;
  }

  src = _src;

  ui32 = (pan_uint32_t)((src[0] ^ 0xFC) << 30) | ((src[1] ^ 0x80) << 24) | ((src[2] ^ 0x80) << 18) | ((src[3] ^ 0x80) << 12) | ((src[4] ^ 0x80) << 6) | (src[5] ^ 0x80);

  d = c;

  e = st2.w + (st2.x * 1000000000);

  exit(0);
}
