#!/bin/bash
sh ./sam_mine.sh ./experiments_fse2012/crossftp.xes.gz 20 1.0
sh ./sam_mine.sh ./experiments_fse2012/crossftp.xes.gz 14 1.0

sh ./sam_mine.sh ./experiments_fse2012/columba.xes.gz 40 1.0
sh ./sam_mine.sh ./experiments_fse2012/columba.xes.gz 20 1.0

sh ./sam_mine_trigger.sh ./experiments_fse2012/crossftp.xes.gz 7 10 1.0
sh ./sam_mine_trigger.sh ./experiments_fse2012/crossftp.xes.gz 17 10 1.0
sh ./sam_mine_trigger.sh ./experiments_fse2012/crossftp.xes.gz 50,51,52 10 1.0

sh ./sam_mine_effect.sh ./experiments_fse2012/crossftp.xes.gz 7 10 1.0
sh ./sam_mine_effect.sh ./experiments_fse2012/crossftp.xes.gz 17 10 1.0
sh ./sam_mine_effect.sh ./experiments_fse2012/crossftp.xes.gz 50,51,52 10 1.0

sh ./sam_mine_trigger.sh ./experiments_fse2012/crossftp.xes.gz 50,51,52 1 0.45
sh ./sam_mine_effect.sh ./experiments_fse2012/crossftp.xes.gz 50,51,52 1 0.45