.PHONY: tainted file

SPLINT = splint

all: file sockets struct nullbranch osd

file:
	${SPLINT} -mts file file1.c -expect 4
	${SPLINT} -mts file file2.c -varuse -fcnuse -expect 2
	${SPLINT} -mts file file3.c -varuse -fcnuse -expect 2
	${SPLINT} -mts file file4.c -varuse -fcnuse -exportlocal -expect 1 
	${SPLINT} -mts file file5.c -varuse -fcnuse -exportlocal -expect 4
	${SPLINT} -mts file file6.c -varuse -fcnuse -exportlocal -expect 2
	${SPLINT} -mts file filebad.c -expect 3

sockets:
	${SPLINT} sockets.mts sockets.c -expect 3
	${SPLINT} sockets.mts sockets2.c -expect 2

struct:
	${SPLINT} -mts file struct.c -expect 1

nullbranch:
	${SPLINT} -mts file nullbranch.c -expect 2

osd:
	${SPLINT} -mts file osd.c -expect 2
