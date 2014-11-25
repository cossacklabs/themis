## This file currently uses GNU Make features; those should really be replaced
## by configure-time handling.

AUTOMAKE_OPTIONS = 1.5 foreign

## Use this to configure diff output
DIFFFLAGS = 

# -u show context

#drl 3/27/2002 get the test suit to print a warning when splint is built under /usr/


## Set up the splint commands

#SPLINT    =  valgrind --leak-resolution=high --num-callers=20 --show-reachable=no --leak-check=no $(top_builddir)/src/splint$(EXEEXT)
SPLINT    =  $(top_builddir)/src/splint$(EXEEXT)

### These are shell-specific
## unexport LARCH_PATH
## unexport LCLIMPORTDIR

### This is horrible!  Can't we make top_builddir absolute?
#SPLINTNEST = valgrind --leak-resolution=high --num-callers=20 --show-reachable=no --leak-check=no ../$(top_builddir)/src/splint$(EXEEXT)
SPLINTNEST = ../$(top_builddir)/src/splint$(EXEEXT)
SPLINTPNEST = @$(SPLINTNEST) -nof
SPLINTP   = @$(SPLINT) -nof

# Make sure .splintrc files are not used so test results do not
# depend on local settings.
SPLINTRN  = $(SPLINTP) -hints -booltype "bool"
SPLINTR   = $(SPLINTRN) -exportlocal +debugfcnconstraint

SPLINTRNNEST  = $(SPLINTPNEST) -hints -booltype "bool"
SPLINTRNEST   = $(SPLINTRNNEST) -exportlocal +debugfcnconstraint

###
### rcfiles not included becuase file names will not match exactly
###

UNITTESTS = \
  help \
  abstptr abstract alias alttypes ansireserved argorder \
  args arraydims arrayinit arraylit blocks break cases cast chararraylit charlit clauses commentchar compdestroy \
  compoundliterals compoundstmt condifomit constannot controldepth csyntax czechnames czechoslovaknames deadparam \
  decl divzero enum enumtag exports external fields flags forbody format freearray \
  funcpointer functionmacro glob globals impabstract info init innerarray inparam internal iter keep libs \
  linked lintcomments list longint loopexec looptesteffect \
  macros macrosef malloc merge mergenull modifies modtest moduncon \
  mongoincludes mystrncat noeffect null nullret nullassign numabstract observer oldstyle outglob outparam \
  parentype postnotnull preds prefixes printflike rc refcounts release repexpose \
  returned russian sharing shifts sizesigns slovaknames \
  specclauses \
  special stack staticarray strings \
  stringliteral \
  structassign typequals typeof ud ulstypes union unioninit \
  unnamedsu unreachable unsignedcompare \
  unused ullint utypes void widestrings
UNITEXPECTS = $(addsuffix .expect, $(UNITTESTS))

SUBDIRTESTS = metastate mergestate tainted fileio \
   simplebufferConstraintTests moreBufferTests moreBufferTests2 globalbufferannotation \
   maxset strchr sizeoftest for manual

### warnuse doesn't work yet!
SUBDIRTESTS += tests2.2
SUBDIRTESTS += tests2.4
SUBDIRTESTS += tests2.5
SUBDIRTESTS += db1 db2 db3

SPLINTTESTS = $(UNITTESTS) $(SUBDIRTESTS)

QUICKTESTS = db3

.PHONY: all check
all check: fulltest

.PHONY: version
version:
	-$(SPLINT) -help version

.PHONY: help

help:
	-@$(SPLINT)
	-@$(SPLINT) -help
	-@$(SPLINTP) -asdf
	-@$(SPLINTP) +boolint +boolint 
	-@$(SPLINT) -help flags alpha

#commenting these out for the release because they will almost always fail
#since the default will only fit one system
#Don't want to panic the user... 
#	-@$(SPLINT) -help flags all     | $(GREP) -v "^larchpath <path> "     | $(GREP) -v "^lclimportdir <directory> " | $(GREP) -v "  Path argument.  Default: " |  $(GREP) -v "  Directory argument.  Default: " | $(GREP) -v "  lclimportdir <directory> "
#	-@$(SPLINT) -help flags full    | $(GREP) -v "^larchpath <path> "     | $(GREP) -v "^lclimportdir <directory> " | $(GREP) -v "  Path argument.  Default: " |  $(GREP) -v "  Directory argument.  Default: " | $(GREP) -v "  lclimportdir <directory> "
#	-@$(SPLINT) -help flags manual     | $(GREP) -v "^larchpath <path> "   | $(GREP) -v "^lclimportdir <directory> " | $(GREP) -v "  Path argument.  Default: " |  $(GREP) -v "  Directory argument.  Default: " | $(GREP) -v "  lclimportdir <directory> "

#larch and lclimportdir have different hardcoded default paths so don't include the path in the output..


.PHONY: clean-local
clean-local:
	-rm -f *~ #*# *.o *.lcs a.out
	-rm -f *.out
	-rm  *.lcd *.lh
	-cd db1; $(MAKE) clean
	-cd db2; $(MAKE) clean
	-cd db3; $(MAKE) clean

## All tests need splint to be built

$(SPLINTTESTS): $(SPLINT)

$(SPLINT):
	cd $(top_builddir)/src; $(MAKE)

## This is a kludgey way of processing the output to make it match exactly
## The last matcher is the most annoying, as it can differ (the first two are
##  pretty much fixed); usually, you'll have 'make[1]:', but on DOS it is
##  'make.exe[1]:' (it might even be 'c:/path/to/make.exe[1]:', but that's
##  not supported).
## The tests should really be re-done as shell-scripts or something... maybe
##  autotest could be used once it's finished.

CLEANOUTPUT = $(GREP) -v "Splint 3." | $(GREP) -v "$(SPLINT)" | $(GREP) -v "^make.*\[[1-9]*\]:" | $(GREP) -v "^gmake.*\[[1-9]*\]:" |   $(GREP) -v "^gmake -e" |  $(GREP) -v "^make -e" |$(GREP) -v "config.status: creating test/Makefile" | $(GREP) -v "cd .. && " | $(GREP) -v "CONFIG_HEADERS=" | $(GREP) -v "CONFIG_FILES="

#drl 12/07/2002 These rules for .c and .expect files don't make sense
# and they are confusing make dist so I'm taking them out.

## Not real C code

#.c.expect:
#	$(MAKE) $* |& $(CLEANOUTPUT) >$*.expect && cat $*.expect
#
#.c.diff:
#	$(MAKE) $* |& $(CLEANOUTPUT) >$*.out && diff $*.expect $*.out


#drl temporally take this out.
#.PHONY: expects
#expects:
#	@echo "Saving old expects..."; \
#	 cat *.expect >expects-`date +"%y-%m-%d-%H"`
#	@for TEST in $(UNITTESTS) $(INTEGTESTS); do \
#	   echo "Making $$TEST.expect..."; \
#	   $(MAKE) $$TEST 2>&1 | $(CLEANOUTPUT) >$$TEST.expect; \
#	   cat $$TEST.expect; \
#	 done

CheckSystemDir = (pwd | grep -q  "/usr" && echo "Warning tests which not run correctly when splint is built in /usr or a subdirectory under /usr.  Build splint in a different directory or disregard test failures." )

