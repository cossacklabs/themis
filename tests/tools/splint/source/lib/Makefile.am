## Note: starting comments with ## means they don't end up in Makefile

### Make phony, always remake everything.

.PHONY: all standard.lcd standardstrict.lcd posix.lcd posixstrict.lcd unix.lcd unixstrict.lcd

AUTOMAKE_OPTIONS = 1.5 foreign

## Put these files in $prefix/share/splint/lib
splintlibdir = $(pkgdatadir)/lib

UnixHeaders = stdio.h stdlib.h

## List of files
splintlib_DATA = \
  standard.h standard.lcd standardstrict.lcd posix.h posix.lcd posixstrict.lcd \
  unix.h unix.lcd unixstrict.lcd CTrait.syms CTraitGen.lcl bool.h \
  file.mts file.xh filerw.mts filerw.xh \
  lclinit.lci linux.h lslinit.lsi tainted.mts tainted.xh \
  stdio.h stdlib.h $(UnixHeaders)

## Include them in the distribution
EXTRA_DIST = $(splintlib_DATA)

##get rid of lcd files if the user does a make clean
CLEANFILES = posix.lcd standard.lcd unix.lcd \
             posixstrict.lcd  standardstrict.lcd  unixstrict.lcd


## Rules to make splint & the dumps

SPLINT = $(top_builddir)/src/splint$(EXEEXT)

$(SPLINT):
	cd $(top_builddir)/src; $(MAKE)

all: standard.lcd standardstrict.lcd posix.lcd posixstrict.lcd unix.lcd unixstrict.lcd

standard.lcd: standard.h $(SPLINT)
	-$(SPLINT) -nof -nolib +impconj standard.h -dump standard
	@touch $@

standardstrict.lcd: standard.h $(SPLINT)
	-$(SPLINT) -nof -nolib +impconj -DSTRICT standard.h -dump standardstrict
	@touch $@

posix.lcd: standard.h posix.h $(SPLINT)
	-$(SPLINT) -nof -nolib +impconj standard.h posix.h -dump posix
	@touch $@

posixstrict.lcd: standard.h posix.h $(SPLINT)
	-$(SPLINT) -nof -nolib +impconj -DSTRICT standard.h posix.h -dump posixstrict
	@touch $@

unix.lcd: standard.h posix.h unix.h $(UnixHeaders) $(SPLINT)
	-$(SPLINT) -supcounts -nof -incondefs -nolib +impconj standard.h posix.h unix.h stdio.h stdlib.h -dump unix
	@touch $@

unixstrict.lcd: standard.h posix.h unix.h $(UnixHeaders) $(SPLINT)
	-$(SPLINT) -supcounts -nof -incondefs -nolib +impconj -DSTRICT standard.h posix.h unix.h  stdio.h stdlib.h  -dump unixstrict
	@touch $@
