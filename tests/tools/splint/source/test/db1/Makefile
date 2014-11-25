###
### Splint db1 test
###
### Taken from sample, second iteration: weakchecks
###

.SUFFIXES: .lcl .lcs .lh .h .c .o
.PHONY: check clean

### SHELL = /bin/csh -f

SPLINT = splint -booltype bool -booltrue TRUE -boolfalse FALSE
LCL = $(SPLINT) -specundef +lh +quiet -nof
SPLINTLH = $(SPLINT) +lh
LCSFILES = bool.lcs dbase.lcs employee.lcs empset.lcs erc.lcs eref.lcs ereftab.lcs 

MODULES = bool.lcl employee eref empset ereftab erc dbase drive.c
OBJS = dbase.o employee.o empset.o erc.o eref.o ereftab.o 

test: 
	$(MAKE) -e clean
	$(MAKE) -e check

###
### Removed +showscan, produces different results on different platforms
###

### evans 2002-12-17: 2 fewer weak errors because of -abstractcompare default in weak

check: $(LCSFILES)
	-$(SPLINT) -showcol -weak $(MODULES) -expect 19
	-$(SPLINT) -showcol +strict -bounds -modfilesystem +showsummary $(MODULES) -expect 338

# evans 2002-07-09: 5 bufferoverflowhigh warnings for using sprintf

### The following rules generate .lh and .lcs files from .lcl files.  They also
### ensure that .h files appear to be updated whenever the corresponding .lh
### file is updated.

.lcl.lh:
	$(LCL) $*.lcl

.lcl.lcs:
	$(LCL) $*.lcl

.lh.h:
	touch $*.h

.c.o:
	$(GCC) -c $*.c 

###
### Clean removes junk and all derived files.
### Tidy removes junk and object and executable files.
###

clean: tidy
	-@rm -f *.c.html *.lcl.html *.h.html *.lh.html *.lcs *.lh 1> /dev/null

tidy:
	-@rm -f ,* .,* *.CKP *.BAK .emacs_[0-9]* core a.out *.*~ *~ *.o drive 11> /dev/null

### The following dependencies ensure that imported LCL specs 
### are checked before the specs that import them.

dbase.lcs:   employee.lcs empset.lcs 
empset.lcs:  employee.lcs
erc.lcs:     eref.lcs
eref.lcl:    employee.lcs
ereftab.lcs: employee.lcs eref.lcs

### The following dependencies ensure that a .o file is recompiled when the
### corresponding .c file, or one of the .h files it #includes, is changed.

dbase.o:    dbase.c bool.h dbase.h employee.h empset.h
drive.o:    drive.c bool.h dbase.h employee.h empset.h erc.h eref.h ereftab.h 
employee.o: employee.c employee.h 
empset.o:   empset.c bool.h empset.h erc.h eref.h employee.h employee.h 
erc.o:      erc.c bool.h erc.h eref.h employee.h 
eref.o:     eref.c eref.h employee.h 
ereftab.o:  ereftab.c bool.h ereftab.h erc.h eref.h employee.h 
