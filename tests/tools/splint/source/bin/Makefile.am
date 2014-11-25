## Note: starting comments with ## means they don't end up in Makefile

AUTOMAKE_OPTIONS = 1.6.3 foreign no-dependencies

SUFFIXES = .h .c .o .l .check

binaryDir = bin

## We only build Splint
bin_PROGRAMS = splint

.PHONY: all

EXTRA_DIST = splint

DIST_SOURCES =

#chessy hack so it doesn't try to build splint
splint$(EXEEXT): ;

install-exec-hook: $(top_builddir)/src/splint$(EXEEXT)

$(top_builddir)/src/splint$(EXEEXT): splint$(EXEEXT)
	ln -s ../$(binaryDir)/splint $(top_builddir)/src/splint$(EXEEXT)

#don't delete ./splint
clean: ;

