#/bin/sh

cd src

for TOOL in `cat ../tools`
do
  wget --timestamping http://esla.informatik.uni-rostock.de:8080/job/$TOOL/lastSuccessfulBuild/artifact/$TOOL/$TOOL.tar.gz
done

cd ..