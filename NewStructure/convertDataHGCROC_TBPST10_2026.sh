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
mapConDefV2=../configs/TB2026/mapping_HGCROC_PST10TB_sumV2_default_inv.csv   # v2 summing board
mapConDefV1=../configs/TB2026/mapping_HGCROC_PST10TB_sumV1_default_inv.csv   # v1 summing board


if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/202604_PST10/raw/      # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/202604_PST10/HGCROCData           # base directory for root trees

elif [ $1 = "egpott" ]; then
	dataRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/raw
	dataDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot

elif [ $1 = "ehagen" ]; then 
	dataRaw=/Users/hagen/Githubs/TB_data
	dataDir=/Users/hagen/Githubs/TB_data

fi
  
# global run list for 2026 PS TB
runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

# default path for the HGCROC analysis	
if [ $2 = "HVScan1" ]; then 	
  if [ $3 = "convert" ]; then 
#     runs='039 066 070 079 080 082 064 065 067 068 069 071 072 073 074 075 076 077 081 083 084 221 223 224 225 226 227 228 230 229 231 '
#     runs='039 066 070 079 080 082 064 065 067 068 069 071 072 073 074 075 076 077 081 083 084' #part 2
    runs='221 223 224 225 226 227 228 230 229 231 ' #part 2
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then  
#     runs='071 072 073 074' # HV 42V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_42V  #ok
#     runs='071 072' # HV 42V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_42V_bottom  #ok
#     runs='073 074' # HV 42V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_42V_top  #ok
# 
#     runs='067 068 069' # HV 42.5V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_42_5V  #ok
#     cp $dataDir/rawHGCROC_062.root $dataDir/rawHGCROC_Muon_HVScan_42_5V_bottom.root #ok  
#     runs='068 069' # HV 42V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_42_5V_top  #ok
#     
#     runs='064 065' # HV 43V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_43V  #ok
#     cp $dataDir/rawHGCROC_065.root $dataDir/rawHGCROC_Muon_HVScan_43V_bottom.root #ok  
#     cp $dataDir/rawHGCROC_064.root $dataDir/rawHGCROC_Muon_HVScan_43V_top.root #ok  
#     
#     runs='077 081' # HV 43.5V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_43_5V  #ok
#     cp $dataDir/rawHGCROC_077.root $dataDir/rawHGCROC_Muon_HVScan_43_5V_bottom.root #ok  
#     cp $dataDir/rawHGCROC_081.root $dataDir/rawHGCROC_Muon_HVScan_43_5V_top.root #ok  
#     
#     runs='084 083' # HV 44V
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon_HVScan_44V  #ok
#     cp $dataDir/rawHGCROC_084.root $dataDir/rawHGCROC_Muon_HVScan_44V_bottom.root #ok  
#     cp $dataDir/rawHGCROC_083.root $dataDir/rawHGCROC_Muon_HVScan_44V_top.root #ok  
# 
    runs='223 224' # HV 44_5V
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_HVScan_44_5V  #ok
    cp $dataDir/rawHGCROC_224.root $dataDir/rawHGCROC_Muon_HVScan_44_5V_bottom.root #ok  
    cp $dataDir/rawHGCROC_223.root $dataDir/rawHGCROC_Muon_HVScan_44_5V_top.root #ok  

    runs='226 227' # HV 45V
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_HVScan_45V  #ok
    cp $dataDir/rawHGCROC_226.root $dataDir/rawHGCROC_Muon_HVScan_45V_bottom.root #ok  
    cp $dataDir/rawHGCROC_227.root $dataDir/rawHGCROC_Muon_HVScan_45V_top.root #ok  

    runs='230 229' # HV 45.5V
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_HVScan_45_5V  #ok
    cp $dataDir/rawHGCROC_230.root $dataDir/rawHGCROC_Muon_HVScan_45_5V_bottom.root #ok  
    cp $dataDir/rawHGCROC_229.root $dataDir/rawHGCROC_Muon_HVScan_45_5V_top.root #ok  

    runs='232 233' # HV 46V
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_HVScan_46V  #ok
    cp $dataDir/rawHGCROC_232.root $dataDir/rawHGCROC_Muon_HVScan_46V_bottom.root #ok  
    cp $dataDir/rawHGCROC_233.root $dataDir/rawHGCROC_Muon_HVScan_46V_top.root #ok  
  fi
