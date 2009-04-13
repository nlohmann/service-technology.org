#!/usr/bin/env bash

####################################################
# Test suit for reduction rules                    #
# See execute_tests() if you want to add own tests #
####################################################

DIR="${top_builddir}"

# call_petri(owfn,preP,preT,preA,poP,poT,poA,rules)
call_petri() {
  OWFN="$1"
  PREPLACES="$2"
  PRETRANSITIONS="$3"
  PREARCS="$4"
  POSTPLACES="$5"
  POSTTRANSITIONS="$6"
  POSTARCS="$7"
  RULES="$8"

  PRESTRING=`${DIR}/utils/petri -v "${OWFN}" 2>&1 | sed -e "s/.*\.net\.owfn: |P|= \([0-9]*\) .* |T|= \([0-9]*\)  |F|= \([0-9]*\).*/\1 \2 \3/"` 

  ${DIR}/utils/petri "${OWFN}" -r"${RULES}" -oowfn &> /dev/null

  POSTSTRING=`${DIR}/utils/petri -v "${OWFN}.reduced.owfn" 2>&1 | sed -e "s/.*\.reduced\.owfn: |P|= \([0-9]*\) .* |T|= \([0-9]*\)  |F|= \([0-9]*\).*/\1 \2 \3/"`
  
  if [ `echo "${PRESTRING}" | awk '{print $1}'` != "${PREPLACES}" ] 
      then return 1
  fi
  
  if [ `echo "${PRESTRING}" | awk '{print $2}'` != "${PRETRANSITIONS}" ] 
      then return 1
  fi
   
  if [ `echo "${PRESTRING}" | awk '{print $3}'` != "${PREARCS}" ] 
      then return 1
  fi
  
  if [ `echo "${POSTSTRING}" | awk '{print $1}'` != "${POSTPLACES}" ] 
      then return 1
  fi
   
  if [ `echo "${POSTSTRING}" | awk '{print $2}'` != "${POSTTRANSITIONS}" ] 
      then return 1
  fi
   
  if [ `echo "${POSTSTRING}" | awk '{print $3}'` != "${POSTARCS}" ] 
      then return 1
  fi

  return 0
}


execute_tests() {

RESULT=0

echo "Testing reduction rules"

printf "Reduce: Dead nodes..."
call_petri "./test_dead_nodes.net.owfn" "3" "3" "6" "2" "2" "4" "dead_nodes"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

echo "Reduce: Equal places...SKIPPED"

printf "Reduce: Identical places..."
call_petri "./test_identical_places.net.owfn" "12" "6" "22" "11" "6" "16" "identical_places"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Identical transitions..."
call_petri "./test_identical_transitions.net.owfn" "6" "8" "18" "6" "7" "12" "identical_transitions"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

echo "Reduce: Initially marked places in choreographies...SKIPPED"

printf "Reduce: Self-loop places..."
call_petri "./test_self_loop_places.net.owfn" "3" "1" "4" "2" "1" "2" "self_loop_places"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Self-loop transitions..."
call_petri "./test_self_loop_transitions.net.owfn" "1" "3" "4" "1" "2" "2" "self_loop_transitions"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Series places..."
call_petri "./test_series_places.net.owfn" "5" "7" "11" "4" "6" "9" "series_places"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Series transitions..."
call_petri "./test_series_transitions.net.owfn" "5" "2" "6" "4" "1" "4" "series_transitions"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Series transitions[Keepnormal=off]..."
call_petri "./test_series_transitions_keepnormal.net.owfn" "5" "2" "6" "4" "1" "4" "series_transitions"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Series transitions[Keepnormal=on]..."
call_petri "./test_series_transitions_keepnormal.net.owfn" "5" "2" "6" "5" "2" "6" "series_transitions,keep_normal"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi



printf "Reduce: Starke 3 for places..."
call_petri "./test_starke_3p.net.owfn" "2" "3" "6" "1" "3" "3" "starke3p"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Starke 3 for transitions..."
call_petri "./test_starke_3t.net.owfn" "5" "2" "10" "5" "1" "5" "starke3t"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Starke 4..."
call_petri "./test_starke_4.net.owfn" "4" "4" "8" "3" "3" "6" "starke4"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Starke 5..."
call_petri "./test_starke_5.net.owfn" "4" "4" "9" "3" "3" "7" "starke5"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Starke 6..."
call_petri "./test_starke_6.net.owfn" "5" "5" "12" "4" "4" "10" "starke6"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Starke 7..."
call_petri "./test_starke_7.net.owfn" "3" "2" "6" "2" "1" "2" "starke7"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Starke 8..."
call_petri "./test_starke_8.net.owfn" "4" "2" "10" "4" "1" "4" "starke8"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

printf "Reduce: Starke 9..."
call_petri "./test_starke_9.net.owfn" "5" "5" "15" "4" "4" "12" "starke9,once"
RET=$?
RESULT=`expr ${RESULT} + ${RET}`
if [ ${RET} != 0 ]
    then printf "FAIL\n"
  else printf "OK\n"
fi

echo "Reduce: Suspicious transitions...SKIPPED"
echo "Reduce: Unused status places...SKIPPED"

echo "Finished testing: ${RESULT} tests failed."

return ${RESULT}

}

execute_tests

