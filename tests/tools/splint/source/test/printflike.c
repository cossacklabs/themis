/*@printflike@*/ 
int mprint (int x);

/*@printflike@*/ 
int kprint (int x, char *s);

/*@scanflike@*/ 
int sprint (void);

/*@scanflike@*/ 
int uprint ();

/*PRINTFLIKE*/
int qprint (int x, char *s, ...);

/*@printflike@*/ /*@messagelike@*/
int qxprint (int x, char *s, int f, ...);

