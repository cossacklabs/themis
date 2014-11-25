
/*this is a fake function so that compilers will not generate warning about
functions that are defined but not used

I should probablymake an exprData api instead but this a simpler and it
restricts access to exprData
*/

/*@ignore@*/

static void fakeQuite1 (int x)
{
void* f;

x = 1;

x = x +6;

llfatalbug ("fake function fakeQuite1 called");
f = (void *) fakeQuite1;

f = (void *)  exprData_free;
f = (void *) exprData_freeShallow;
f =   (void *) exprData_getInitNode;
f = (void *)   exprData_getInitId;
f = (void *)   exprData_getIterSname;
f = (void *)   exprData_getIterAlist;
f = (void *)   exprData_getIterBody;
f = (void *)   exprData_getIterEname;
f = (void *)   exprData_getIterCallIter;
f = (void *)   exprData_getIterCallArgs;
f = (void *)   exprData_getCastNode;
f =  (void *)  exprData_getCastTok;
f =  (void *)  exprData_getCastType;
f =  (void *)  exprData_getId;
f =  (void *)  exprData_getTok;
f = (void *)   exprData_getOffsetType;
f = (void *)   exprData_getOffsetName;
f = (void *)   exprData_makeOp;
f = (void *)   exprData_makeUop;
f = (void *)   exprData_makeSingle;
f = (void *)   exprData_makeTok;
f = (void *)   exprData_makeIter;
f = (void *)   exprData_makeCall;
f = (void *)   exprData_makeIterCall;
f = (void *)   exprData_makeField;
f = (void *)   exprData_makeOffsetof;
f = (void *)   exprData_makeSizeofType;
f = (void *)   exprData_makeCast;
f = (void *)   exprData_makeInit;
f = (void *)   exprData_makeCond;
f = (void *)   exprData_makeFor;
f = (void *)   exprData_getPairA;
f = (void *)   exprData_getPairB;
f = (void *)   exprData_getFcn;
f = (void *)   exprData_getArgs;
f = (void *)   exprData_getTriplePred;
f = (void *)   exprData_getTripleInit;
f = (void *)   exprData_getTripleTrue;
f = (void *)   exprData_getTripleTest;
f = (void *)   exprData_getTripleFalse;
f = (void *)   exprData_getTripleInc;
f = (void *)   exprData_getFieldNode;
f = (void *)   exprData_getFieldName;
f = (void *)   exprData_getUopTok;
f = (void *)   exprData_getLiteral;
f = (void *)   exprData_getType;
}
/*@end@*/
