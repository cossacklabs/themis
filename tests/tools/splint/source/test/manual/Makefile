###
### This Makefile produces the figures included in the Splint Manual.
###

.SUFFIXES: .out .expect .c .lcl .h .lh .diff
.PHONY: clean

SPLINT = splint
SPLINTFLAGS = -warnflags -nof -linelen 55 -hints -showcol -booltype bool -showfunc -exportlocal -exportheader

RUNTOHTML = cat
# /u/evs/lclint/scripts/runtohtmls
GENHTML = /u/evs/lclint/scripts/genhtmls
CTOHTML = /u/evs/bin/ctohtmlplain
BASEDIR = /u/evs/perl/basedir
MAKERUNNER = /u/evs/lclint/scripts/makerunner

### SHELL = /bin/csh 

DIFF = diff

all: sample null mstring usedef bool palindrome only stack rstring unique \
     exposure modify globals annotglobs clauses order loop switch noeffect \
     ignore setChar multiError bounds

### In line example
.PHONY: sample
sample: sample.c
	$(SPLINT) $(SPLINTFLAGS) sample.c -expect 2

### Figure 2:
.PHONY: null
null: null.c
	$(SPLINT) $(SPLINTFLAGS) null.c -expect 1

### Figure 3:
.PHONY: mstring
mstring: mstring.c
	$(SPLINT) $(SPLINTFLAGS) mstring.c -expect 0

### Figure 4:
.PHONY: usedef
usedef: usedef.c
	$(SPLINT) $(SPLINTFLAGS) usedef.c -expect 3
	$(SPLINT) $(SPLINTFLAGS) usedef.c +impouts -expect 2

### Figure 5:
.PHONY: bool
bool: bool.c bool.h
	$(SPLINT) $(SPLINTFLAGS) bool.c +predboolptr -booltype bool -linelen 70 -expect 7

### Figure 6:
.PHONY: palindrome
palindrome: palindrome.c mstring.h bool.h
	$(SPLINT) $(SPLINTFLAGS) palindrome.c -expect 4 -linelen 45

### Figure 7:
.PHONY: only
only: only.c
	$(SPLINT) $(SPLINTFLAGS) only.c -expect 6

### Figure 8:
.PHONY: stack
stack: stack.c
	$(SPLINT) $(SPLINTFLAGS) stack.c -expect 3

### Figure 9:
### no code

### Figure 10:
.PHONY: rstring
rstring: rstring.c
	$(SPLINT) $(SPLINTFLAGS) rstring.c -expect 1

### Figure 11:
.PHONY: unique
unique: unique.c
	$(SPLINT) $(SPLINTFLAGS) unique.c -expect 1

### Figure 12:
.PHONY: returned
returned: returned.c
	$(SPLINT) $(SPLINTFLAGS) returned.c -expect 1

### Figure 13:
.PHONY: exposure
exposure: exposure.c
	$(SPLINT) $(SPLINTFLAGS) exposure.c -expect 4 +checks -exportlocal -exportheader  -linelen 50

### Figure 14:
.PHONY: modify
modify: modify.c
	$(SPLINT) +checks $(SPLINTFLAGS) modify.c -expect 2

### Figure 15:
.PHONY: globals
globals: globals.c
	$(SPLINT) +checks $(SPLINTFLAGS) globals.c -expect 2

### Figure 16:
.PHONY: annotglobals
annotglobs: annotglobs.c
	$(SPLINT) $(SPLINTFLAGS) annotglobs.c -expect 3 

### Figure 17:
.PHONY: clauses
clauses: clauses.c
	$(SPLINT) $(SPLINTFLAGS) clauses.c -expect 0

### Figure 18:
.PHONY: order
order: order.c
	$(SPLINT) $(SPLINTFLAGS) order.c +evalorderuncon -expect 4 -linelen 60
	$(SPLINT) $(SPLINTFLAGS) order.c -expect 3

### Figure 19:
.PHONY: loop
loop: loop.c
	$(SPLINT) $(SPLINTFLAGS) loop.c +infloopsuncon -expect 2
	$(SPLINT) $(SPLINTFLAGS) loop.c -expect 1

### Figure 20:
.PHONY: switch
switch: switch.c
	$(SPLINT) $(SPLINTFLAGS) switch.c -expect 2

### Figure 21:
.PHONY: noeffect
noeffect: noeffect.c
	$(SPLINT) $(SPLINTFLAGS) noeffect.c +noeffectuncon -expect 3
	$(SPLINT) $(SPLINTFLAGS) noeffect.c -expect 2

### Figure 22:
.PHONY: ignore
ignore: ignore.c
	$(SPLINT) $(SPLINTFLAGS) bool.h ignore.c -expect 2
	$(SPLINT) $(SPLINTFLAGS) bool.h ignore.c -retvalbool -expect 1
	$(SPLINT) $(SPLINTFLAGS) bool.h ignore.c -retvalint -expect 1

### Figure 23:
.PHONY: Anames
names: names.c
	$(SPLINT) $(SPLINTFLAGS) names.c +distinctinternalnames +internalnamelookalike +isoreserved -expect 3

### Figure ???:

.PHONY: setChar
setChar: setChar.c
	$(SPLINT) $(SPLINTFLAGS) setChar.c +bounds -exportlocal +showconstraintlocation -expect 1


.PHONY: multiError
multiError: multiError.c
	$(SPLINT) $(SPLINTFLAGS) multiError.c +bounds -exportlocal +showconstraintlocation -expect 1


.PHONY: bounds
bounds: bounds.c
	$(SPLINT) $(SPLINTFLAGS) bounds.c +bounds -exportlocal +showconstraintlocation -expect 1 -linelen 50

clean: 
	-rm -f core a.out *.*~

