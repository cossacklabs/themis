#!/bin/sh

#This script is intended to be called in make dist
#THis is intended to be run in the distribution directory...

pwd
#echo doing bash
#bash

#Need to do this so modifications to the destination directory won't effect
#the real directory 
rm Makefile.am
rm configure.ac

mv  Makefile.binary.am Makefile.am
mv  configure.binary.ac configure.ac
sleep 3
mv configure.binary configure


#should probably do this in Makefile but it doesn't seem to work..
sed -e 's/Makefile\.binary/Makefile/' Makefile.binary.in  > Makefile.in
rm Makefile.binary.in
#run these if possible...
#autoconf
#automake
mkdir src || echo "COULD NOT MAKE src tar ball may be incorrect"
echo "Created this file because some versions of tar are too simple include this directory without it"  > src/dummy

# Need to do this in the makefile because the tar command in make dist wants to derefence symlinks
#ln -s ../bin/splint src/splint
