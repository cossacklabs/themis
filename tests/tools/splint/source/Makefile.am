## Process this file with automake to create Makefile.in. -*-Makefile-*-
##
## Makefile for Splint 3.0
## For more information: http://www.splint.org
##
## Copyright (C) 2001-2 University of Virginia,
##         Massachusetts Institute of Technology
##
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by the
## Free Software Foundation; either version 2 of the License, or (at your
## option) any later version.
## 
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## The GNU General Public License is available from http://www.gnu.org/ or
## the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
## MA 02111-1307, USA.
##
## For information on spint: info@splint.org
## To report a bug: splint-bug@splint.org
## 

.PHONY: test

AUTOMAKE_OPTIONS = 1.5 foreign

binaryfixscript = ./fixBinaryDist.sh
SUBDIRS =   src lib imports test doc

binaryDir = bin
binaryDirFiles =  $(binaryDir)/Makefile.am $(binaryDir)/Makefile.in

binaryBuiltFiles = Makefile.binary.am configure.binary.ac

#include these in case we're trying to build on a systems
#without the bleeding edge versions of automake and autoconf
binaryDerivedFiles = Makefile.binary.in configure.binary

#try to remake these but don't stop if it fails.
configure.binary:configure.binary.ac
	-autoconf -oconfigure.binary configure.binary.ac

Makefile.binary.in:Makefile.binary.am
	-automake Makefile.binary
	$(SED) -e s/Makefile\.binary/Makefile/ Makefile.binary.in > tMakefile.binary.in
#	$(RM) Makefile.binary.in
	$(CP) tMakefile.binary.in Makefile.binary.in
#drl add this hack 3/1/2003
#make the generated makefile depend on Makefile not Makefile.binary

$(binaryDir)/Makefile.in: $(binaryDir)/Makefile.am
	-automake $(binaryDir)/Makefile

EXTRA_DIST = $(binaryfixscript) $(binaryBuiltFiles) $(binaryDerivedFiles) $(binaryDirFiles) install.html

preDist:  $(binaryDir)/Makefile.in  $(binaryDerivedFiles) 

bindist: $(binaryDerivedFiles) 
	$(MAKE)
	$(RM) -f $(binaryDir)/splint
	$(CP) src/splint $(binaryDir)/splint
	$(MV) install.html installSource.html
	$(CP) doc/` uname | tr [A-Z] [a-z]`.html install.html
	$(MAKE) -e dist dh_script=$(binaryfixscript) SUBDIRS="$(binaryDir) lib imports  doc"
	$(MV) $(distdir).tar.gz $(distdir).`uname`.tgz
	$(MV) installSource.html install.html

dist-hook:
	cd $(distdir); 	pwd; $(dh_script)


