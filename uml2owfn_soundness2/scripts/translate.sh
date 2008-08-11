#! /bin/bash
rm translate-err.log
rm uml2owfn_translate.csv

printf "NET;CORRECT SYNTAX;SYNTAX ERROR\n" >> uml2owfn_translate.csv

time sh ./translate-library.sh a1
time sh ./translate-library.sh a2
time sh ./translate-library.sh a3
time sh ./translate-library.sh a4
time sh ./translate-library.sh b1
time sh ./translate-library.sh b2
time sh ./translate-library.sh b3
time sh ./translate-library.sh b4
time sh ./translate-library.sh m1
time sh ./translate-library.sh p1

echo done
