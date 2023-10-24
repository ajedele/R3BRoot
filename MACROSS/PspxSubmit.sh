#!/bin/bash

runid=190
runmax=195

#while [ $runid -lt $runmax ]
#do
	#memUsage=$(free -m | awk 'NR==2{printf"%d\n",$3*100/$2}')
	#if [ "$memUsage" -lt 80 ];
	#then
	#echo "Memory Usage: $memUsage MB"	
	#root -l -q -b PspxMapped2Poscal.C\($runid\)
	#root -l -q -b PspxPoscal2Cal.C\($runid\)
	#sleep 4h
	root -l -q -b PspxCal2Hit.C\(208\)
	root -l -q -b PspxCal2Hit.C\(209\)
	
	#root -l -q -b PspxCal2HitPar.C\(184\)
	#root -l -q -b PspxCal2HitPar.C\(186\)
	#root -l -q -b PspxCal2HitPar.C\(187\)
	#root -l -q -b PspxCal2HitPar.C\(188\)
	#root -l -q -b PspxCal2HitPar.C\(189\)
	#root -l -q -b PspxCal2HitPar.C\(190\)
	#root -l -q -b PspxCal2HitPar.C\(191\)
	#root -l -q -b PspxCal2HitPar.C\(192\)
	#root -l -q -b PspxCal2HitPar.C\(193\)
	#root -l -q -b PspxCal2HitPar.C\(211\)
	#root -l -q -b PspxCal2HitPar.C\(212\)
	#root -l -q -b PspxCal2HitPar.C\(220\)
	#root -l -q -b PspxCal2HitPar.C\(229\)
	#((runid++))
	#fi
#done

