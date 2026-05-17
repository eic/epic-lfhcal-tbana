#! /bin/bash

function MergeMuonsFileList(){
  echo "========================================================================="
  echo "dataDir: $1"
  echo "run-list: $2"
  echo "Additional name: $3"
  echo "========================================================================="
  runs=`cat $2`
  if [ -f listMerge.txt ]; then
    rm listMerge.txt
  fi
	for runNr in $runs; do 
		ls $1/rawHGCROC_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $1/rawHGCROC_$3.root $fileList
  rm listMerge.txt
}

echo "username $1"
echo "run option $2"

# different mapping files for layering
mapConDefV2=../configs/TB2026/mapping_HGCROC_SPSH2TB_sumV2_default.csv   # v2 summing board
mapConDefV1=../configs/TB2026/mapping_HGCROC_PST10TB_sumV1_default_inv.csv   # v1 summing board


if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/202605_SPSH2/raw/      # source directory for output files from DAQ system
	dataDir=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData           # base directory for root trees  fi
elif [ $1 = "egpott" ]; then # bla bla bla test test test
	dataRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/raw
	dataDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot

elif [ $1 = "ehagen" ]; then 
	dataRaw=/Users/hagen/Githubs/TB_data
	dataDir=/Users/hagen/Githubs/TB_data
	
elif [ $1 = "atamis" ]; then 
	dataRaw=/home/drewtam20/Documents/eic/LFHCALDATA/Runs
	dataDir=/home/drewtam20/Documents/eic/LFHCALDATA/Converted

elif [ $1 = "Preet" ]; then
	dataRaw=/home/lfhcal/Data # source directory for output files from DAQ system
	dataDir=/home/lfhcal/HGCROCData # base directory for root trees
elif [ $1 = "kchandra" ]; then
	dataRaw=/run/media/lfhcal/Howard4TB/202605_SPSH2/raw # source directory for output files from DAQ system
	dataDir=/run/media/lfhcal/Howard4TB/202605_SPSH2/HGCROCData # base directory for root trees
fi



# global run list for 2026 SPS TB
runList=../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv

# default path for the HGCROC analysis	
#V2 summing board, initial preamp settings 
if [ $2 = "InitMuon" ]; then 	
  if [ $3 = "convert" ]; then 
#     runs='001 002 003'
#     runs='004 005 006 007 008 009 010 011 012 013 014 015 016'
#       runs='046 047 017 018 019 020 021 022 '  #046 047
#       runs='023 024 025 026 027 028 029 030 031 032 033 034 035 036 037 038 039 '  
#       runs='017 018 019 020 021 022 '  
#       runs='048 049 '  
#       runs='050 051 '  
#       runs='052'  
      # fixed mapping starting with 52
#       runs='052 053 054 055 056 057 058 059 060 '  
#       runs='048 049 061 062 063 064 065 066 067 068 069 070 '  
#         runs='030 031 032 033 034 035 036 037 038 039 040 041 042 043 044 045 046 047'
#     runs='017 018 019 020 021 022'
    runs='023 024 025 026 027 028 029'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then  
    echo "not defined yet"
  fi
# 43 V, summing board V2, Preamp settings 9 7 10 1?
elif [ $2 = "FullSetB" ]; then 
  if [ $3 = "convert" ]; then 
     echo "started conversion"
#     runs='' #full list
#         runs='071 072 073 074 075 076 077 078 079 080 081 082 083 084' #full list
      runs='071 126' #pedestals
#       runs='072 073 074 075 076 077 078 079 080 081 082 083 084' #muons set 1
#       runs='130 131 132 133' #muons set 2
#       runs='085 086 067 088 089 089 090' #e-
#       runs='091 092 093 094 095 096 097 ' #e+
#       runs='113 114 115 116 117 118 119 120 121 122 123 124 125' #pi-
#       runs='098 099 100 101 102 103 104 105 106 107 108 109 110 111 112' #h+
#       runs='100 101 102 103 104 105 106 107 108 109 110 111 112' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
#     runs='072 073 074 075 076 077 078 079 080 081 082 083 084' # set 1
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_1  #ok
    runs='130 131 132 133' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_2  #ok
  fi
# 44 V, summing board V2, Preamp settings 9 7 10 1?
elif [ $2 = "FullSetC" ]; then 
  if [ $3 = "convert" ]; then 
  #     runs='' #full list
#       runs='134 135' #pedestals
#       runs='137 188' #pedestals
#       runs='147 148 149 146 145 144 141 142 142 140 139 138' #muons set 1
#       runs='194 190 191 193 192' #muons set 2
      runs='150 151 152 153 154 155' #e-
#       runs='156 157 158 159 160 161' #e+
#       run='156 158' # something weird
#       runs='159 160 161' #e+
#       runs='162 163 164 165 166 167 168 169 170 171 172 173' #pi-
#       runs='174 175 176 177 178 179 180 181 182 183 184 186 187' #h+
#       runs='184 186 187' #h+ 
#       runs='185'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='147 148 149 146 145 144 141 142 142 140 139 138' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetC_1  #ok
    runs='194 190 191 193 192' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetC_1  #ok
  fi
  
# 45 V, summing board V2, Preamp settings 9 7 10 4?
elif [ $2 = "FullSetD" ]; then
  if [ $3 = "convert" ]; then
    # runs='206 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224' #full list
      # runs='206' #pedestals
#       runs='208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224' #muons set 1
      # runs='225 226 227 228 229 231' #e-
      # runs='232 233 234 235 236 237' #e+
    #  runs='238'
       runs='238 239 240 241 242 243 244 245 246 247 248 249' #pi-
    #   runs='250 251 252 253 ' #h+
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then
    runs='072 073 074 075 076 077 078 079 080 081 082 083 084' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_1  #ok
  fi


elif [ $2 = "HVScan" ]; then
  if [ $3 = "convert" ]; then
    # runs='194 195 196 197 198 199 200 201 202' #mu
    runs='188'
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  fi

  elif [ $2 = "HadronScan" ]; then
  if [ $3 = "convert" ]; then
      runs=' 207 ' #e-
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
fi