elif [ $2 = "FirstPosScanMuons" ]; then 	
  if [ $3 = "convert" ]; then 
    runs='039 040 041 042 043 044 045 046 047 048 049 050 051 052 053 054 055 056 057 058 059 060 061 062 063'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='040 041 042' # firstMuons
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt FirstMuons 

    # position scan
    runs='040 041 042 043 044 045 046 047 048 049' # firstMuons 0,0
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt FMuonCent # ok
    runs='050 051 052' # firstMuons 5,0
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt FMuon_5_0 #ok
    runs='053 054 055' # firstMuons 5,-5
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt FMuon_5_-5  #ok
    runs='056 057' # firstMuons 0,-5
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt FMuon_0_-5  #ok
    # firstMuons -5,-5
    cp $dataDir/rawHGCROC_058.root $dataDir/rawHGCROC_FMuon_-5_-5.root #ok
    # firstMuons -5,0
    cp $dataDir/rawHGCROC_059.root $dataDir/rawHGCROC_FMuon_-5_0.root #ok
    # firstMuons 0,5
    cp $dataDir/rawHGCROC_062.root $dataDir/rawHGCROC_FMuon_0_5.root #ok
    # firstMuons 5,5
    cp $dataDir/rawHGCROC_063.root $dataDir/rawHGCROC_FMuon_5_5.root  #ok 
    runs='060 061' # firstMuons -5,5
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt FMuon_-5_5  #ok
  fi
  
elif [ $2 = "SetA-PosScanMuons" ]; then 	
  if [ $3 = "convert" ]; then 
    runs='085 086 087 088 089 090 091 092 '
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    cp $dataDir/rawHGCROC_088.root $dataDir/rawHGCROC_Muon_0_5.root 
    runs='086 087' # Muons 5,5
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_0_-5
    cp $dataDir/rawHGCROC_090.root $dataDir/rawHGCROC_Muon_5_-5.root 
    cp $dataDir/rawHGCROC_089.root $dataDir/rawHGCROC_Muon_5_5.root 
    cp $dataDir/rawHGCROC_092.root $dataDir/rawHGCROC_Muon_-5_5.root 
    cp $dataDir/rawHGCROC_091.root $dataDir/rawHGCROC_Muon_-5_-5.root 
  fi
# 43 V, summing board V2, original default preamp settings
elif [ $2 = "FullSetA" ]; then 
  if [ $3 = "convert" ]; then 
    runs='085 086 087 088 089 090 091 092 094 095 096 097 098 099 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 123 124 125 126' #full list
    #   runs='085 120' #pedestals
    #   runs='088 086 087 090 089 092 091' #muons set 1
    #   runs='121 122 123 124 125 126' #muons set 2
    #   runs='098 096 099 100 101 101 102 103 104 105 106 097 108' #pi-
    #   runs='109 110 111 112 113 114 115 116 117 118 119 095 094' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='086 087 088 089 090 091 092 ' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetA_1  #ok
    runs='121 122 123 124 125 126 ' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetA_2  #ok
    runs='104 105'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_8GeV_FullSetA  #ok
    runs='097 106'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_9GeV_FullSetA  #ok
    runs='115 116'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Had+_7GeV_FullSetA  #ok
    runs='117 118 119'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Had+_8GeV_FullSetA  #ok
  fi