.PHONY: quicktest
quicktest:
	$(CheckSystemDir)	
	@for TEST in $(QUICKTESTS); do \
	   echo "Checking $$TEST..."; \
	   $(MAKE) $$TEST 2>&1 | $(CLEANOUTPUT) >$$TEST.out && \
	     $(DIFF) $(DIFFFLAGS) $$TEST.expect $$TEST.out || \
               echo "*** FAIL ***"; \
	 done

.PHONY: fulltest
fulltest:
	@echo "Testing $(PACKAGE) $(VERSION)..."
	@echo
	@echo "Version Info:"
	@$(SPLINT) -help version
	@$(SPLINT) -help vars
	@echo ""
	@echo "Tests:"
	@echo ""
	@for TEST in $(SPLINTTESTS); do \
	   echo "Checking $$TEST..."; \
	   $(MAKE) $$TEST 2>&1 | $(CLEANOUTPUT) >$$TEST.out && \
	     $(DIFF) $(DIFFFLAGS) $$TEST.expect $$TEST.out || \
               echo "*** FAIL ***"; \
	 done

### Rules for tests start here

.PHONY: abstptr
abstptr:
	-$(SPLINTR) abstptr -expect 9
	-$(SPLINTR) abstptr +voidabstract -expect 6

.PHONY: abstract
abstract:
	-$(SPLINTR) abst_t.lcl commentcmd.c -expect 15

.PHONY: alias
alias:
	-$(SPLINTR) +lh mut
	-$(SPLINTR) mut alias +globalias -expect 19
	-$(SPLINTR) mut alias2 +globalias -expect 17
	-$(SPLINTR) +lh alias3 -expect 14 
	-$(SPLINTR) +lh alias4 +boolint
	-$(SPLINTR) alias4 -pred +retalias -expect 6
	-$(SPLINTR) +lh alias5 +memchecks -null -specundef -expect 5

.PHONY: alttypes
alttypes:
	-$(SPLINTR) alttypes.c -expect 2

###
### evans 2001-06-07 - updated nameCheck.c to reflect C9X.
### Reports one new errors for ansireserved.c - wctomb is bad even as a local
###    variable (could be a macro?)
### Reports 3 additional errors for +checks; no longer suppresses name errors
###    in the presense of other errors.
###

.PHONY: ansireserved
ansireserved:
	-$(SPLINTR) ansireserved.c +ansireserved -nolib -expect 9
	-$(SPLINTR) ansireserved.c +ansireserved +ansireservedlocal -nolib -expect 11
	-$(SPLINTRN) ansireserved.c +checks -exportlocal -exportheadervar -exportheader -expect 12
	-$(SPLINTR) ansireserved2.c +ansireserved -expect 1

.PHONY: argorder
argorder:
	-$(SPLINTR) argorder.c -expect 4
	-$(SPLINTR) argorder2  -expect 5
	-$(SPLINTR) argorder3.c -expect 8
	-$(SPLINTR) argorder4 -expect 9
	-$(SPLINTR) argorder4 -evalorder -expect 1
	-$(SPLINTR) argorder5.c +evalorderuncon -expect 3

.PHONY: args
args:
	-$(SPLINTR) args -noeffect -expect 12

.PHONY: arraydims
arraydims:
	-$(SPLINTR) arraydims.c -varuse -expect 2
	-$(SPLINTR) arraydims.c -initsize -varuse

.PHONY: arrayinit
arrayinit:
	-$(SPLINTR) arrayinit.c -expect 9

.PHONY: arraylit
arraylit:
	-$(SPLINTR) arraylit.c -expect 2
	-$(SPLINTR) arraylit.c +stringliteralsmaller -expect 4

.PHONY: blocks
blocks:
	-$(SPLINTR) blocks.c -expect 4
	-$(SPLINTR) blocks.c +ifblock +elseifcomplete -expect 7
	-$(SPLINTR) blocks.c -ifempty +whileempty +whileblock -expect 3
	-$(SPLINTR) blocks.c -ifempty +forempty +forblock -expect 3
	-$(SPLINTR) blocks.c +allempty -expect 6
	-$(SPLINTRN) blocks.c +strict -exportlocal +partial -exportheader -expect 11

.PHONY: break
break:
	-$(SPLINTR) break.c -expect 4
	-$(SPLINTR) break.c +deepbreak -expect 6
	-$(SPLINTR) break.c +deepbreak -looploopbreak -expect 5

.PHONY: cases
cases: 
	-$(SPLINTR) cases.c -expect 5
	-$(SPLINTR) cases2.c -expect 2
	-$(SPLINTRN) cases2.c +checks -exportlocal -exportheader -expect 3
	-$(SPLINTRN) cases2.c +checks -exportlocal -exportheader -branchstate -expect 3

.PHONY: cast
cast:
	-$(SPLINTR) cast -accessmodule -expect 20
	-$(SPLINTRN) cast2.c +checks -exportlocal -exportheader -expect 3

.PHONY: chararraylit
chararraylit:
	-$(SPLINTR) chararraylit.c -expect 2

### Two addition errors detected with 2.5 with -numliteral.

### evans 2002-12-15: finds one more but in charlit
.PHONY: charlit
charlit:
	-$(SPLINTR) +hints charlit.c -expect 4
	-$(SPLINTR) +hints -numliteral charlit.c -expect 6
	-$(SPLINTR) +hints charlit.c +charintliteral +ignoresigns -expect 1

.PHONY: clauses
clauses: 
	-$(SPLINTR) clauses.c +memchecks -expect 4
	-$(SPLINTR) clauses2.c +memchecks 
	-$(SPLINTR) clauses3.c +memchecks -expect 2
	-$(SPLINTR) clauses3.c +memchecks +unixlib -expect 3

.PHONY: commentchar
commentchar:
	-$(SPLINTR) commentchar.c -expect 4
	-$(SPLINTR) -commentchar '#' commentchar.c -expect 4

.PHONY: controldepth
controldepth:
	-$(SPLINTR) +hints -controlnestdepth 2 controldepth.c -expect 2
	-$(SPLINTR) +hints -controlnestdepth 1 controldepth.c -expect 2

###
### 1 extra warning reported for +strict now because of out-of-bounds read
###

.PHONY: compdestroy
compdestroy:
	-$(SPLINTRN) compdestroy.c +checks -exportlocal -exportheader -expect 1
	-$(SPLINTRN) compdestroy.c +checks -exportlocal -exportheader +strictdestroy -expect 2
	-$(SPLINTRN) compdestroy.c +checks -exportlocal -exportheader +strictdestroy +strictusereleased -expect 3
	-$(SPLINTRN) compdestroy.c +strict +partial -exportheader -expect 4

.PHONY: compoundliterals
compoundliterals:
	${SPLINTRN} compoundliterals.c 

.PHONY: compoundstmt
compoundstmt:
	${SPLINTRN} compoundstmt.c -expect 3

.PHONY: condifomit
condifomit:
	${SPLINTRN} -gnuextensions condifomit.c -expect 4
	${SPLINTRN} +gnuextensions condifomit.c -expect 1

.PHONY: constannot
constannot:
	${SPLINTRN} constannot.c +boundswrite -exportlocal -expect 2

