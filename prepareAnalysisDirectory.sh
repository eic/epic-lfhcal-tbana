if [ $1 = "fbock" ]; then
    LFHCALTBDIR=/home/fbock/EIC/Software/epic-LFHCalTB
    HGCROCDECDIR=/home/fbock/EIC/Software/h2g_decode.git/src/
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
    mkdir -p NewStructure/waveform_fitting
    mkdir -p NewStructure/include/h2g_decode
    ln -sf $LFHCALTBDlib/IR/NewStructure/*.h NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/*.cc NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/*.sh NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/Makefile NewStructure/
    ln -sf $LFHCALTBDIR/NewStructure/waveform_fitting/*.h NewStructure/waveform_fitting
    ln -sf $LFHCALTBDIR/NewStructure/waveform_fitting/*.cc NewStructure/waveform_fitting
    ln -sf $HGCROCDECDIR/*.h NewStructure/include/h2g_decode
fi
