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
mapConDef=../configs/TB2025/mapping_HGCROC_PSTB2025_default_wSegments.txt   # layers 0-32 equipped
mapCon2=../configs/TB2025/mapping_HGCROC_PSTB2025_config2_wSegments.txt     # layers 0-24, 33-40 equipped
mapCon3=../configs/TB2025/mapping_HGCROC_PSTB2025_config3_wSegments.txt     # layers 0-16, 25-32, 41-49
mapCon4=../configs/TB2025/mapping_HGCROC_PSTB2025_config4_wSegments.txt     # layers 0-16, 33-40, 50-58


if [ $1 = "fbockTB" ]; then 
  dataRaw=/media/fbock/LFHCal2/202511_PST09/raw/TBMain2025/      # source directory for output files from DAQ system
  dataDir=/media/fbock/LFHCal2/202511_PST09/HGCROCData           # base directory for root trees

elif [ $1 = "egpott" ]; then
  dataRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/raw
  dataDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot

elif [ $1 = "ehagen" ]; then 
  dataRaw=/Volumes/UWU/25_TB_Data
  dataDir=/Volumes/UWU/25_TB_Data

elif [ $1 = "yale" ]; then
	dataRaw=/mnt/wwn-0x5000c500d93c8d2c-part2/Test_Beams/202511_PST09/raw/TBMain2025 
  dataDir=/mnt/wwn-0x5000c500d93c8d2c-part2/Test_Beams/202511_PST09/rawroot
fi
  
# global run list for 2025 TB
runList=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

# make sure dataDir exists
mkdir -p $dataDir/
# default path for the HGCROC analysis
if [ $2 = "FullSetA" ]; then
  if [ $3 = "convert" ]; then	 
#     runs='013 076 164 024 070 071 072 027 073 074 025 069 026 075 161 201 202 203 204 206 205 207 208 165 166 191 167 192 168 169 193 170 194 195 171 172 196 200 173 199 174 198 175 197 176 177 178 179 180 181 182 183 184 185 186 187 188'
    runs='195'
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done

  elif [ $3 = "merge" ]; then 
#     runs='161'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon1_ped_FullSetA
#     runs='207 208'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon2_ped_FullSetA
#     runs='024 070 071 072 027 073 074 025 069 026 075 013 076 164'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon1_FullSetA
#     runs='201 202 203 204 206 205'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt Muon2_FullSetA
# 
#     # electrons
#     runs='165'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e-_1GeV_FullSetA 
#     runs='166 191'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e-_2GeV_FullSetA
#     runs='167 192'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e-_3GeV_FullSetA
#     runs='168 169 193'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e-_4GeV_FullSetA
    runs='170 194 195'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_5GeV_FullSetA
#     # positrons
#     runs='171'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e+_1GeV_FullSetA 
#     runs='172 196 200'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e+_2GeV_FullSetA
#     runs='173 199'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e+_3GeV_FullSetA
#     runs='174 198'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e+_4GeV_FullSetA
#     runs='175 197'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt e+_5GeV_FullSetA
#     # negative hadrons
#     runs='176'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h-_3GeV_FullSetA 
#     runs='177'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h-_5GeV_FullSetA
#     runs='178 179'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h-_8GeV_FullSetA
#     runs='180'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h-_10GeV_FullSetA
#     runs='181'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h-_12GeV_FullSetA
#     runs='182'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h-_15GeV_FullSetA
#     # positive hadrons
#     runs='183'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h+_3GeV_FullSetA 
#     runs='184'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h+_5GeV_FullSetA
#     runs='185'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h+_8GeV_FullSetA
#     runs='186'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h+_10GeV_FullSetA
#     runs='187'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h+_12GeV_FullSetA
#     runs='188'
#     echo $runs > runList.txt
#     MergeMuonsFileList $dataDir runList.txt h+_15GeV_FullSetA
  fi



elif [ $2 = "FullSetB" ]; then
  if [ $3 = "convert" ]; then	 