.PHONY: csyntax
csyntax:
	-$(SPLINTR) +quiet -incondefs csyntax.c -expect 1 
	-$(SPLINTR) +quiet csyntax2.c -expect 2
	-$(SPLINTR) +quiet csyntax3.c -expect 1
	-$(SPLINTR) +quiet -incondefs csyntax4.c
	-$(SPLINTR) +quiet csyntax5.c
	-$(SPLINTR) +quiet csyntax6.c
	-$(SPLINTR) +quiet csyntax7.c
	-$(SPLINTR) +quiet csyntax8.c
	-$(SPLINTR) +quiet csyntax9.c
	-$(SPLINTR) +quiet csyntax10.c
	-$(SPLINTR) +quiet csyntax11.c
	-$(SPLINTR) +quiet csyntax12.c
	-$(SPLINTR) +quiet csyntax13.c -expect 1
	-$(SPLINTR) +quiet csyntax14.c
	-$(SPLINTR) +quiet csyntax15.c
	-$(SPLINTR) +quiet csyntax16.c -expect 2
	-$(SPLINTR) +quiet csyntax17.c -expect 3

.PHONY: czechnames
czechnames:
	-$(SPLINTR) czechnames.c
	-$(SPLINTR) +hints +czech czechnames.c -expect 2
	-$(SPLINTR) +hints +czech -czechvars czechnames.c -expect 1
	-$(SPLINTR) +hints +czech -accessczech czechnames.c -expect 6

.PHONY: czechoslovaknames
czechoslovaknames:
	-$(SPLINTR) +hints +czechoslovak czechnames.c -expect 1
	-$(SPLINTR) +hints +czechoslovak slovaknames.c -expect 1
	-$(SPLINTR) +hints +czechoslovak +slovakvars slovaknames.c -expect 2

###
### deadparam added 2001-05-27
###

.PHONY: deadparam
deadparam:
	${SPLINTR} deadparam.c -expect 3

#
# Was expect 3 before 2.4.  Earlier versions did not handle implicit
# function pointers correctly.
#

.PHONY: decl
decl:
	-$(SPLINTR) decl.c -expect 2
	-$(SPLINTRN) decl.c +strict -exportlocal -expect 6
	-$(SPLINTR) decl2 -expect 4

.PHONY: divzero
divzero:
	-$(SPLINTR) divzero.c -varuse -expect 0

.PHONY: enum
enum:
	-$(SPLINTR) enum -expect 16
	-$(SPLINTR) enum -misscase -expect 14

.PHONY: enumtag
enumtag:
	-$(SPLINTR) enumtag.c -expect 2

.PHONY: exports
exports:
	-$(SPLINTR) exports.c +exporttype +exportvar +exportfcn +topuse +typeuse -expect 6
	-$(SPLINTR) exports.c +exportany -expect 3
	-$(SPLINTR) exports.c

.PHONY: external
external:
	-$(SPLINTR) external.c +partial
	-$(SPLINTR) external.c +partial +distinctexternalnames +ansi89limits -expect 2
	-$(SPLINTR) external.c -nolib +partial -externalnamelength 3 -expect 3
	-$(SPLINTR) external.c -nolib +partial -externalnamelength 3 +externalnamecaseinsensitive -expect 3
	-$(SPLINTR) external.c +partial -externalnamelength 3 -expect 4

.PHONY: fields
fields:
	-$(SPLINTR) fields.c +memchecks -expect 6
	-$(SPLINTR) fields2.c +memchecks -expect 5
	-$(SPLINTR) fields3.c +memchecks

.PHONY: flags
flags:
	-$(SPLINTR) flags.c -expect 8
	-$(SPLINTR) +nocomments flags.c -expect 2

### Added 2001-06-02

.PHONY: forbody
forbody:
	${SPLINTR} forbody.c -expect 2

### Added 2001-06-03
.PHONY: format
format:
	${SPLINTR} format.c -expect 3
	${SPLINTR} format.c -formatconst 

# two new errors (invalid lhs)

.PHONY: funcpointer
funcpointer:
	-$(SPLINTR) +memchecks +noparams funcpointer.c -expect 18

.PHONY: functionmacro
functionmacro:
	-$(SPLINTR) functionmacro.c -expect 2

.PHONY: glob
glob:
	-$(SPLINTR) glob -expect 4
	-$(SPLINTR) glob -globuse -expect 3
	-$(SPLINTR) glob +globunspec -expect 6

.PHONY: globals
globals:
	-$(SPLINTR) -modifies globals.c -expect 5
	-$(SPLINTR) -modifies globals.c +allglobals -expect 6
	-$(SPLINTR) -modifies globals.c +impcheckedglobals -expect 6
	-$(SPLINTR) -modifies globals.c -globals -checkstrictglobals -expect 2
	-$(SPLINTR) -modifies globals.c +globunspec -expect 6
	-$(SPLINTR) -modifies globals.c +globunspec +allglobals -expect 8

# Was -accessfile
.PHONY: impabstract
impabstract: 
	-$(SPLINTR) -accessmodule impabstract.c 
	-$(SPLINTR) -accessmodule +hints +impabstract impabstract.c -expect 2
	-$(SPLINTR) -accessmodule +hints +impabstract impabstract -expect 4

###
### evans 2001-12-30: Handle unrecognized pre-processor directives
###    (Reported by Pierluigi Sanzani)

.PHONY: info
info:
	${SPLINTR} info.c -expect 4

### evans 2001-10-14: Expected errors updated
.PHONY: init
init:
	-$(SPLINTR) init.c -expect 14
	-$(SPLINTRN) init.c +checks -exportlocal -exportheadervar -expect 17

### evans 2003-09-16
.PHONY: innerarray
innerarray:
	-$(SPLINTR) innerarray.c -expect 10



.PHONY: inparam
inparam:
	-$(SPLINTR) inparam.c -expect 2
	-$(SPLINTR) +impouts inparam.c -expect 1

.PHONY: internal
internal:
	-$(SPLINTR) internal.c -expect 1
	-$(SPLINTR) internal.c +distinctinternalnames -expect 1
	-$(SPLINTR) internal.c +distinctinternalnames +ansi89limits -expect 2
	-$(SPLINTR) internal.c -internalnamelen 28 -expect 3
	-$(SPLINTR) internal.c +internalnamecaseinsensitive -expect 3
	-$(SPLINTR) internal.c +internalnamecaseinsensitive +internalnamelookalike -expect 11

###
### iter
### 2001-06-06: Error message for iter.lcl:3,6 fixed to iter.lcl:3:6
### 

.PHONY: iter
iter:
	-$(SPLINTR) iter -expect 14 -lclexpect 1
	-$(SPLINTR) iter2.c -expect 12

.PHONY: keep
keep:
	-$(SPLINTR) keep.c +memchecks -expect 6

### libs
### 2001-05-22: 2 new errors found (fixed spec of signal)
### 2001-05-30: 3 new errors found (formatconst)
### 2002-07-08: 2 new errors found (getc modifies errno)

.PHONY: libs
libs:
	-$(SPLINTR) libs.c +longunsignedunsignedintegral -expect 18
	-$(SPLINTR) libs.c -expect 22
	-$(SPLINTR) libs.c +globunspec +modunspec -expect 25
	-$(SPLINTR) libs.c +strictlib +globunspec +modunspec -expect 44

.PHONY: lintcomments
lintcomments:
	-$(SPLINTR) lintcomments.c -expect 5
	-$(SPLINTR) lintcomments.c -warnlintcomments -expect 1
	-$(SPLINTR) lintcomments.c -lintcomments -expect 4

