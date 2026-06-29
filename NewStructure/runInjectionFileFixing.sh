#/bin/bash

#runInjectionFileFixing

# runlist='ls /media/fbock/ALICE2-4TB/202604_PST10/Calibrations/205_Injection_*/*.csv'
# runlist='ls /media/fbock/ALICE2-4TB/202604_PST10/Calibrations/*/205_Inje*/*.csv'
# runlist='ls /media/fbock/ALICE2-4TB/202604_PST10/Calibrations/*/*/205_Inje*/*.csv'
runlist='ls /media/fbock/ALICE2-4TB/202604_PST10/Calibrations/*/*/*/205_Inje*/*.csv'
for run in $runlist; do 
  echo $run
  bash FixInjectionsFormat_add_header_and_ids.sh $run
done
