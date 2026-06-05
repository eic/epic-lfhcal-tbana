if [ $1 = "fbock" ]; then
  LFHCALTBDIR=/home/fbock/EIC/Software/epic-LFHCalTB
elif [ $1 = "eglimos" ]; then
  LFHCALTBDIR=/home/ewa/EIC/epic-lfhcal-tbana
elif [ $1 = "vandrieu" ]; then
  LFHCALTBDIR=/home/vandrieu/Software/epiclfhcal
elif [ $1 = "ehagen" ]; then
  LFHCALTBDIR=/Users/hagen/Githubs/epic-lfhcal-tbana/TB_Data
else 
    echo "Please add your user name to the script and define the location of the TBAnalysisSoftware"
    exit
fi

echo $LFHCALTBDIR

mkdir -p configs

ln -sf $LFHCALTBDIR/configs/*.txt configs/
if [ $3 = "2023" ]; then 
  mkdir -p configs/TB2023
  ln -sf $LFHCALTBDIR/configs/TB2023/*.txt configs/TB2023
  ln -sf $LFHCALTBDIR/configs/TB2023/*.csv configs/TB2023
elif [ $3 = "2024" ]; then 
  mkdir -p configs/TB2024
  ln -sf $LFHCALTBDIR/configs/TB2024/*.txt configs/TB2024
  ln -sf $LFHCALTBDIR/configs/TB2024/*.csv configs/TB2024
elif [ $3 = "2025" ]; then 
  mkdir -p configs/TB2025
  ln -sf $LFHCALTBDIR/configs/TB2025/*.txt configs/TB2025
  ln -sf $LFHCALTBDIR/configs/TB2025/*.csv configs/TB2025
elif [ $3 = "2026" ]; then 
  mkdir -p configs/TB2026
  ln -sf $LFHCALTBDIR/configs/TB2026/*.txt configs/TB2026
  ln -sf $LFHCALTBDIR/configs/TB2026/*.csv configs/TB2026
fi

if [ $2 = "old" ]; then
    mkdir -p OldStructure
    ln -sf $LFHCALTBDIR/OldStructure/*.C OldStructure/
    ln -sf $LFHCALTBDIR/OldStructure/*.h OldStructure/
    ln -sf $LFHCALTBDIR/OldStructure/*.sh OldStructure/
fi 

if [ $2 = "new" ]; then
    mkdir -p NewStructure
    ln -sf $LFHCALTBDIR/NewStructure/CompareCalib NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/CompareHGCROCCalib NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/CompareInjection NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/CompareAna NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/Convert NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/DataAna NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/DataPrep NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/Display NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/HGCROC_Conversion NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/HGCROCStudy NewStructure/    
    ln -sf $LFHCALTBDIR/NewStructure/ParseCalibSamples NewStructure/    
    ln -sf $LFHCALTBDIR/NewStructure/*.sh NewStructure/
fi