#     runs='213 214 217 215 211 212 216 209 210 256 257 255 258 252 253 254 260 259 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251'
    runs='217 215 211 212 216 209 210 256 257 255 258 252 253 254 260 259 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251'
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done

  elif [ $3 = "merge" ]; then 
    # Muon sets
    runs='209 210'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon1_ped_FullSetB
    runs='213 214 217 215 211 212 216' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon1_FullSetB
    runs='259'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon2_ped_FullSetB
    runs='256 257 255 258 252 253 254 260'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon2_FullSetB

    # electrons
    runs='218 219'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_1GeV_FullSetB
    runs='220 221 222 223 224 225 226 227'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_2GeV_FullSetB
    runs='228 229 230 231 232 233 234'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_3GeV_FullSetB
    runs='235'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_4GeV_FullSetB
    runs='236'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_5GeV_FullSetB
   
    # negative hadrons
    runs='237'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_3GeV_FullSetB 
    runs='238'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_5GeV_FullSetB
    runs='239'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_8GeV_FullSetB
    runs='240'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_10GeV_FullSetB
    runs='241'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_12GeV_FullSetB
    runs='242'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_15GeV_FullSetB
    
    # positive hadrons
    runs='243'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_3GeV_FullSetB
    runs='244'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_5GeV_FullSetB
    runs='245 246'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_8GeV_FullSetB
    runs='247 248 249'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_10GeV_FullSetB
    runs='250'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_12GeV_FullSetB
    runs='251'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_15GeV_FullSetB
  fi

elif [ $2 = "DepthScan1" ]; then
  if [ $3 = "convert" ]; then	 
    runs='276 289 275 273 274 282 281 280 279 278 277 288 287 286 285 284 283'
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done

  elif [ $3 = "merge" ]; then 
    # Muon set
    runs='276 298 275 273 274'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_DepthScan1
   
    # negative hadrons
    runs='282'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_3GeV_DepthScan1 
    runs='281'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_5GeV_DepthScan1
    runs='280'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_8GeV_DepthScan1
    runs='279'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_10GeV_DepthScan1
    runs='278'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_12GeV_DepthScan1
    runs='277'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_15GeV_DepthScan1
    
    # positive hadrons
    runs='288'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_3GeV_DepthScan1
    runs='287'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_5GeV_DepthScan1
    runs='286'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_8GeV_DepthScan1
    runs='285'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_10GeV_DepthScan1
    runs='284'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_12GeV_DepthScan1
    runs='283'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_15GeV_DepthScan1
  fi


elif [ $2 = "DepthScan2" ]; then
  if [ $3 = "convert" ]; then	 
#     runs='296 299 297 298 291 292 293 295 301 302 303 304 305 306 307 308 309 310 311 312 313'
    runs='300'
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    # Muon set
    runs='291 292 293 295'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt ped_DepthScan2
    runs='296 299 297 298'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_DepthScan2
   
    # negative hadrons
    runs='301'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_3GeV_DepthScan2 
    runs='302'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_5GeV_DepthScan2
    runs='303'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_8GeV_DepthScan2
    runs='304'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_10GeV_DepthScan2
    runs='305'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_12GeV_DepthScan2
    runs='306'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_15GeV_DepthScan2
    
    # positive hadrons
    runs='307'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_3GeV_DepthScan2
    runs='308 309'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_5GeV_DepthScan2
    runs='310'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_8GeV_DepthScan2
    runs='311'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_10GeV_DepthScan2
    runs='312'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_12GeV_DepthScan2
    runs='313'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_15GeV_DepthScan2
  fi

elif [ $2 = "DepthScan3" ]; then
  if [ $3 = "convert" ]; then	 
    runs='325 326 328 330 327 329 314 315 316 317 318 321 322 323 331 332 333 334 335 336 339 340 341 342 343 344'
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon3 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    # Muon set
    runs='314 315 316 317 318 321 322 323'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt ped_DepthScan3
    runs='325 325 328 330 327 329'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_DepthScan3
  
    # negative hadrons
    runs='331'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_3GeV_DepthScan3 
    runs='332'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_5GeV_DepthScan3
    runs='333'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_8GeV_DepthScan3
    runs='334'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_10GeV_DepthScan3
    runs='335'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_12GeV_DepthScan3
    runs='336'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_15GeV_DepthScan3
    
    # positive hadrons
    runs='339'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_3GeV_DepthScan3
    runs='340'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_5GeV_DepthScan3
    runs='341'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_8GeV_DepthScan3
    runs='342'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_10GeV_DepthScan3
    runs='343'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_12GeV_DepthScan3
    runs='344'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_15GeV_DepthScan3
  fi