.PHONY: list
list:
	-$(SPLINTR) list.c -expect 3

###
### 2002-12-12: Added test case for +longint and +shortint flags
###

.PHONY: longint
longint:
	-$(SPLINTR) longint.c -expect 3
	-$(SPLINTR) longint.c +longint -expect 2
	-$(SPLINTR) longint.c +shortint -expect 2
	-$(SPLINTR) longint.c +shortint +longint -expect 0

###
### 2002-01-01: Added test case for obvious loop execution.
###

.PHONY: loopexec
loopexec:
	-$(SPLINTR) loopexec.c -expect 1
	-$(SPLINTR) loopexec.c -obviousloopexec -expect 3

.PHONY: looptesteffect
looptesteffect:
	-$(SPLINTR) looptesteffect.c -expect 1

.PHONY: macros
macros:
	-$(SPLINTR) macros -expect 17 
	-$(SPLINTR) macros.c +allmacros -expect 34
	-$(SPLINTR) macros.c +fcnmacros -expect 31


.PHONY: macrosef
macrosef:
	-$(SPLINTR) macrosef -expect 4
	-$(SPLINTR) macrosef.c +allmacros -expect 3
	-$(SPLINTR) macrosef.c +allmacros +sefuncon -expect 4

.PHONY: malloc
malloc:
	-$(SPLINTRN) malloc.c +bounds -exportlocal -expect 7

.PHONY: merge
merge:
	-$(SPLINTRN) merge.c +checks -exportlocal -exportheadervar -exportheader -expect 3

.PHONY: mergenull
mergenull:
	-$(SPLINTRN) mergenull.c 

.PHONY: modifies
modifies:
	-$(SPLINTR) modifies.c modclient.c +impcheckedstatics +mustmod -expect 7

.PHONY: modtest
modtest:
	-$(SPLINTR) modtest -expect 10
	-$(SPLINTR) modtest +modunspec -expect 13
	-$(SPLINTR) modtest +mustmod -expect 14

.PHONY: moduncon
moduncon:
	-$(SPLINTR) moduncon.c +moduncon -memchecks -expect 4
	-$(SPLINTRN) moduncon.c +strict +impboundsconstraints -exportlocal -expect 22 

.PHONY: mongoincludes
mongoincludes:
	-$(SPLINTR) mongoincludes.c -includenest 1 -expect 19
	-$(SPLINTR) mongoincludes.c -includenest 2 -expect 10
	-$(SPLINTR) mongoincludes.c -includenest 3 -expect 4
	-$(SPLINTR) mongoincludes.c -includenest 4 -expect 1
	-$(SPLINTR) mongoincludes.c -includenest 5 -expect 0

.PHONY: mystrncat
mystrncat:
	-$(SPLINTR) mystrncat.c +boundsread +boundswrite -expect 4 

.PHONY: noeffect
noeffect:
	${SPLINTP} noeffect.c +allmacros +checks -expect 3

###
### 2002-01-01: null1.c: expect increased to 15 because out must be defined
###                         checking detects one new error
###

.PHONY: null
null: 
	-$(SPLINTR) null1.c -expect 15
	-$(SPLINTR) null1.c -null -mustdefine -expect 4
	-$(SPLINTR) null2.c -expect 11
	-$(SPLINTR) null3.c -expect 15
	-$(SPLINTR) null3.c +unixlib -expect 16
	-$(SPLINTR) null4.c -expect 1
	-$(SPLINTR) null5.c -expect 4
	-$(SPLINTR) null6 -expect 4
	-$(SPLINTR) +quiet null6.lcl -dump null6
	-$(SPLINTR) null6.c -load null6 -expect 4

### Added for 3.0 (bugs reported by Kevin Broady)

.PHONY: nullret
nullret:
	-$(SPLINTR) nullret.c -expect 2
	-$(SPLINTR) -nullret nullret.c -expect 0

.PHONY: nullassign
nullassign:
	-$(SPLINTR) nullassign.c -expect 2
	-$(SPLINTR) -nullassign nullassign.c -expect 1


### Added for 3.1 - evans 2002-12-14

.PHONY: numabstract
numabstract:
	-$(SPLINTR) numabstract.c -expect 11
	-$(SPLINTR) numabstract.c +numabstractlit -expect 9
	-$(SPLINTR) numabstract.c -numabstractcast -expect 10

#
# Before 2.4, expected one more because error was reported both as 
# dependent and observer.
#

.PHONY: observer
observer:
	-$(SPLINTRN) observer +checks -exportlocal -exportheader -expect 9
	-$(SPLINTRN) observer.c +checks -exportlocal -exportheader -expect 8
	-$(SPLINTR) observer.c -expect 7

.PHONY: oldstyle
oldstyle:
	-$(SPLINTR) oldstyle oldstyle2.c -expect 5

.PHONY: outglob
outglob:
	-$(SPLINTR) outglob -expect 10

.PHONY: outparam
outparam:
	-$(SPLINTR) outparam -expect 12

### evans 2001-08-26: postnotnull new

.PHONY: postnotnull
postnotnull:
	${SPLINTR} postnotnull.c -expect 1

### evans 2002-02-09: added parentype.c
.PHONY: parentype
parentype:
	${SPLINTR} parentype.c 

#
# Four new +fcnuse errors for -strict (evans 2001-07-22)
# 

.PHONY: preds
preds:
	-$(SPLINTR) +hints preds.c -expect 6
	-$(SPLINTRN) +hints preds.c -weak -expect 1
	-$(SPLINTRN) +hints preds.c -strict +impboundsconstraints -exportlocal -exportheader -expect 10

.PHONY: prefixes
prefixes:
	-$(SPLINTR) prefixes.c +partial
	-$(SPLINTRN) prefixes.c +allmacros +checks -exportlocal +partial -exportheader -exportheadervar -expect 4
	-$(SPLINTR) prefixes.c -typeprefix "T" -expect 2
	-$(SPLINTR) prefixes.c -typeprefix "^" -expect 1
	-$(SPLINTR) prefixes.c -typeprefix "^*" -expect 2
	-$(SPLINTR) prefixes.c -typeprefix "^%*" -expect 2
	-$(SPLINTR) prefixes.c -typeprefix "^~*" -expect 2
	-$(SPLINTR) prefixes.c -typeprefix "^" +typeprefixexclude -expect 7
	-$(SPLINTR) prefixes.c -filestaticprefix "^^" -expect 4
	-$(SPLINTR) prefixes.c -filestaticprefix "^#" -expect 5
	-$(SPLINTR) prefixes.c -filestaticprefix "^?&x" -expect 5
	-$(SPLINTR) prefixes.c -globalprefix "G" -expect 1
	-$(SPLINTR) prefixes.c -globalprefix "&G?_^" -expect 1
	-$(SPLINTR) prefixes.c -externalprefix "G" -expect 5
	-$(SPLINTR) prefixes.c -typeprefix "T" -externalprefix "G" -expect 4
	-$(SPLINTR) prefixes.c -localprefix "?*" +localprefixexclude -expect 13

.PHONY: printflike
printflike:
	-$(SPLINTR) printflike.c -expect 6
	-$(SPLINTR) printflike.c -warnlintcomments -expect 5

