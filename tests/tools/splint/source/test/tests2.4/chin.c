#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <stdarg.h>
#include <errno.h>
#include "FrameL.h"
  
int main(int argc, char *argv[])   /*-------------- main ----*/
{
    struct FrFile *iFile;
    struct FrFile *oFile;
    struct FrameH *iFrame;
    struct FrameH *oFrame;
    struct FrAdcData *dmroAdcData;
    struct FrVect    *dmroVector;
    void             *actualData;
    double           *times;		   /* times at which samples
					      are taken */
    char        *projectName = "";
    unsigned int   runNumber = 0;
    unsigned int   frameNumber = 0;
    unsigned int   initialTime = 0;        /* start time of 1st frame */
    unsigned int   finalTime = 0;          /* time of end of data
					    * window wanted */
    unsigned int   startTime = 0;          /* start time of current
					    * frame */
    unsigned int   startTimeResidual = 0;  /* nanoseconds */
    
    long debugLevel;
    int i;           /* loop counter */
    size_t lastSize; /* size of latest actualData, in units of
		      * sizeof(short) */

    /*--- increase the Debug level to see possible errors ---*/

    if (argc == 3) {
	sscanf(argv[2],"%ld",&debugLevel);
    }
    else {
	debugLevel = 1;
	}
		FrLibSetLvl(debugLevel);
   }

/*
And here's the error message:


LCLint 2.4b --- 18 Apr 98

frameread.c: (in function main)
frameread.c:50:24: Variable debugLevel used before definition
  An rvalue is used that may not be initialized to a value on some execution
  path. (-usedef will suppress message)
frameread.c:50:35: *** Internal Bug at exprNode.c:3843: exprNode_preOp:
             unhandled op: . [errno: 25]
*** Last code point: exprNode.c:8474
*** Previous code point: exprNode.c:8474
system error: : Not a typewriter
     *** Please report bug to lclint-bug@sds.lcs.mit.edu ***
       (attempting to continue, results may be incorrect)
frameread.c:50:23: Format argument 1 to sscanf (%ld) expects long int * gets
                      long int: .debugLevel
  Type of parameter is not consistent with corresponding code in format string.
  (-formattype will suppress message)
   frameread.c:50:20: Corresponding format code
frameread.c:50:2: Return value (type int) ignored: sscanf(argv[2], ...
  Result returned by function call is not used. If this is intended, can cast
  result to (void) to eliminate message. (-retvalint will suppress message)
frameread.c:53:17: *** Internal Bug at exprNode.c:4765: exprNode.c:4765:
    llassert failed: FALSE: Unexpected default case reached! [errno: 25]
*** Last code point: exprNode.c:8474
*** Previous code point: exprNode.c:3216
system error: : Not a typewriter
     *** Please report bug to lclint-bug@sds.lcs.mit.edu ***
       (attempting to continue, results may be incorrect)

*/
