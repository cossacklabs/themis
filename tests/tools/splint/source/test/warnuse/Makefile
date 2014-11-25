.PHONY: warnuse

SPLINT = splint

all: warnuse warngets

warnuse:
	${SPLINT} warnuse.c -expect 4
	${SPLINT} warnuse.c -warnuse -expect 1
	${SPLINT} warnuse.c -warnuse +bufferoverflow -expect 1
	${SPLINT} warnuse.c -bufferoverflow -expect 2

warngets:
	${SPLINT} warngets.c -expect 1
