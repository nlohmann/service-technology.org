#!/usr/bin/env bash

# This script gets the most recent version of the
# Petri Net API from "svn://svn.gna.org/service-tech/trunk/pnapi"
# and will install it in the relative directory "./libs/pnapi/".

########################################################################
# IMPORTANT: Call this script from the customer tool's root directory! #
########################################################################

# print information
echo
echo "  This script will get the most recent version of the"
echo "  Petri Net API from 'svn://svn.gna.org/service-tech/trunk/pnapi'"
echo "  and will install it in the relative directory './libs/pnapi/'."
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

# get recent API from the repository to temporary directory
# (Done due to .svn information)
svn co svn://svn.gna.org/service-tech/trunk/pnapi/src ./libs/pnapi-tmp

# copy API
cp ./libs/pnapi-tmp/* ./libs/pnapi/

# remove temporary directory
rm -rf ./libs/pnapi-tmp

# replace original Makefile.am by the one designed for customer tools
mv -f ./libs/pnapi/Makefile.am.customer ./libs/pnapi/Makefile.am

# tell user we have finished
echo
echo "  Finished installing PNAPI"
echo "  Please check, whether all files are versioned properly in your tool."
echo

