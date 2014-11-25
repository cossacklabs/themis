typedef struct {
  char *name;
  int val;
} *rec;

extern rec rec_last ;

extern rec 
  rec_create (char *name,
	      int val);