elif [ $2 = "DepthScan4" ]; then
  if [ $3 = "convert" ]; then	 
#     runs='354 353 355 356 351 357 358 359 360 361 362 363 364 365 366 367 368 369 370'
    runs='352'
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon4 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    # Muon set
    runs='351'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt ped_DepthScan4
    runs='354 353 355 356'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_DepthScan4
  
    # negative hadrons
    runs='357'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_3GeV_DepthScan4 
    runs='358 359'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_5GeV_DepthScan4
    runs='360'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_8GeV_DepthScan4
    runs='361'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_10GeV_DepthScan4
    runs='362'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_12GeV_DepthScan4
    runs='363'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h-_15GeV_DepthScan4
    
    # positive hadrons
    runs='364'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_3GeV_DepthScan4
    runs='365'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_5GeV_DepthScan4
    runs='366'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_8GeV_DepthScan4
    runs='367'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_10GeV_DepthScan4
    runs='368'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_12GeV_DepthScan4
    runs='369 370'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt h+_15GeV_DepthScan4
  fi


# 42.5V electron scan
elif [ $2 = "ElectronScan" ]; then
  if [ $3 = "convert" ]; then	  
    runs='381 382 383 384 385 386 387 388 389 390'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done

  elif [ $3 = "merge" ]; then
    runs='381'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt ped_ElectronScan

    # Muon set
    runs='382 383 385 384' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_ElectronScan

    # Electrons
    runs='386'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_1GeV_ElectronScan
    runs='387'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_2GeV_ElectronScan
    runs='388'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_3GeV_ElectronScan
    runs='389'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_4GeV_ElectronScan
    runs='390'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt e-_5GeV_ElectronScan
  fi


# Muon HV scans
elif [ $2 = "MuonHVScans" ]; then
  if [ $3 = "convert" ]; then	  
#     runs='033 028 029 030 031 032 267 266 265 264 263 268 262 260 261 270'
    runs='267 266 265 264 263 268 262 260 261 270'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done

  elif [ $3 = "merge" ]; then
    # Muon scan 1
    runs='033' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_41V_MuonScan1
    runs='028'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_42V_MuonScan1
    runs='029' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_43V_MuonScan1
    runs='030'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_44V_MuonScan1
    runs='031' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_45V_MuonScan1
    runs='032'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_46V_MuonScan1

    # Muon scan 2
    runs='270'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt ped_MuonScan2
    runs='267' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_41V_MuonScan2
    runs='266'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_42V_MuonScan2
    runs='265'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_42_5V_MuonScan2
    runs='264' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_43V_MuonScan2
    runs='263 268'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_43_5V_MuonScan2
    runs='262'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_44V_MuonScan2
    runs='260' 
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_45V_MuonScan2
    runs='261'
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_46V_MuonScan2
  fi
# Muon Position scans
elif [ $2 = "MuonPosScans" ]; then
  if [ $3 = "convert" ]; then	  
#     runs='007 008 009 010 011 012 013 014 015 016 017 018 019 020 021 022 023 034 035 068'
#     runs='157' # doesn't exist
#     runs='271 272'
    runs='395'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done
  fi
# Hadron scans initial
elif [ $2 = "HadScanInit" ]; then
  if [ $3 = "convert" ]; then	  
#     runs='036 037 038 039 040 041 042 043 046 047 048 049 050 051 052 053 054 055 056 057 058 059'
#     runs='054'
#     runs='160 159'
    runs='372 373 374 375 376 377 378 379 380' # rate scan
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done
  fi
elif [ $2 = "pedestals" ]; then 
  if [ $3 = "convert" ]; then	  
    # pedestals with different Nr of asics
#     runs='122 123 124 125 126 127 128 129'
#     runs='127'
#     for runNr in $runs; do 
#       ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
#     done
#     runs='036 055 056 058 059 068 130 141 156 161 207 208 209 210 259 269 270  381 391'
    runs='269'
    for runNr in $runs; do 
    ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
    done
#     
#     runs='291 292 293 295'
#     for runNr in $runs; do 
#     ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon2 -r $runList
#     done
    runs='319'
#     runs='314 315 316 317 318 319 321 322 323'
    for runNr in $runs; do 
    ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon3 -r $runList
    done
#     runs='351'
#     for runNr in $runs; do 
#     ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon4 -r $runList
#     done
  fi
fi
