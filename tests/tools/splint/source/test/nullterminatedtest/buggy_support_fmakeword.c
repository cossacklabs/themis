
//lclint work arround

/*@nullterminated@*/ char * strcpy (char * tmp1, /*@nullterminated@*/ 
char * tmp);
/*@nullterminated@*/ char * LCLstrcpy (char * tmp1, /*@nullterminated@*/ 
char * tmp);

#define strcpy LCLstrcpy
/*@nullterminated@*/ char * NotdefinedLCLstrcpy (int * tmp1,  
/*@nullterminated@*/ char * tmp);


// char *my_malloc(int);

// /*@null@*/ /*@nullterminated@*/char *fmakeword() {
//    char *word;
//
//    word = my_malloc(1);
//    word = "df";
//    return word;
//}


/*@nullterminated@*/ char * test1 (char * p1)
{
// p1 = NotdefinedLCLstrcpy (p1, "bob");
 p1 = LCLstrcpy (p1, "bob");
return p1;
}