# 42.5 V, summing board V2, original default preamp settings
elif [ $2 = "FullSetB" ]; then 
  if [ $3 = "convert" ]; then 
    runs='189 190 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 217 218 219 220' #full list
    #   runs='215' #pedestals
    #   runs='189 190 192 193' #muons set 1
    #   runs='217 218 219 220' #muons set 2
    #   runs='194 195 196 197 198 199 200 201 202 203 204' #pi-
    #   runs='205 206 207 208 209 210 211 212 213 214' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='189 190 192 193 ' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_1  #ok
    runs='217 218 219 220 ' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_2  #ok
  fi
# 44 V, summing board V2, original default preamp settings
elif [ $2 = "FullSetC" ]; then 
  if [ $3 = "convert" ]; then 
    runs='130 121 132 133 134 135 136 139 160 140 161 141 162 142 163 143 164 144 165 145 166 146 167 150 151 152 153 154 155 156 157 149 168 148 168 169 147 170 158 159 171 177 179 178 181 182 180 183 184' #full list
    #   runs='130 171' #pedestals
    #   runs='130 121 132 133 134 135 136' #muons set 1
    #   runs='177 179 178 181 182 180 183 184' #muons set 2
    #   runs='139 160 140 161 141 162 142 163 143 164 144 165 145 166 146 167 150 151 152  ' #pi-
    #   runs='153 154 155 156 157 149 168 148 168 169 147 170 158 159' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='130 121 132 133 134 135 136' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetC_1  #ok
    runs='177 179 178 181 182 180 183 184' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetC_2  #ok
    
    #pi- merge
    runs='139 160 '
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_1GeV_FullSetA  #ok
    runs='140 161'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_2GeV_FullSetA  #ok
    runs='141 162'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_3GeV_FullSetA  #ok
    runs='142 163 '
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_4GeV_FullSetA  #ok
    runs='143 164  '
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_5GeV_FullSetA  #ok
    runs='144 165  '
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_6GeV_FullSetA  #ok
    runs='145 166  '
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_7GeV_FullSetA  #ok
    runs='146 167  '
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_8GeV_FullSetA  #ok
    cp $dataDir/rawHGCROC_150.root $dataDir/rawHGCROC_pi-_9GeV_FullSetA.root 
    runs='151 152  '
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_10GeV_FullSetA  #ok
    
    # had merge
    runs='149 168'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Had+_6GeV_FullSetA  #ok
    runs='148 169'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Had+_7GeV_FullSetA  #ok
    runs='147 170'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Had+_8GeV_FullSetA  #ok

  fi  
# 44V, summing board V2, new preamp settings 
elif [ $2 = "FullSetD" ]; then 
  if [ $3 = "convert" ]; then 
    runs='238 242 241 240 244 245 246 247 248 249 250 251 252 253 254 255 256 257 258 259 260 261 262 263 264 265' #full list
    #   runs='238 265' #pedestals
    #   runs='242 241 240 244' #muons set 1
    #   runs='266 267 268 269' #muons set 2
    #   runs='245 246 247 248 249 250 251 252 253 254' #pi-
    #   runs='255 256 257 258 259 260 261 262 263 264' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='242 241 240 244' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetD_1  #ok
    runs='266 267 268 269' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetD_2  #ok
    
  fi
# 43V, summing board V2, new preamp settings 
elif [ $2 = "FullSetE" ]; then 
  if [ $3 = "convert" ]; then 
    runs='287 288 289 290 291 292 293 294 295 296 297 300 301 302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320' #full list
    #   runs='287 315' #pedestals
    #   runs='288 289 290 291' #muons set 1
    #   runs='316 317 318 319 320' #muons set 2
    #   runs='292 293 294 295 296 297 300 301 302 303 304' #pi-
    #   runs='305 306 307 308 309 310 311 312 313 314' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='288 289 290 291' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetE_1  #ok
    runs='316 317 318 319 320' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetE_2  #ok
    runs='292 293'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt pi-_1GeV_FullSetA  #ok
  fi
fi
