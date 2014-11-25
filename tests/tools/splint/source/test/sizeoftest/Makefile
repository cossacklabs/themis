.PHONY: check

SPLINT = splint -exportlocal
 
check:
	$(SPLINT) +bounds sizeof.c sizeofConst.c -expect 1


