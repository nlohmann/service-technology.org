#!/usr/bin/env bash

# This script gets the most recent version of the
# Petri Net API from "svn://svn.gna.org/service-tech/trunk/pnapi"
# and will install it in the relative directory "./libs/pnapi/".

########################################################################
# IMPORTANT: Call this script from the customer tool's root directory! #
########################################################################

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
