.PHONY: tainted 

SPLINT = splint

all: tainted 

tainted:
	${SPLINT} -mts taintednm taintednm.c -mustfree -exportlocal -expect 2
	${SPLINT} -mts tainted taintednm.c -mustfree -exportlocal -expect 4
