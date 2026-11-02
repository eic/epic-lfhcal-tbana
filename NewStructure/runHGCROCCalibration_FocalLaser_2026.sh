#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=..
runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202609.csv

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbock" ]; then 
  dataDirRaw=/media/fbock/FrediExt4TB/FOCAL_TB/202609_FocalLaser/HGCROCData
  dataDirOut=/media/fbock/FrediExt4TB/FOCAL_TB/202609_FocalLaser/HGCROCData
  PlotBaseDir=/media/fbock/FrediExt4TB/FOCAL_TB/202609_FocalLaser/
  runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202609.csv
elif [ $1 = "fbockOct" ]; then 
  dataDirRaw=/media/fbock/FrediExt4TB/FOCAL_TB/202610_FocalLaser/HGCROCData
  dataDirOut=/media/fbock/FrediExt4TB/FOCAL_TB/202610_FocalLaser/HGCROCData
  PlotBaseDir=/media/fbock/FrediExt4TB/FOCAL_TB/202610_FocalLaser/
  runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202610.csv
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

########################################################################
######################## Pedestal Running ##############################
########################################################################
if [ $2 = "pedestal" ]; then
  if [ $3 = "Sept" ]; then 
    runs='166 167 168 178 188 198 208 210 212 213 216 218 221 258 268 279 309 334 359 372 386 401 416 431'
  ########################################################################
  ######################## OCT Laser campaign ############################
  ########################################################################
  else 
    runs='1 2 477 478 504 762 763 789 815 816 843 1105 1106 1133'
  fi
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    Pedestal $2 $runNrPed $dataDirRaw $dataDirOut Run$runNrPed pdf
  done
fi

########################################################################
######################## TOA phase extraction ##########################
########################################################################
if [ $2 = "toaPhase" ]; then   
  if [ $3 = "SetA" ]; then
    runNrPed='168'    
#     runs='173'
    runs='169 170 171 172 173 174 175 176 177'
  elif [ $3 = "SetD" ]; then
    runNrPed='178'
    runs='179 180 181 182 183 184 185 186 187'
  fi
  for runNr in $runs; do 
    ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirOut/rawHGCROC_wPed_$runNrPed.root #-F png
  done
fi

