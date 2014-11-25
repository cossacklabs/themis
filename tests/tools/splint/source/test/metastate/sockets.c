extern void sockets_initialize (void)
     /*@requires sockets_uninitialized@*/
     /*@ensures sockets_initialized@*/ ;

extern void sockets_finalize (void)
     /*@requires sockets_initialized@*/
     /*@ensures sockets_uninitialized@*/ ;

extern void useSockets (void) /*@requires sockets_initialized@*/ ;

void test1 (void) /*@requires sockets_uninitialized@*/ 
{ 
  useSockets (); /* error */
}

void test2 (void) /*@requires sockets_initialized@*/ 
{
  useSockets (); /* okay */
}

void test3 (void) /*@requires sockets_uninitialized@*/ 
{ 
  sockets_initialize ();
  useSockets (); /* okay */
}

void test4 (void) /*@requires sockets_uninitialized@*/ /*@ensures sockets_uninitialized@*/
{
  sockets_initialize ();
  useSockets (); /* okay */
} /* error not finzalized */

void test5 (void) /*@requires sockets_uninitialized@*/ /*@ensures sockets_uninitialized@*/
{
  sockets_initialize ();
  sockets_finalize ();
  useSockets (); /* error */
} /* okay */

void test6 (void)
{ 
  useSockets (); /* error (default is uninitialized) */
}

void test7 (void)
{ 
  sockets_initialize (); /* okay (default is uninitialized) */
}


