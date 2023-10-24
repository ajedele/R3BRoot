runindex1=191
runindex2=192
runindex3=197
runindex4=221
#runindex5=221
#runindex6=228
#runindex7=230
#runindex8=232


      root -l -q TofdCal2HitPar.C\($runindex1,200000000,1\)	  
      root -l -q TofdCal2HitPar.C\($runindex2,200000000,2\)
      root -l -q TofdCal2HitPar.C\($runindex3,200000000,3\)	  
      root -l -q TofdCal2HitPar.C\($runindex4,200000000,4\)	  
      
      
#runid=196
#runmax=198
#
#      while [ $runid -lt $runmax ]
#      do
#	      root -l -q PspxPoscal2CalPar.C\($runid\)
#	      ((runid++))
#      done