########################################################################
######################## Calibration running ###########################
########################################################################
if [ $2 == "calibMuon" ]; then
  toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
  badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt
  
  if [ $4 == "SetA" ]; then 
    runNrPed='168'    
    runs='169 170 171 172 173 174 175 176 177'
  elif [ $4 == "SetD" ]; then 
    runNrPed='178'
    runs='179 180 181 182 183 184 185 186 187'
  elif [ $4 == "SetE" ]; then 
    runNrPed='188'
    runs='189 190 191 192 193 194 195 196 197'
  elif [ $4 == "SetQ" ]; then 
    runNrPed='198'
    runs='199 200 201 202 203 204 205 206 207'
  elif [ $4 == "SetB" ]; then 
    runNrPed='221'
    runs='222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 256 257'
  elif [ $4 == "SetC" ]; then 
    runNrPed='258'
    runs='259 260 261 262 263 264 265 266 267'
  elif [ $4 == "SetF" ]; then 
    runNrPed='268'
    runs='269 270 271 272 273 274 275 276 277'
  elif [ $4 == "SetK" ]; then 
    runNrPed='279'
    runs='280 281 282 283 284 285 286 287 288 289 290 291 292 293'
  elif [ $4 == "SetL" ]; then 
    runNrPed='279'
    runs='294 295 296 297 298 299 300 301 302 303 304 305 306 307 308'
  elif [ $4 == "SetM" ]; then 
    runNrPed='309'
    runs='310 311 312 313 314 315 316 317 318 319 320 321 322 323 324 325 326 327 328 329 330 331 332 333'
  elif [ $4 == "SetG" ]; then 
    runNrPed='334'
    runs='335 336 337 338 339 340 341 342 343 344 345 346 347 348 349 350 351 352 353 354 355 356 357 358 '
  elif [ $4 == "SetH" ]; then 
    runNrPed='359'
    runs='360 361 362 363 364 365 366 367 368 369 370 371 '
  elif [ $4 == "SetI" ]; then 
    runNrPed='372'
    runs='373 374 375 376 377 378 379 380 381 382 383 384 385 '
  elif [ $4 == "SetJ" ]; then 
    runNrPed='386'
    runs='387 388 389 390 391 392 393 394 395 396 397 398 399 400 '
  elif [ $4 == "SetN" ]; then 
    runNrPed='401'
    runs='402 403 404 405 406 407 408 409 410 411 412 413 414 415 '
  elif [ $4 == "SetO" ]; then 
    runNrPed='416'
    runs='417 418 419 420 421 422 423 424 425 426 427 428 429 430  '
  elif [ $4 == "SetP" ]; then 
    runNrPed='431'
    runs='432 433 434 435 436 437 438 439 440 441 442 443 444 445'
  elif [ $4 == "HVScan" ]; then 
    runNrPeds='208 210 212 214 216 218 220'
    runs='209 211 213 215 217 219'
  ########################################################################
  ######################## OCT Laser campaign ############################
  ########################################################################
  elif [ $4 = "Oct_RF4_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='002'
    runs=''
    rm runlist.txt
    for runNr in {003..476}; do 
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V43_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='478'
    runs=''
    rm runlist.txt
    for runNr in {479..503}; do   #Set AA
      echo "$runNr " >> runlist.txt
    done
    for runNr in {530..554}; do   #Set AC
      echo "$runNr " >> runlist.txt
    done
    for runNr in {570..594}; do   #Set AE
      echo "$runNr " >> runlist.txt
    done
    for runNr in {610..634}; do   #Set AG
      echo "$runNr " >> runlist.txt
    done
    for runNr in {650..674}; do   #Set AI
      echo "$runNr " >> runlist.txt
    done
    for runNr in {695..706}; do   #Set AK
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V45_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='504'
    runs=''
    rm runlist.txt
    for runNr in {505..529}; do   #Set AB
      echo "$runNr " >> runlist.txt
    done
    for runNr in {555..569}; do   #Set AD
      echo "$runNr " >> runlist.txt
    done
    for runNr in {595..609}; do   #Set AF
      echo "$runNr " >> runlist.txt
    done
    for runNr in {635..649}; do   #Set AH
      echo "$runNr " >> runlist.txt
    done
    for runNr in {675..694}; do   #Set AJ
      echo "$runNr " >> runlist.txt
    done
    for runNr in {707..717}; do   #Set AL
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V43_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='816'
    runs=''
    rm runlist.txt
    for runNr in {817..842}; do   #Set BA
      echo "$runNr " >> runlist.txt
    done
    for runNr in {869..893}; do   #Set BC
      echo "$runNr " >> runlist.txt
    done
    for runNr in {919..956}; do   #Set BE
      echo "$runNr " >> runlist.txt
    done
    for runNr in {971..995}; do   #Set BG
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1011..1035}; do   #Set BI
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1060..1084}; do   #Set BK
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V45_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='843'
    runs=''
    rm runlist.txt
    for runNr in {844..868}; do   #Set BB
      echo "$runNr " >> runlist.txt
    done
    for runNr in {894..918}; do   #Set BD
      echo "$runNr " >> runlist.txt
    done
    for runNr in {957..970}; do   #Set BF
      echo "$runNr " >> runlist.txt
    done
    for runNr in {996..1010}; do   #Set BH
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1036..1059}; do   #Set BJ
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1085..1104}; do   #Set BL
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V43_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='719'
    runs=''
    rm runlist.txt
    for runNr in {720..744}; do   #Set AAA
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V45_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='745'
    runs=''
    rm runlist.txt
    for runNr in {746..761}; do   #Set AAB
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V43_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='763'
    runs=''
    rm runlist.txt
    for runNr in {764..788}; do   #Set AAA
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V45_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='789'
    runs=''
    rm runlist.txt
    for runNr in {790..814}; do   #Set AAB
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  fi
  
  for runNr in $runs; do
    MuonCalibHGCROC $3 $runNrPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset 	
  done
fi

########################################################################
######################## TOT studies ###################################
########################################################################
if [ $2 = "tot" ]; then
  toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
  badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt
  cellList="../configs/FOCalTest2026/cellListActive_board2_laserScanFocalLab.txt"
  if [ $3 = "SetA" ]; then
    runNrPed='168'    
    runs='173'
