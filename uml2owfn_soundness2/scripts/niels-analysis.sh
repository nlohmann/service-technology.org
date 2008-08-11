# /bin/sh

printf "Process name\n"                 > result_NAME.txt
printf "Correct syntax\n"               > result_STRUCTURE.txt
printf "Sound\n"                        > result_SOUNDNESS.txt
printf "States seen\n"                  > result_STATES.txt
printf "Edges seen\n"                   > result_EDGES.txt
printf "Transitions\n"                  > result_TRANSITIONS.txt
printf "Places\n"                       > result_PLACES.txt
printf "Reason for incorrrect syntax\n" > result_REASONS_SYNTAX.txt
printf "Reason for unsound behavior\n"  > result_REASONS_SOUNDNESS.txt

# printf "complete LoLA log\n" > niels-analysis_lola.log

for LOLA_FILE in `find *.lola`
do
    # convert the filename to a process name (danger, the prefix "b3" is hardcoded!)
    PROCESS_NAME=`printf "%s" $LOLA_FILE | sed -e 's/_/#/g;s/b3.//;s/.lola//'`

    printf "%s " $PROCESS_NAME
    printf "%s\n" $PROCESS_NAME >> result_NAME.txt
    
    # show if we find a formula in the LoLA file
    grep --quiet "FORMULA" $LOLA_FILE
    res=$?

    # analyze grep's return value
    if [ $res = 0 ]; then
        # a formula was found
        printf "true\n" >> result_STRUCTURE.txt
        
        # look for this process name in the translation log
        grep -C1 "$PROCESS_NAME" translate.log | grep --quiet "free-choice"
        res2=$?

        # analyze grep's return value
        if [ $res2 = 0 ]; then
            # we found "free choice"
            printf "net is not free choice\n" >> result_REASONS_SYNTAX.txt
        else
            # it must have been "workflow net"
            printf "net has no workflow structure\n" >> result_REASONS_SYNTAX.txt
        fi

        # call LoLA to check soundness
        lola-mc $LOLA_FILE > result_LOLA.txt 2> result_LOLA.txt
        res3=$?

        # analyze LoLA's return value
        case $res3 in
            0) #printf "true (mc:0)\n"
               printf "true\n"                      >> result_SOUNDNESS.txt 
               printf "\n"                          >> result_REASONS_SOUNDNESS.txt
               ;;
            1) #printf "false (mc:1)\n"
               printf "false\n"                     >> result_SOUNDNESS.txt
               printf "CTL formula does not hold\n" >> result_REASONS_SOUNDNESS.txt
               ;;
            4) #printf "false (mc:4)\n"
               printf "false\n"                     >> result_SOUNDNESS.txt
               printf "net is not safe\n"           >> result_REASONS_SOUNDNESS.txt
               ;;
            *) printf "RESULT %s\n"                 >> result_SOUNDNESS.txt $res
               printf "RESULT %s\n"                 >> result_REASONS_SOUNDNESS.txt $res
               ;;
        esac
        
    else
        # we analyze a free choice workflow net
        printf "true\n" >> result_STRUCTURE.txt
        printf "\n" >> result_REASONS_SYNTAX.txt

        # call LoLA to check for deadlocks
        lola-dl $LOLA_FILE > result_LOLA.txt 2> result_LOLA.txt
        res3=$?

        # analyze LoLA's return value
        case $res3 in
            # in LoLA-DL, return values are interchanged: 0 means found a deadlock
            0) #printf "false (dl:0)\n"
               printf "false\n"                     >> result_SOUNDNESS.txt 
               printf "net is not deadlock-free\n"  >> result_REASONS_SOUNDNESS.txt
               ;;
            1) #printf "true (dl:1)\n"
               printf "true\n"                      >> result_SOUNDNESS.txt
               printf "\n"                          >> result_REASONS_SOUNDNESS.txt
               ;;
            4) #printf "false (dl:4)\n"
               printf "false\n"                     >> result_SOUNDNESS.txt
               printf "net is not safe\n"           >> result_REASONS_SOUNDNESS.txt
               ;;
            *) printf "RESULT %s\n"                 >> result_SOUNDNESS.txt $res
               printf "RESULT %s\n"                 >> result_REASONS_SOUNDNESS.txt $res
               ;;
        esac
    fi
    
    # write the full log
    #printf "_____________ %s _____________\n" $PROCESS_NAME >> niels-analysis_lola.log
    #cat result_LOLA.txt >> niels-analysis_lola.log
    
    # in case no states were seen, we use the hash table entries instead
    STATES=`cat result_LOLA.txt | grep "States"             | awk '{print $2}'`
    HASH=`cat result_LOLA.txt   | grep "Hash table entries" | awk '{print $6}'`
    if [ $STATES = 0 ]; then
        STATES=$HASH
    fi
    
    cat result_LOLA.txt | grep "Places"      | awk '{print $1}' >> result_PLACES.txt
    cat result_LOLA.txt | grep "Transitions" | awk '{print $1}' >> result_TRANSITIONS.txt
    cat result_LOLA.txt | grep "Edges"       | awk '{print $4}' >> result_EDGES.txt
    printf "%d\n" $STATES >> result_STATES.txt

done

# create the table from the results
paste -d';' \
    result_NAME.txt result_STRUCTURE.txt \
    result_SOUNDNESS.txt \
    result_TRANSITIONS.txt result_PLACES.txt \
    result_STATES.txt result_EDGES.txt \
    result_STATES_FULL.txt result_EDGES_FULL.txt \
    result_REASONS_SYNTAX.txt result_REASONS_SOUNDNESS.txt \
    > result.csv

#rm result_*.txt
