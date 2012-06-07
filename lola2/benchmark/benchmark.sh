#!/bin/sh

for NET in phils10.lola Kanban5.pnml.lola
do
    for STORE in bloom bin stl
    do
        echo "Checking $NET with $STORE store - full..."
        local/bin/lola ../tests/testfiles/$NET --check=full --stats --store=$STORE 2> $NET-$STORE-full.log
    done
done
