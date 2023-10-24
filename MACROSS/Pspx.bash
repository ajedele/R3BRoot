#runindex1=194
#runindex2=210
#runindex3=213
#runindex4=219
#runindex5=221
#runindex6=228
#runindex7=230
#runindex8=232
#
#
#      root -l -q PspxMapped2CalPar.C\($runindex1,$runindex2\)	  
#
#      root -l -q PspxMapped2CalPar.C\($runindex3,$runindex4\)
#
#      root -l -q PspxMapped2CalPar.C\($runindex5,$runindex6\)	  
#
#      root -l -q PspxMapped2CalPar.C\($runindex7,$runindex8\)	  
      
      
runid=196
runmax=198

      while [ $runid -lt $runmax ]
      do
	      root -l -q PspxPoscal2CalPar.C\($runid\)
	      ((runid++))
      done
