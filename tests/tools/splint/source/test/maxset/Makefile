.PHONY: check

SPLINT = splint -exportlocal
 
check:
	$(SPLINT) +boundswrite maxsetannotations.c
	$(SPLINT) +boundswrite maxsetnoannotations.c -expect 1
