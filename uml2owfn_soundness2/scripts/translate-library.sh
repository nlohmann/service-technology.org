#! /bin/bash
echo Translating ${1}
cd lib_${1}
###########
##
#UML2OWFN -- Translating UML Processes into Petri Net Models
#
#Usage: uml2owfn [OPTION]
#
#Options:
# -p, --parameter=PARAM  modify processing with given parameter
# -i, --input=FILE       read a BPEL process from FILE
# -o, --output[=NAME]    write output to file (NAME sets filename)
# -f, --format=FORMAT    create output of the given format
# -d, --debug=NUMBER     set a debug level (NUMBER=0..4 or "flex" or "bison")
# -r, --reduce=NUMBER    apply structural reduction level (NUMBER=0..5)
# -a, --analyze=TASK     analyze the process with the given task
# -h, --help             print this help list and exit
# -v, --version          print program version and exit
#
#  PARAMETER is one of the following (multiple parameters permitted):
#    filter              filter out infeasible processes from the library
#    log                 write a log file for the translation
#    taskfile            write analysis task to a separate file#
#
#  FORMAT is one of the following (multiple formats permitted):
#    lola, owfn, dot, pep, tpn, apnn, ina, spin, info, pnml, txt, info
#
#  TASK is one of the following (multiple parameters permitted):
#    soundness           analyze for soundness
#    deadlocks           check for deadlocks (except in the final state),
#                        requires -a soundness
#    stop                distinguish stop nodes from end nodes
#    keeppins            keep unconnected pins
##
# suggested combinations:
# for soundness analysis with LoLA
# -f lola -a soundness -p filter -r <reduction-level> -o [-d <debug-level>]
#
# for generating all original nets full size
# -f <format> -o [-d <debug-level>]
#
# for generating useful png-graphics of the soundness-checked net,
# reduction level 1 removes all chains of places/transitions
# -f dot -a soundness -p filter -r 1 -o [-d <debug-level>]
#
###########
uml2owfn -i ${1}.xml -f lola -a soundness -p filter -p log -o -d 2 > translate.log 2> translate-err.log
# uml2owfn -i ${1}.xml -f lola -a soundness -p filter -p log -r 4 -o -d 2 > translate.log 2> translate-err.log
# uml2owfn -i ${1}.xml -f dot -p filter -r 1 -d 2 -o > translate.log 2> translate-err.log

echo ------- ${1} ------- >> ../translate-err.log
cat translate-err.log >> ../translate-err.log

cat uml2owfn_${1}.log >> ../uml2owfn_translate.csv
cd ..