#     runs='169 170 171 172 173 174 175 176 177'
  elif [ $3 = "SetD" ]; then
    runNrPed='178'
    runs='179 180 181 182 183 184 185 186 187'
  fi
  
  for runNr in $runs; do
    ./HGCROCStudy -f -d 1 -V -C $dataDirRaw/rawHGCROC_wPed_wBC_$runNr.root -i $dataDirRaw/rawHGCROC_wPed_wBC_$runNr.root -o $dataDirOut/TOTvsMeanADC_Output_Run$runNr.root -O $PlotBaseDir/HGCROC_PlotsTotStudy/ -r $runList -l $cellList
  done
fi

########################################################################
######################## Wave studies ##################################
########################################################################
if [ $2 = "wave" ]; then
  toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
  badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt
  cellList="../configs/FOCalTest2026/cellListActive_board2_laserScanFocalLab.txt"
  if [ $3 = "SetA" ]; then
    runNrPed='168'    
#     runs='173'
    runs='169 170 171 172 173 174 175 176 177'
  elif [ $3 = "SetB" ]; then
    runNrPed='178'
    runs='179 180 181 182 183 184 185 186 187'
  ########################################################################
  ######################## OCT Laser campaign ############################
  ########################################################################
  elif [ $4 = "Oct_RF4_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='002'
    runs=''
    rm runlist.txt
    for runNr in {003..476}; do 
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V43_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='478'
    runs=''
    rm runlist.txt
    for runNr in {479..503}; do   #Set AA
      echo "$runNr " >> runlist.txt
    done
    for runNr in {530..554}; do   #Set AC
      echo "$runNr " >> runlist.txt
    done
    for runNr in {570..594}; do   #Set AE
      echo "$runNr " >> runlist.txt
    done
    for runNr in {610..634}; do   #Set AG
      echo "$runNr " >> runlist.txt
    done
    for runNr in {650..674}; do   #Set AI
      echo "$runNr " >> runlist.txt
    done
    for runNr in {695..706}; do   #Set AK
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V45_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='504'
    runs=''
    rm runlist.txt
    for runNr in {505..529}; do   #Set AB
      echo "$runNr " >> runlist.txt
    done
    for runNr in {555..569}; do   #Set AD
      echo "$runNr " >> runlist.txt
    done
    for runNr in {595..609}; do   #Set AF
      echo "$runNr " >> runlist.txt
    done
    for runNr in {635..649}; do   #Set AH
      echo "$runNr " >> runlist.txt
    done
    for runNr in {675..694}; do   #Set AJ
      echo "$runNr " >> runlist.txt
    done
    for runNr in {707..717}; do   #Set AL
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V43_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='816'
    runs=''
    rm runlist.txt
    for runNr in {817..842}; do   #Set BA
      echo "$runNr " >> runlist.txt
    done
    for runNr in {869..893}; do   #Set BC
      echo "$runNr " >> runlist.txt
    done
    for runNr in {919..956}; do   #Set BE
      echo "$runNr " >> runlist.txt
    done
    for runNr in {971..995}; do   #Set BG
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1011..1035}; do   #Set BI
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1060..1084}; do   #Set BK
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF12_V45_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='843'
    runs=''
    rm runlist.txt
    for runNr in {844..868}; do   #Set BB
      echo "$runNr " >> runlist.txt
    done
    for runNr in {894..918}; do   #Set BD
      echo "$runNr " >> runlist.txt
    done
    for runNr in {957..970}; do   #Set BF
      echo "$runNr " >> runlist.txt
    done
    for runNr in {996..1010}; do   #Set BH
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1036..1059}; do   #Set BJ
      echo "$runNr " >> runlist.txt
    done
    for runNr in {1085..1104}; do   #Set BL
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V43_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='719'
    runs=''
    rm runlist.txt
    for runNr in {720..744}; do   #Set AAA
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V45_T22" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='745'
    runs=''
    rm runlist.txt
    for runNr in {746..761}; do   #Set AAB
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V43_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='763'
    runs=''
    rm runlist.txt
    for runNr in {764..788}; do   #Set AAA
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  elif [ $4 = "Oct_RF9_V45_T-27" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='789'
    runs=''
    rm runlist.txt
    for runNr in {790..814}; do   #Set AAB
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  fi

  
  for runNr in $runs; do
    time ./HGCROCStudy -d 2 -E 2 -f -w -i $dataDirRaw/rawHGCROC_wPed_wBC_$runNr.root -o $dataDirOut/rawHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWaveOffSetCorr/Run_$runNr -r $runList -t $toaPhaseOffset #-l $cellList
  done
fi

