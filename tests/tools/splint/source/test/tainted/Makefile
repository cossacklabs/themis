.PHONY: tainted 

SPLINT = splint

all: tainted 

tainted:
	-${SPLINT} -mts tainted tainted.c -mustfree -exportlocal -expect 4
	-${SPLINT} tainted-bad.mts tainted.xh tainted.c -mustfree -exportlocal -expect 9
	-${SPLINT} -mts tainted tainted2.c -mustfree -exportlocal -expect 1
	-${SPLINT} -mts tainted tainted3.c -mustfree -exportlocal -expect 1
	-${SPLINT} -mts tainted tainted4.c -mustfree -exportlocal -expect 1
	-${SPLINT} -mts tainted tainted5.c -mustfree -exportlocal
	-${SPLINT} -mts tainted taintedmerge.c -mustfree -exportlocal -expect 3
	-${SPLINT} -mts tainted taintedimplicit.c -mustfree -exportlocal -expect 1
	-${SPLINT} -mts tainted sprintf.c -expect 5
	
# evans 2002-07-09: 3 more warnings for sprintf because of sprintf
