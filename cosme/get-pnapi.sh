#!/usr/bin/env bash

# This script gets the most recent version of the Petri Net API from
# "http://esla.informatik.uni-rostock.de:8080/job/pnapi/lastSuccessfulBuild/artifact/pnapi/pnapi.tar.gz"
# and will install it in the relative directory "./libs/pnapi/".

########################################################################
# IMPORTANT: Call this script from the customer tool's root directory! #
########################################################################

# print information
echo
echo "  This script will get the most recent version of the"
echo "  Petri Net API from 'esla.informatik.uni-rostock.de'"
echo "  and will install it in the relative directory"
echo "  './libs/pnapi/' using the tools 'wget' and 'tar'."
echo
echo "  If a previous version of the PNAPI is alreadey installed,"
echo "  it will be removed before installing the newer one."
echo "  If not, all needed directories will be created."
echo
echo "  This script WILL NOT touch any subversion information ('.svn')"
echo "  in the directory './libs/pnapi'."
echo "  You have to take care of this by yourself."
echo
echo

# wait for key pressed
read -n 1 -p "    Press any key to continue"
echo


# create "libs/pnapi" directory if it does not exists
mkdir -p ./libs/pnapi

# remove possible previous version of the api
rm -rf ./libs/pnapi/*

# get last successful build of the API from esla
cd ./libs/pnapi/
wget http://esla.informatik.uni-rostock.de:8080/job/pnapi/lastSuccessfulBuild/artifact/pnapi/pnapi.tar.gz
tar xfz pnapi.tar.gz

# copy relevant files
cp pnapi/src/*.cc pnapi/src/*.h pnapi/src/*.ll pnapi/src/*.yy .
cp pnapi/src/Makefile.am.customer Makefile.am

# cleanup
rm -rf pnapi
rm -rf pnapi.tar.gz
rm -rf parser-*.cc
rm -rf parser-*.h

# tell user we have finished
echo
echo "  Finished installing PNAPI"
echo "  Please check, whether all files are versioned properly in your tool."
echo

