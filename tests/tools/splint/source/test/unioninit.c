union test2_u {
  short   test2_s;
  struct  test2_ius_s {
    unsigned char a;
    unsigned char b;
  } test2_ius;
};

union test2_u test2 = { 0 };

union test2_u test3 = { 0, 1 }; /* bad */

union test2_u test4 = { 23L }; /* bad */
