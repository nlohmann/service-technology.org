#!/bin/bash

cd ..

HOMEPWD=$(pwd)
COLLECTION=$(pwd)/_build/tools.json

echo -n "generating tool list..."
TOOLS=$(find * -name configure.ac | grep -v "_build" | sed 's|/configure.ac||')

echo "{\"tools\": [" > $COLLECTION

for TOOL in $TOOLS
do
  cd $TOOL
  TOOLNAME=`basename \`pwd\``
  echo "[$TOOLNAME] autogen..."
  autogen.sh --quiet

  echo "[$TOOLNAME] configure..."
  ./configure >& /dev/null

  echo "[$TOOLNAME] generating JSON file..."
  cp $HOMEPWD/_build/tool.json.template tool.json.in
  ./config.status --file=tool.json --quiet
  rm tool.json.in

  gsed -i "s|@TRUNK@|$TOOL|" tool.json
  TOOLID=$(echo $TOOL | sed 's@/@_@')
  gsed -i "s|@TOOLID@|$TOOLID|" tool.json

  LASTVERSION=$(curl -s http://download.gna.org/service-tech/$TOOLNAME/ | grep -oh "$TOOLNAME-[0-9.]*" | sort | uniq | sed "s/$TOOLNAME-//g" | sed 's/.$//g' | tail -n1)
  gsed -i "s|@OFFICIAL_VERSION@|$LASTVERSION|" tool.json

#  PURPOSE=$(sed -n '/PURPOSE/,/COPYRIGHT/p' README | sed '/PURPOSE/d;/COPYRIGHT/d' | sed 's/^[ \t]*//;s/[ \t]*$//')
#  gsed -i "s|@PURPOSE@|$PURPOSE|" tool.json
#  cat tool.json | perl -i -pe "s/@PURPOSE@/$PURPOSE/" > tool.json~
#  mv tool.json~ tool.json

  echo "[$TOOLNAME] cleaning..."
  make svn-clean >& /dev/null

  cat tool.json >> $COLLECTION
  rm tool.json

  cd $HOMEPWD
done

echo "]}" >> $COLLECTION

gsed -i '2,2d' $COLLECTION
