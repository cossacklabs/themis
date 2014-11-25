extern void sockets_initialize (void)
     /*@requires sockets_uninitialized@*/
     /*@ensures sockets_initialized@*/ ;

extern void sockets_finalize (void)
     /*@requires sockets_initialized@*/
     /*@ensures sockets_uninitialized@*/ ;

extern void useSockets (void) /*@requires sockets_initialized@*/ ;

void test1 (int x) /*@requires sockets_uninitialized@*/ 
{ 
  if (x > 3) {
    sockets_initialize ();
   }

  useSockets (); /* okay (error before) */
}

void test2 (int x) /*@requires sockets_initialized@*/ 
{ 
  if (x > 3) {
    sockets_finalize ();
  } /* error */
}
