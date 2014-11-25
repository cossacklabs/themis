.PHONY: filerw fileopen

SPLINT = splint

all: filerw fileopen filebranch

filerw:
	${SPLINT} -mts file -mts filerw filerw.c -expect 3

fileopen:
	${SPLINT} -mts file -mts filerw file.c -expect 3

filebranch:
	${SPLINT} -mts file filebranch.c
