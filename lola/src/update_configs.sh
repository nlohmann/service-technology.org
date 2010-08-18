#!/bin/sh

# This script creates a file "Makefile.configs" which contains targets to
# build a preconfigured LoLA for each userconfig.H in the subdirectories of
# configs. Make sure that, once called, you commit the changed
# file "Makefile.configs".

FILELIST=`ls configs`

printf "EXTRA_PROGRAMS = " > Makefile.configs
for FILE in $FILELIST
do
  printf "bin-lola-$FILE " >> Makefile.configs
done

printf "\n\n" >> Makefile.configs

for FILE in $FILELIST
do
  ESCAPEDNAME=`echo "lola-$FILE" | sed -e 's/-/_/g'`

  printf "bin_%s_SOURCES = \$(lola_SOURCES)\n" $ESCAPEDNAME >> Makefile.configs
  printf "bin_%s_CPPFLAGS = \$(AM_CPPFLAGS)\n" $ESCAPEDNAME >> Makefile.configs

  printf "lola-%s:\n\t\$(AM_V_at)\$(MAKE) bin-lola-%s \$(AM_MAKEFLAGS) DEFAULT_INCLUDES=\"-I \$(srcdir)/configs/%s \$(DEFAULT_INCLUDES)\" CPPFLAGS=-DSTANDARDCONFIG\n" $FILE $FILE $FILE >> Makefile.configs
  printf "\t\$(AM_V_GEN)mv \$(builddir)/bin-lola-%s \$(builddir)/lola-%s\n\n" $FILE $FILE >> Makefile.configs
done
