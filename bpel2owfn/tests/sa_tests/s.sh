# /bin/bash

for FILE in `ls -1 *.bpel`
do
  SANAME=`echo $FILE | sed  -e 's/.bpel//;s/sa//'`
  TESTFILENAME="test-staticanalysis-sa$SANAME"

  echo "# /bin/bash" > $TESTFILENAME
  echo >> $TESTFILENAME
  echo "command=\"bpel2owfn -mcfg --input=\$testdir/sa_tests/$FILE\"" >> $TESTFILENAME
  echo "echo \"\"" >> $TESTFILENAME
  echo "echo \"test: static analysis test [SA$SANAME]\"" >> $TESTFILENAME
  echo "echo \"      \$command\"" >> $TESTFILENAME
  echo "\$command &> /dev/null" >> $TESTFILENAME

  chmod a+x $TESTFILENAME
done