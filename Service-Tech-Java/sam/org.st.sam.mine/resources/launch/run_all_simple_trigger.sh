#!/bin/bash
sh ./sam_mine_trigger.sh experiments/crossftp_succinct/crossftp_succinct_traceset.xes.gz 12 1.0
sh ./sam_mine_trigger.sh experiments/crossftp_invariants/crossftp_invariants.xes.gz 80 1.0
sh ./sam_mine_trigger.sh experiments/jeti_invariants/jeti_invariants.xes.gz 20 1.0
sh ./sam_mine_trigger.sh experiments/jeti/jeti.xes.gz 20 1.0
sh ./sam_mine_trigger.sh experiments/columba/columba.xes.gz 20 1.0