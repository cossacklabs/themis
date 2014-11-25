/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
typedef struct limwr_t {
        int (*puts) (/* struct limwr_t *self, char *s, int slen */);
        unsigned (*tell) (/* struct limwr_t *self */);
        int (*seek) (/* struct limwr_t *self, unsigned n */);
        void (*close) (/* struct limwr_t *self */);
} *limwr_t;

mail()
{
  liwmr_t y;
  int i;

  i = (y->puts)();
    
  i = (y->close)();
