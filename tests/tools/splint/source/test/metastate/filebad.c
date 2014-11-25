extern /*@open@*/ int badOpen (/*@closed@*/ FILE *p_f);

extern int badEnsures (FILE *p_f, /*@closed@*/ int p_x)
     /*@ensures open p_x;@*/ ;
