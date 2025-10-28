if [ $1 = "fbock" ]; then
    LFHCALTBDIR=/home/fbock/EIC/Software/epic-LFHCalTB
elif [ $1 = "eglimos" ]; then
    LFHCALTBDIR=/home/ewa/EIC/epic-lfhcal-tbana
elif [ $1 = "vandrieu" ]; then
    LFHCALTBDIR=/home/vandrieu/Software/epiclfhcal
elif [ $1 = "kmaret" ]; then
    LFHCALTBDIR=/mnt/d/work
else 
    echo "Please add your user name to the script and define the location of the TBAnalysisSoftware"
    exit
fi

echo $LFHCALTBDIR

mkdir -p configs

ln -sf $LFHCALTBDIR/configs/*.txt configs/
ln -sf $LFHCALTBDIR/configs/*.csv configs/

if [ $2 = "old" ]; then
    mkdir -p OldStructure
    ln -sf $LFHCALTBDIR/OldStructure/*.C OldStructure/
    ln -sf $LFHCALTBDIR/OldStructure/*.h OldStructure/
    ln -sf $LFHCALTBDIR/OldStructure/*.sh OldStructure/
fi 

if [ $2 = "new" ]; then
    mkdir -p NewStructure
    ln -sf $LFHCALTBDIR/NewStructure/CompareCalib NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/CompareAna NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/Convert NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/DataAna NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/DataPrep NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/Display NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/HGCROC_Conversion NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/*.sh NewStructure/
fi
