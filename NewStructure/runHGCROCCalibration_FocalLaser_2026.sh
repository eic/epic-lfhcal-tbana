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
  dataDirRaw=/media/fbock/ALICE2-4TB/202609_FocalLaser/HGCROCData
  dataDirOut=/media/fbock/ALICE2-4TB/202609_FocalLaser/HGCROCData
  PlotBaseDir=/media/fbock/ALICE2-4TB/202609_FocalLaser/
  runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202609.csv
elif [ $1 = "fbockOct" ]; then 
  dataDirRaw=/media/fbock/ALICE2-4TB/202610_FocalLaser/HGCROCData
  dataDirOut=/media/fbock/ALICE2-4TB/202610_FocalLaser/HGCROCData
  PlotBaseDir=/media/fbock/ALICE2-4TB/202610_FocalLaser/
  runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202610.csv
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
  if [ $3 = "Sept" ]; then 
    runs='166 167 168 178 188 198 208 210 212 213 216 218 221 258 268 279 309 334 359 372 386 401 416 431'
  else 
    runs='1 2'
  fi
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    Pedestal $2 $runNrPed $dataDirRaw $dataDirOut Run$runNrPed pdf
  done
fi

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
  elif [ $3 = "Oct_SetA" ]; then
    runNrPed='002'
#     runs='003 004 005 006 007 008 009 010 011 012 013 014 015 016 017 018 019 020'
    runs=''
#     for runNr in {021..135}; do 
    rm runlist.txt
    for runNr in {141..422}; do 
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
#     echo $runs
  fi
  
  for runNr in $runs; do
    time ./HGCROCStudy -d 2 -E 2 -f -w -i $dataDirRaw/rawHGCROC_wPed_wBC_$runNr.root -o $dataDirOut/rawHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWaveOffSetCorr/Run_$runNr -r $runList -t $toaPhaseOffset #-l $cellList
  done
fi


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
  elif [ $4 = "Oct_SetA" ]; then
    toaPhaseOffset='../configs/FOCalTest2026/toAOffset_SetA.txt'
    badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt

    runNrPed='002'
    runs=''
    rm runlist.txt
    for runNr in {141..422}; do 
      echo "$runNr " >> runlist.txt
    done
    runs=`cat runlist.txt`
  fi
  
  for runNr in $runs; do
    MuonCalibHGCROC $3 $runNrPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset 	
  done
fi