.PHONY: rc
rc:
	-$(SPLINTR) -DMYSTERY='"a flag\"wicked cool"' rc.c -expect 1
	-$(SPLINTR) -DMYSTERY=12 rc.c -expect 1
	-$(SPLINTR) -f rc1.splintrc rc.c -expect 1
	-$(SPLINTR) -UMYSTERY -f rc1.splintrc rc.c -expect 1
	-$(SPLINTR) -f rc3.splintrc rc.c -expect 1

.PHONY: rcfiles
rcfiles:
	cd rcfiles; ${MAKE} SPLINT="-$(SPLINTPNEST)"

.PHONY: refcounts
refcounts:
	-$(SPLINTR) refcounts.c -expect 7 

.PHONY: release
release:
	-$(SPLINTR) release.c +memchecks -expect 1

.PHONY: repexpose
repexpose:
	-$(SPLINTR) +lh repexpose +memchecks -expect 12
	-$(SPLINTR) repexpose +memchecks +retalias -expect 15
	-$(SPLINTRN) repexpose +checks -exportlocal -expect 27

### returned added 2001-05-27
### (Bug discovered checking splint sources.)

.PHONY: returned
returned:
	${SPLINTR} returned.c -expect 1

### russian added 2003-06-07: bug reported in pre-processing non-standard characters.
.PHONY: russian
russian:
	${SPLINTR} russian.c 

.PHONY: sharing
sharing: 
	-$(SPLINTR) sharing1.c -expect 21
	-$(SPLINTR) sharing3.c -expect  3
	-$(SPLINTR) sharing4.c -expect 13
	-$(SPLINTR) sharing4.c -paramimptemp -expect 12
	-$(SPLINTR) sharing5.c -expect 6

.PHONY: shifts
shifts:
	-$(SPLINTR) shifts.c -expect 4
	-$(SPLINTR) shifts.c -shiftimplementation -expect 3
	-$(SPLINTR) shifts.c -shiftnegative -expect 1

#drl comment this out until sizesigns is added to cvs

### evans - added 2002-08-17: check warnings with arbitrary integral types
sizesigns:
	-$(SPLINTR) +strict sizesigns.c -expect 5
	-$(SPLINTR) +strict +matchanyintegral sizesigns.c -expect 4

.PHONY: slovaknames
slovaknames:
	-$(SPLINTR) +hints slovaknames.c -expect 1
	-$(SPLINTR) +hints slovaknames.c +accessslovak 
	-$(SPLINTR) +hints +slovak slovaknames.c -expect 3
	-$(SPLINTR) +hints +slovak -slovakvars slovaknames.c -expect 2
	-$(SPLINTR) +hints +slovak -accessslovak slovaknames.c -expect 7

##
## evans 2002-07-22: 1 less warning for specclauses3.c because of NULL result
##

.PHONY: specclauses
specclauses:
	-$(SPLINTR) specclauses.c -expect 6
	-$(SPLINTR) specclauses2.c -expect 8
	-$(SPLINTR) specclauses3.c -expect 5
	-$(SPLINTR) specclauses4.c -expect 3
	-$(SPLINTR) specclauses5.c -expect 3

.PHONY: specclauses1
specclauses1:
	-$(SPLINTR) specclauses.c -expect 6

.PHONY: specclauses2
specclauses2:
	-$(SPLINTR) specclauses2.c -expect 8

.PHONY: specclauses3
specclauses3:
	-$(SPLINTR) specclauses3.c -expect 6

.PHONY: specclauses4
specclauses4:
	-$(SPLINTR) specclauses4.c -expect 3

.PHONY: specclauses5
specclauses5:
	-$(SPLINTR) specclauses5.c -expect 3

.PHONY: special
special:
	-$(SPLINTR) special -expect 20
	-$(SPLINTR) special -relaxquals -expect 22

.PHONY: stack
stack:
	-$(SPLINTR) stack.c -expect 5
	-$(SPLINTR) stack.c -stackref

.PHONY: staticarray
staticarray:
	-$(SPLINTR) staticarray.c -expect 3

###
### evans 2002-03-16: Default setting of stringliteralnoroomfinalnull changed
### 

.PHONY: stringliteral
stringliteral:
	-$(SPLINTR) stringliteral.c -stringliteralnoroomfinalnull -expect 3
	-$(SPLINTR) stringliteral.c -expect 4

.PHONY: strings
strings:
	-$(SPLINTR) strings.c -expect 3
	-$(SPLINTR) -readonlystrings -expect 1 strings.c
	-$(SPLINTR) +modobserverstrict -maintype -expect 4 strings.c

.PHONY: structassign
structassign:
	-$(SPLINTR) structassign.c -expect 4

.PHONY: typeof
typeof:
	-$(SPLINTR) typeof.c -expect 2

.PHONY: typequals
typequals:
	-$(SPLINTR) typequals.c tq.lcl -expect 5
	-$(SPLINTR) typequals.c -expect 2

.PHONY: ud
ud:
	-$(SPLINTR) ud.c -expect 9
	-$(SPLINTR) ud2 -specundef -expect 3

.PHONY: ulstypes
ulstypes:
	-$(SPLINTR) ulstypes.c -expect 8
	-$(SPLINTR) ulstypes.c +ignorequals 
	-$(SPLINTRN) ulstypes.c +strict -exportheader -exportheadervar -expect 28

# 3 more detected with version 2.5 (change in -numliteral setting)

.PHONY: union
union:
	-$(SPLINTR) +memchecks union.c -expect 8

###
### Added 2001-12-30: fixed union initializer checking in response to
### bug report from Jim Zelenka.
###

.PHONY: unioninit
unioninit:
	-$(SPLINTR) unioninit.c -expect 2

.PHONY: unnamedsu
unnamedsu:
	-$(SPLINTR) unnamedsu.c -expect 0

.PHONY: unreachable
unreachable:
	-$(SPLINTR) unreachable.c -expect 5
	-$(SPLINTR) -unreachable unreachable.c -expect 2
	-$(SPLINTR) switch.c -expect 4

.PHONY: unsignedcompare
unsignedcompare:
	${SPLINTR} +posixlib unsignedcompare.c -expect 4


###
### 2001-06-08 evans: 2 new errors after fixing ansireserved name checks
###

.PHONY: unused
unused:
	-$(SPLINTRN) unused.c +checks -exportlocal -expect 5
	-$(SPLINTRN) unused.c +checks -exportlocal +topuse -expect 8

###
### 2001-06-10: Provided by Jim Zalenka
###

.PHONY: ullint
ullint:
	${SPLINTRN} ullint.c -expect 5
	${SPLINTRN} ullint.c +charint +charintliteral -expect 2

###
### 2001-12-30: Poor warnings reported by Peter Deutsch
###
### 2002-07-03: removed warnings for datatype/macro definitions
###

.PHONY: utypes
utypes:
	${SPLINTRN} utypes.c -expect 2

###

.PHONY: void
void:
	${SPLINTRN} void.c -expect 2

###
### 2001-12-30: Problems with wide character strings reported by Nelson Beebe
###

.PHONY: widestrings
widestrings:
	${SPLINTRN} widestrings.c -expect 2

###
### New since 2.5q:
###

.PHONY: linked
linked:
	${SPLINTR} linked.c -expect 4
	${SPLINTR} linked2.c -expect 3
	${SPLINTR} linked3.c -expect 5
	${SPLINTR} linked4.c -expect 6
	${SPLINTR} linked5.c -expect 4
	${SPLINTR} linked6.c -expect 4

