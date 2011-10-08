#!/bin/sh

# JENKINS_URL = http://esla.informatik.uni-rostock.de/jenkins
# TOOL = Name des Tools für die Release

rm -fr *

wget $JENKINS_URL/job/$TOOL/lastSuccessfulBuild/artifact/$TOOL/$TOOL.tar.gz
tar xfz $TOOL.tar.gz
cd $TOOL
./configure
make all distcheck pdf || exit 1

TARBALLNAME=`ls -1 $TOOL-*.tar.gz`
gpg --detach $TARBALLNAME

echo "cd /upload/service-tech/$TOOL" > sftp-script
echo "-rm $TOOL.tar.gz" >> sftp-script
echo "ln $TARBALLNAME $TOOL.tar.gz" >> sftp-script
echo "exit" >> sftp-script

scp -C $TARBALLNAME $TARBALLNAME.sig ChangeLog doc/$TOOL.pdf niels@download.gna.org:/upload/service-tech/$TOOL
sftp -b sftp-script niels@download.gna.org
