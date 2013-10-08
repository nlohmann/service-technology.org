call sam_mine ./experiments_fse2012/crossftp.xes.gz 20 1.0
call sam_mine ./experiments_fse2012/crossftp.xes.gz 14 1.0

call sam_mine ./experiments_fse2012/columba.xes.gz 40 1.0
call sam_mine ./experiments_fse2012/columba.xes.gz 20 1.0

call sam_mine_trigger ./experiments_fse2012/crossftp.xes.gz 7 10 1.0
call sam_mine_trigger ./experiments_fse2012/crossftp.xes.gz 17 10 1.0
call sam_mine_trigger ./experiments_fse2012/crossftp.xes.gz 50,51,52 10 1.0

call sam_mine_effect ./experiments_fse2012/crossftp.xes.gz 7 10 1.0
call sam_mine_effect ./experiments_fse2012/crossftp.xes.gz 17 10 1.0
call sam_mine_effect ./experiments_fse2012/crossftp.xes.gz 50,51,52 10 1.0

call sam_mine_trigger ./experiments_fse2012/crossftp.xes.gz 50,51,52 1 0.45
call sam_mine_effect ./experiments_fse2012/crossftp.xes.gz 50,51,52 1 0.45