.PHONY: freearray
freearray:
	${SPLINTR} freearray.c -expect 1

.PHONY: sizeof
sizeof:
	-$(SPLINTR) +bounds sizeof.c -expect 1


.PHONY: buffertest
buffertest:
	-$(SPLINTR) +bounds buffertest1.c -expect 5

## Integration Tests

## evans 2000-12-22
## db2: 2 errors are no longer reported, since eref is immutable.
## Need to clarify what it means for an object to be immutable;
## there should be 2 types with different storage requirements.

.PHONY: $(SUBDIRTESTS)
$(SUBDIRTESTS):
	cd $@; $(MAKE) SPLINT="$(SPLINTRNNEST)"

#drl 11/29/2001 This is a very ugly hack to get make dist to work
EXTRA_DIST =  ./abst_t.lcl \
              ./abstptr.lcl \
              ./alias.lcl \
              ./alias2.lcl \
              ./alias3.lcl  \
              ./db1/erc.lcl \
              ./db1/bool.lcl \
              ./db1/dbase.lcl \
              ./db1/employee.lcl \
              ./db1/empset.lcl \
              ./db1/eref.lcl \
              ./db1/ereftab.lcl \
              ./alias4.lcl  \
              ./alias5.lcl \
              ./argorder2.lcl \
              ./argorder4.lcl \
              ./args.lcl \
              ./cast.lcl \
              ./decl2.lcl \
              ./empty.lcl \
              ./enum.lcl \
              ./glob.lcl \
              ./impabstract.lcl \
              ./iter.lcl \
              ./macros.lcl \
              ./macrosef.lcl \
              ./modtest.lcl \
              ./mut.lcl \
              ./mut.lh.expect \
              ./db2/dbase.lcl \
              ./db2/employee.lcl \
              ./db2/empset.lcl \
              ./db2/erc.lcl \
              ./db2/eref.lcl \
              ./db2/ereftab.lcl \
              ./db2/etest.lcl \
              ./null6.lcl \
              ./observer.lcl \
              ./oldstyle.lcl \
              ./outglob.lcl \
              ./outparam.lcl \
              ./rc1.splintrc \
              ./rc2.splintrc \
              ./rc3.splintrc \
              ./rc3.splintrc.os2 \
              ./repexpose.lcl \
              ./repexpose.lh.expect \
              ./special.lcl \
              ./db3/.splintrc \
              ./db3/bool.lcl \
              ./db3/check.lcl \
              ./db3/dbase.lcl \
              ./db3/employee.lcl \
              ./db3/empset.lcl \
              ./db3/erc.lcl \
              ./db3/eref.lcl \
              ./db3/ereftab.lcl \
              ./db3/README \
              ./db3/clean \
              ./db3/tidy \
              ./tq.lcl \
              ./ud2.lcl \
              ./for/.splintrc \
              ./globalbufferannotation/.splintrc \
              ./maxset/.splintrc \
              ./strchr/.splintrc \
              ./tests2.2/bool.lcl \
              ./tests2.4/subdir/main.lcl \
              ./abstptr.c \
              ./alias.c \
              ./alias2.c \
              ./alias3.c \
              ./db1/dbase.c \
              ./db1/drive.c \
              ./db1/employee.c \
              ./db1/empset.c \
              ./db1/erc.c \
              ./db1/eref.c \
              ./db1/ereftab.c \
              ./alias4.c \
              ./alias5.c \
              ./alttypes.c \
              ./ansireserved.c \
              ./ansireserved2.c \
              ./argorder.c \
              ./argorder2.c \
              ./argorder3.c \
              ./argorder4.c \
              ./argorder5.c \
              ./args.c \
              ./arrayinit.c \
              ./arraylit.c \
              ./blocks.c \
              ./branchstate.c \
              ./break.c \
              ./cases.c \
              ./buffertest1.c \
              ./cases2.c \
              ./cast.c \
              ./cast2.c \
              ./chararraylit.c \
              ./charlit.c \
              ./clauses.c \
              ./clauses2.c \
              ./clauses3.c \
              ./commentchar.c \
              ./commentcmd.c \
              ./compdestroy.c \
              ./compoundliterals.c ./compoundliterals.expect \
              ./compoundstmt.c ./compoundstmt.expect \
              ./condifomit.c \
              ./csyntax.c \
              ./constannot.c \
              ./controldepth.c \
              ./csyntax10.c \
              ./csyntax11.c \
              ./csyntax12.c \
              ./csyntax13.c \
              ./csyntax14.c \
              ./csyntax15.c \
              ./csyntax16.c \
              ./csyntax17.c \
              ./csyntax2.c \
              ./csyntax3.c \
              ./csyntax4.c \
              ./csyntax5.c \
              ./csyntax6.c \
              ./csyntax7.c \
              ./csyntax8.c \
              ./csyntax9.c \
              ./czechnames.c \
              ./czechoslovaknames.c \
              ./deadparam.c \
              ./decl.c \
              ./decl2.c \
              ./dkf5kEnum.c \
              ./dkf5kRange.c \
              ./dkf5kSprintf.c \
              ./ensures.c \
              ./enum.c \
              ./enumtag.c \
              ./exports.c \
              ./external.c \
              ./fields.c \
              ./fields2.c \
              ./fields3.c \
              ./flags.c \
              ./glob.c \
              ./forbody.c \
              ./format.c \
              ./freearray.c \
              ./funcpointer.c \
              ./globals.c \
              ./globals2.c \
              ./impabstract.c \
              ./init.c \
              ./innerfree.c \
              ./inparam.c \
              ./internal.c \
              ./iter.c \
              ./iter2.c \
              ./keep.c \
              ./libs.c \
              ./linked.c \
              ./linked2.c \
              ./linked3.c \
              ./linked4.c \
              ./linked5.c \
              ./linked6.c \
              ./lintcomments.c \
              ./list.c \
              ./longconstants.c \
              ./macros.c \
              ./macrosef.c \
	      ./malloc.c \
              ./merge.c \
              ./modclient.c \
              ./modifies.c \
              ./modtest.c \
              ./moduncon.c \
              ./mongoincludes.c \
              ./mparen.c \
              ./mut.c \
              ./mystrncat.c \
              ./noeffect.c \
              ./null1.c \
              ./null2.c \
              ./null3.c \
              ./null4.c \
              ./null5.c \
              ./db2/dbase.c \
              ./db2/drive.c \
              ./db2/employee.c \
              ./db2/empset.c \
              ./db2/erc.c \
              ./db2/eref.c \
              ./db2/ereftab.c \
              ./db2/etest.c \
              ./null6.c \
              ./nullassign.c \
              ./nullret.c \
              ./observer.c \
              ./oldstyle.c \
              ./outglob.c \
              ./outparam.c \
              ./postnotnull.c \
              ./preds.c \
              ./prefixes.c \
              ./printflike.c \
              ./rc.c \
              ./refcounts.c \
              ./release.c \
              ./repexpose.c \
              ./returned.c \
              ./sharing1.c \
              ./sharing2.c \
              ./sharing3.c \
              ./sharing4.c \
              ./sharing5.c \
              ./sizeof.c \
              ./slovaknames.c \
              ./specclauses.c \
              ./specclauses2.c \
              ./specclauses3.c \
              ./specclauses4.c \
              ./specclauses5.c \
              ./specclauses6.c \
              ./special.c \
              ./stack.c \
              ./staticarray.c \
              ./strings.c \
              ./structassign.c \
              ./switch.c \
              ./t1.c \
              ./test.c \
              ./db3/dbase.c \
              ./db3/drive.c \
              ./db3/employee.c \
              ./db3/empset.c \
              ./db3/erc.c \
              ./db3/eref.c \
              ./db3/ereftab.c \
              ./typequals.c \
              ./ud.c \
              ./ud2.c \
              ./ullint.c \
              ./ulstypes.c \
              ./union.c \
              ./unreachable.c \
              ./unsignedcompare.c \
              ./unnamedsu.c \
              ./unused.c \
              ./void.c \
              ./conditions/miroslaw.c \
              ./conditions/releases.c \
              ./fileio/file.c \
              ./fileio/filebranch.c \
              ./fileio/filerw.c \
              ./for/for.c \
              ./globalbufferannotation/globalvariable.c \
              ./maxset/maxsetannotations.c \
              ./maxset/maxsetnoannotations.c \
              ./mergestate/taintednm.c \
              ./metastate/file.c \
              ./metastate/file1.c \
              ./metastate/file2.c \
              ./metastate/file3.c \
              ./metastate/file4.c \
              ./metastate/file5.c \
              ./metastate/file6.c \
              ./metastate/file7.c \
              ./metastate/filebad.c \
              ./metastate/global.c \
              ./metastate/nullbranch.c \
              ./metastate/nullbranch2.c \
              ./metastate/nullret.c \
              ./metastate/osd.c \
              ./metastate/sockets.c \
              ./metastate/sockets2.c \
              ./metastate/struct.c \
              ./metastate/test.c \
              ./metastate/voidptr.c \
              ./moreBufferTests/initialization.c \
              ./moreBufferTests/simplifyTest.c \
              ./moreBufferTests/strncatNotReallyGood.c \
              ./moreBufferTests/strncatReallyGood.c \
              ./moreBufferTests/strrchr.c \
              ./moreBufferTests/unrecogCall.c \
              ./nullterminatedtest/buggy1.c \
              ./nullterminatedtest/buggy_support1.c \
              ./nullterminatedtest/buggy_support_fmakeword.c \
              ./nullterminatedtest/test1.c \
              ./nullterminatedtest/test3.c \
              ./simplebufferConstraintTests/m.c \
              ./simplebufferConstraintTests/sizeof.c \
              ./simplebufferConstraintTests/test3.c \
              ./simplebufferConstraintTests/test7.c \
              ./sizeoftest/sizeof.c \
              ./sizeoftest/sizeofConst.c \
              ./sizeoftest/Makefile \
              ./strchr/strchr.c \
              ./suppressfile/test.c \
              ./tainted/sprintf.c \
              ./tainted/t1.c \
              ./tainted/tainted.c \
              ./tainted/tainted2.c \
              ./tainted/tainted3.c \
              ./tainted/tainted4.c \
              ./tainted/tainted5.c \
              ./tainted/taintedimplicit.c \
              ./tainted/taintedmerge.c \
              ./tainted/taintedx.c \
              ./tainted/test.c \
              ./tclauses/globals.c \
              ./tclauses/gt.c \
              ./tclauses/modifies.c \
              ./tclauses/struct.c \
              ./tclauses/undef.c \
              ./tests2.2/arbints.c \
              ./tests2.2/arrayfcn.c \
              ./tests2.2/booldef.c \
              ./tests2.2/boolenum.c \
              ./tests2.2/boolops.c \
              ./tests2.2/break.c \
              ./tests2.2/bstring.c \
              ./tests2.2/decl.c \
              ./tests2.2/enumbool.c \
              ./tests2.2/extension.c \
              ./tests2.2/libraries.c \
              ./tests2.2/modarray.c \
              ./tests2.2/nestext.c \
              ./tests2.2/offsetof.c \
              ./tests2.2/posix.c \
              ./tests2.2/realloc.c \
              ./tests2.2/rex.c \
              ./tests2.2/sizeofarray.c \
              ./tests2.2/struct.c \
              ./tests2.2a/addassign.c \
              ./tests2.2a/arrayparam.c \
              ./tests2.2a/bitops.c \
              ./tests2.2a/boolcomp.c \
              ./tests2.2a/boolenum.c \
              ./tests2.2a/dobb.c \
              ./tests2.2a/duff.c \
              ./tests2.2a/erik.c \
              ./tests2.2a/floatdouble.c \
              ./tests2.2a/florian.c \
              ./tests2.2a/fred.c \
              ./tests2.2a/isalpha.c \
              ./tests2.2a/notreached.c \
              ./tests2.2a/obviousloop.c \
              ./tests2.2a/popik.c \
              ./tests2.2a/sizeof.c \
              ./tests2.2a/toralf.c \
              ./tests2.4/subdir/main.c \
              ./tests2.4/alignof.c \
              ./tests2.4/array.c \
              ./tests2.4/bitfields.c \
              ./tests2.4/bug1.c \
              ./tests2.4/bug2.c \
              ./tests2.4/bug3.c \
              ./tests2.4/chin.c \
              ./tests2.4/cpptest.c \
              ./tests2.4/driverstub.c \
              ./tests2.4/duffs.c \
              ./tests2.4/emptycase.c \
              ./tests2.4/enumtest.c \
              ./tests2.4/error.c \
              ./tests2.4/fink.c \
              ./tests2.4/flagequal.c \
              ./tests2.4/forward.c \
              ./tests2.4/hash.c \
              ./tests2.4/hexconstants.c \
              ./tests2.4/innercomment.c \
              ./tests2.4/komazi.c \
              ./tests2.4/longlong.c \
              ./tests2.4/main.c \
              ./tests2.4/nothing.c \
              ./tests2.4/offsetof.c \
              ./tests2.4/print.c \
              ./tests2.4/ric.c \
              ./tests2.4/syncomment.c \
              ./tests2.4/syslog.c \
              ./tests2.4/test0.c \
              ./tests2.4/test1.c \
              ./tests2.4/test2.c \
              ./tests2.4/timecard.c \
              ./tests2.4/toothman.c \
              ./tests2.4/ulrich.c \
              ./tests2.5/badcomment.c \
              ./tests2.5/boolbad.c \
              ./tests2.5/boolt.c \
              ./tests2.5/booltest.c \
              ./tests2.5/dummyfile.c \
              ./tests2.5/hoof.c \
              ./tests2.5/immutable.c \
              ./tests2.5/impabsmodule.c \
              ./tests2.5/literals.c \
              ./tests2.5/quals.c \
              ./tests2.5/sort.c \
              ./tests2.5/testalt.c \
              ./tests2.5/testimmutable.c \
              ./tests2.5/uconstants.c \
              ./tests2.5/ull.c \
              ./warnuse/warnflags.c \
              ./warnuse/warngets.c \
              ./warnuse/warnuse.c \
              ./moreBufferTests2/unknownsize.c \
              ./moreBufferTests2/arrayConstExpr.c \
              abstptr.expect \
              abstract.expect \
              alias.expect \
              alttypes.expect \
              ansireserved.expect \
              argorder.expect \
              args.expect \
              arrayinit.expect \
              arraylit.expect \
              blocks.expect \
              branchstate.expect \
              break.expect \
              buffertest.expect \
              cases.expect \
              cast.expect \
              chararraylit.expect \
              charlit.expect \
              clauses.expect \
              commentchar.expect \
              compdestroy.expect \
              constannot.expect \
              controldepth.expect \
              csyntax.expect \
              czechnames.expect \
              czechoslovaknames.expect \
              db1.expect \
              db2.expect \
              db3.expect \
              deadparam.expect \
              decl.expect \
              enum.expect \
              enumtag.expect \
              exports.expect \
              external.expect \
              fields.expect \
              fileio.expect \
              flags.expect \
              for.expect \
              forbody.expect \
              format.expect \
              freearray.expect \
              funcpointer.expect \
              glob.expect \
              globalbufferannotation.expect \
              globals.expect \
              help.expect \
              impabstract.expect \
              init.expect \
              inparam.expect \
              internal.expect \
              iter.expect \
              keep.expect \
              libs.expect \
              linked.expect \
              lintcomments.expect \
              list.expect \
              longconstants.expect \
              macros.expect \
              macrosef.expect \
              maxset.expect \
              merge.expect \
              mergestate.expect \
              metastate.expect \
              modifies.expect \
              modtest.expect \
              moduncon.expect \
              mongoincludes.expect \
              moreBufferTests.expect \
              moreBufferTests2.expect \
              mut.lh.expect \
              mystrncat.expect \
              noeffect.expect \
              null.expect \
              nullterminatedtest.expect \
              observer.expect \
              oldstyle.expect \
              outglob.expect \
              outparam.expect \
              postnotnull.expect \
              preds.expect \
              prefixes.expect \
              printflike.expect \
              rc.expect \
              refcounts.expect \
              release.expect \
              repexpose.expect \
              repexpose.lh.expect \
              returned.expect \
              sharing.expect \
              simplebufferConstraintTests.expect \
              sizeof.expect \
              sizeoftest.expect \
              slovaknames.expect \
              specclauses.expect \
              special.expect \
              stack.expect \
              staticarray.expect \
              strchr.expect \
              strings.expect \
              structassign.expect \
              tainted.expect \
              tests2.2.expect \
              tests2.2a.expect \
              tests2.3.expect \
              tests2.4.expect \
              tests2.5.expect \
              typequals.expect \
              ud.expect \
              ullint.expect \
              ulstypes.expect \
              union.expect \
              unnamedsu.expect \
              unreachable.expect \
              unsignedcompare.expect \
              unused.expect \
              void.expect \
              ./db1/bool.h \
              ./db1/dbase.h \
              ./db1/employee.h \
              ./db1/empset.h \
              ./db1/erc.h \
              ./db1/eref.h \
              ./db1/ereftab.h \
              ./bool.h \
              ./decl2.h \
              ./exports.h \
              ./iter.h \
              ./iter2.h \
              ./minc1.h \
              ./minc2.h \
              ./minc3.h \
              ./minc4.h \
              ./minc5.h \
              ./modifies.h \
              ./mut.h \
              ./db2/bool.h \
              ./db2/dbase.h \
              ./db2/employee.h \
              ./db2/empset.h \
              ./db2/erc.h \
              ./db2/eref.h \
              ./db2/ereftab.h \
              ./pivo.h \
              ./repexpose.h \
              ./db3/bool.h \
              ./db3/dbase.h \
              ./db3/employee.h \
              ./db3/empset.h \
              ./db3/erc.h \
              ./db3/eref.h \
              ./db3/ereftab.h \
              ./tq.h \
              ./metastate/global.h \
              ./suppressfile/test.h \
              ./tests2.2/mbool.h \
              ./tests2.4/forward.h \
              ./tests2.4/hash.h \
              ./tests2.4/timecard.h \
              ./tests2.5/baz.h \
              ./tests2.5/bimbim.h \
              ./tests2.5/immutable.h \
              ./tests2.5/impabsmodule.h \
              ./tests2.5/socket.h \
              ./Makefile.os2 \
              ./db1/Makefile \
              ./db2/Makefile \
              ./db3/Makefile \
              ./conditions/Makefile \
              ./fileio/Makefile \
              ./for/Makefile \
              ./globalbufferannotation/Makefile \
              ./maxset/Makefile \
              ./mergestate/Makefile \
              ./metastate/Makefile \
              ./moreBufferTests/Makefile \
              ./simplebufferConstraintTests/Makefile \
              ./strchr/Makefile \
              ./tainted/Makefile \
              ./temp/Makefile \
              ./tests2.2/Makefile \
              ./tests2.2/Makefile-tests2.2.os2 \
              ./tests2.4/Makefile \
              ./tests2.4/Makefile-tests2.4.os2 \
              ./tests2.5/Makefile \
              ./warnuse/Makefile \
              ./Makefile.am \
              ./Makefile.in \
              ./moreBufferTests2/Makefile \
              ./fileio/eof.mts ./fileio/file.mts ./fileio/filerw.mts ./mergestate/tainted.mts ./mergestate/taintednm.mts ./metastate/file.mts ./metastate/nullterminated.mts ./metastate/sockets.mts ./tainted/tainted-bad.mts ./tainted/tainted.mts ./fileio/file.xh ./fileio/filerw.xh ./mergestate/tainted.xh ./mergestate/taintednm.xh ./metastate/file.xh ./tainted/tainted.xh \
                db2.old-expect   union.pp warnuse.old-expect \
               ./tests2.5/badcomment  ./tests2.5/boolbad  ./tests2.5/booltest   ./tests2.5/uconstants  loopexec.expect mergenull.expect shifts.expect looptesteffect.expect \
                unioninit.expect  unioninit.c utypes.c \
                utypes.expect widestrings.expect  widestrings.c  \
                functionmacro.expect functionmacro.c  info.c info.expect \
                longint.c loopexec.c looptesteffect.c mergenull.c shifts.c \
		longint.expect nullret.expect numabstract.expect \
		sizesigns.expect typeof.expect \
                manual.expect ./manual/Makefile \
                ./manual/annotglobs.c  ./manual/bool.c  ./manual/bool.h  ./manual/clauses.c  ./manual/employee.h  ./manual/exposure.c  ./manual/globals.c  ./manual/ignore.c  ./manual/implicit.c  ./manual/intSet.h  ./manual/list.c  ./manual/loop.c  ./manual/macros.c  ./manual/modify.c  ./manual/mstring.c  ./manual/mstring.h  ./manual/mstringnn.c  ./manual/multiError.c  ./manual/names.c  ./manual/noeffect.c  ./manual/null.c  ./manual/only.c  ./manual/order.c  ./manual/palindrome.c  ./manual/palindrome.h  ./manual/refs.c  ./manual/returned.c  ./manual/rgb.c  ./manual/rstring.c  ./manual/rstring.h  ./manual/sample.c  ./manual/setChar.c  ./manual/setname.c  ./manual/setname.h  ./manual/special.c  ./manual/stack.c  ./manual/sumsquares.c  ./manual/switch.c  ./manual/testpal.c  ./manual/types.c  ./manual/unique.c  ./manual/usedef.c  ./manual/bounds.c \
                  arraydims.expect arraydims.c \
                  moreBufferTests2/fixedArrayType.c oldstyle2.c \
                  divzero.expect parentype.expect \
                  divzero.c parentype.c \
stringliteral.expect  stringliteral.c \
  numabstract.c sizesigns.c typeof.c   nullassign.expect 

#		sizesigns.c sizesigns.expect \
