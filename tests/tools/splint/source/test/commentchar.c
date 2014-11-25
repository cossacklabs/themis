int f (void) /*#modifies x#*/;

/*#-commentchar %#*/

/*@ignore@*/ 
int x = 'c'; /* not ignored */
/*@end@*/

/*%ignore%*/ 
int x = 'c'; /* is ignored */
/*%end%*/

/*%-commentchar @%*/
/*@-commentchar5*/ 
/*@=commentchar*/ 
