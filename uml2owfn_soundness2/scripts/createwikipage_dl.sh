#! /bin/bash

for DIRECTORY in `ls -1 -d */`
do
    cd $DIRECTORY

    LOLA_FILE_COUNT=`ls *.lola | wc -l`
    SOUNDNESS_COUNT=0
    EXCEEDED_COUNT=0
    FALSE_COUNT=0

    for LOLA_FILE in `find *.lola`
    do
        printf "PROCESSING FILE %s\n" $LOLA_FILE
        printf "PROCESSING FILE %s\n" $LOLA_FILE >> ../lola-result.txt

        start_time="$(date +%s)"
        lola-dl $LOLA_FILE >> ../lola-result.txt 2>> ../lola-result.txt

        res=$?

        if [ $res = 4 ]; then
            printf "0 Places\n0 Transitions\nresult: nonsafe\n>>>>> 0 States, 0 Edges\n" >> ../lola-result.txt
            let EXCEEDED_COUNT=$EXCEEDED_COUNT+1
        else
            if [ $res = 0 ]; then
                let SOUNDNESS_COUNT=$SOUNDNESS_COUNT+1
                printf "result: false\n" >> ../lola-result.txt
            else
                let FALSE_COUNT=$FALSE_COUNT+1
                printf "result: true\n" >> ../lola-result.txt
            fi
        fi

        printf "FINISHED WITH RETURN VALUE %d\n" $res >> ../lola-result.txt
        end_time="$(date +%s)"
        time_diff=`expr $end_time - $start_time`
        printf "RUNTIME: %u seconds\n" $time_diff >> ../lola-result.txt
    done

    cd ..

    printf "%s: %d nets - %d sound - %d unsound - %d exceeded\n" $DIRECTORY $LOLA_FILE_COUNT $SOUNDNESS_COUNT $EXCEEDED_COUNT $FALSE_COUNT >> global.txt
done

echo "NET" > result-names.txt
grep "PROCESSING FILE" lola-result.txt | awk '{print $3}' >> result-names.txt

echo "PLACES" > result-places.txt
grep "Places" lola-result.txt | awk '{print $1}' >> result-places.txt

echo "TRANSITIONS" > result-transitions.txt
grep "Transitions" lola-result.txt | awk '{print $1}' >> result-transitions.txt

echo "SOUND" > result-soundness.txt
grep "result:" lola-result.txt | awk '{print $2}' >> result-soundness.txt

echo "STATES" > result-states.txt
grep "States" lola-result.txt | awk '{print $2}' >> result-states.txt

echo "EDGES" > result-edges.txt
grep "Edges" lola-result.txt | awk '{print $4}' >> result-edges.txt

echo "TIME" > result-time.txt
grep "RUNTIME:" lola-result.txt | awk '{print $2}' >> result-time.txt

paste -d';' result-names.txt result-places.txt result-transitions.txt result-soundness.txt result-states.txt result-edges.txt result-time.txt > result.csv

paste -d'|' result-names.txt result-places.txt result-transitions.txt result-soundness.txt result-states.txt result-edges.txt result-time.txt | sed 's/|/ | /g;' | awk '{ printf ("| %s |\n", $0) }' | sed 's/| NET | PLACES | TRANSITIONS | SOUND | STATES | EDGES | TIME |/^ NET ^ PLACES ^ TRANSITIONS ^ SOUND ^ STATES ^ EDGES ^ TIME ^/;' | sed 's/.lola//;' | sed 's/__/--/;' > result.wiki


rm result-names.txt result-places.txt result-transitions.txt result-soundness.txt result-states.txt result-edges.txt result-time.txt lola-result.txt
