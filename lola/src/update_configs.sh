#!/bin/bash

FILELIST=`ls configs`

echo -n "EXTRA_PROGRAMS = " > Makefile.configs
for FILE in $FILELIST
do
  echo -n "bin-lola-$FILE " >> Makefile.configs
done

echo >> Makefile.configs
echo >> Makefile.configs

for FILE in $FILELIST
do
  ESCAPEDNAME=`echo "lola-$FILE" | sed -e 's/-/_/g'`

  printf "bin_%s_SOURCES = \$(lola_SOURCES)\n" $ESCAPEDNAME >> Makefile.configs
  printf "bin_%s_CPPFLAGS = \$(AM_CPPFLAGS)\n" $ESCAPEDNAME >> Makefile.configs

  printf "lola-%s:\n\t\$(AM_V_at)\$(MAKE) bin-lola-%s \$(AM_MAKEFLAGS) DEFAULT_INCLUDES=\"-I \$(srcdir)/configs/%s \$(DEFAULT_INCLUDES)\" CPPFLAGS=-DSTANDARDCONFIG\n" $FILE $FILE $FILE >> Makefile.configs
  printf "\t\$(AM_V_GEN)mv \$(builddir)/bin-lola-%s \$(builddir)/lola-%s\n\n" $FILE $FILE >> Makefile.configs
done
