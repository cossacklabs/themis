union
{
  unsigned int BCD;
  struct
  {
    unsigned int EINER              :4;
    unsigned int ZEHN               :4;
    unsigned int HUNDERT    :4;
    unsigned int TAUSEND    :4;
  } nibble;
} ergebnis;

