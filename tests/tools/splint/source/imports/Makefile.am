## Note: starting comments with ## means they don't end up in Makefile

AUTOMAKE_OPTIONS = 1.5 foreign

## Put these files in $prefix/share/lclint/imports
importsdir = $(pkgdatadir)/imports

## List of files
imports_DATA = \
  assert.lcl  assert.lcs  ctype.lcl  ctype.lcs  errno.lcl  errno.lcs  \
  limits.lcl  limits.lcs  locale.lcl locale.lcs math.lcl   math.lcs   \
  setjmp.lcl  setjmp.lcs  signal.lcl signal.lcs stdarg.lcl stdarg.lcs \
  stdio.lcl   stdio.lcs   stdlib.lcl stdlib.lcs string.lcl string.lcs \
  strings.lcl strings.lcs time.lcl   time.lcs

## Include them in the distribution
EXTRA_DIST = $(imports_DATA)
