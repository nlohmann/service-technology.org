############################################################################
# Copyright 2007 Jan Bretschneider                                         #
#                                                                          #
# This file is part of Fiona.                                              #
#                                                                          #
# Fiona is free software; you can redistribute it and/or modify it         #
# under the terms of the GNU General Public License as published by the    #
# Free Software Foundation; either version 2 of the License, or (at your   #
# option) any later version.                                               #
#                                                                          #
# Fiona is distributed in the hope that it will be useful, but WITHOUT     #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    #
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for #
# more details.                                                            #
#                                                                          #
# You should have received a copy of the GNU General Public License along  #
# with Fiona; if not, write to the Free Software Foundation, Inc., 51      #
# Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                     #
############################################################################

# Allows test scrips to be directly called from the command line. Set all unset
# environment variables that are normally set by the Makefile.

source ../env.sh

if [ "$testdir" == "" ]; then
    testdir=.
fi

if [ "$builddir" == "" ]; then
    builddir=.
fi

if [ "$MKDIR_P" == "" ]; then
    MKDIR_P="mkdir -p"
fi
