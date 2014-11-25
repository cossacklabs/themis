###
### Splint db3 test
###
### Taken from sample, thirteenth iteration: strictlib
###

.SUFFIXES: .lcl .lcs .lh .h .c .o
.PHONY: check clean

### SHELL = /bin/csh -f

SPLINT = splint
SPLINTF = $(SPLINT) -f ./.splintrc
LCL = $(SPLINTF) -specundef +lh +quiet -nof
SPLINTLH = $(SPLINTF) +lh 
LCSFILES = dbase.lcs employee.lcs empset.lcs erc.lcs eref.lcs ereftab.lcs bool.lcs check.lcs
MODULES = employee eref empset ereftab erc dbase drive.c bool.lcl check.lcl
OBJS = dbase.o employee.o empset.o erc.o eref.o ereftab.o 

test: 
	@$(MAKE) -e check

### Note there is a .splint file used for checking!

check: $(LCSFILES)
	-$(SPLINTF) -showcol $(MODULES) +strict -bounds -modfilesystem +showsummary -exportconstant -exportlocal -protoparamprefix p_ -expect 5
	-$(SPLINTF) -showcol $(MODULES) +strict -bounds -modfilesystem +showsummary -exportconstant -exportlocal -protoparamprefix p_ +strictlib -expect 82

# evans 2002-07-09: 5 new sprintf warnings


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
	-@rm -f *.c.html *.lcl.html *.h.html *.lh.html *.lcs *.lh >& /dev/null

tidy:
	-@rm -f ,* .,* *.CKP *.BAK .emacs_[0-9]* core a.out *.*~ *~ *.o drive >& /dev/null

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




