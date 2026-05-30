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
#     runs='023 024 025 026 027 028 029'
    runs='017'
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
#       runs='071 126' #pedestals
#       runs='072 073 074 075 076 077 078 079 080 081 082 083 084' #muons set 1
#       runs='130 131 132 133' #muons set 2
#       runs='085 086 087 088 089 089 090' #e-
#       runs='091 092 093 094 095 096 097 ' #e+
#       runs='113 114 115 116     runs='195' #mu117 118 119 120 121 122 123 124 125' #pi-
#       runs='098 099 100 101 102 103 104 105 106 107 108 109 110 111 112' #h+
#       runs='100 101 102 103 104 105 106 107 108 109 110 111 112' #h+
    runs='127 128 129 136 087' #additional runs
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='072 073 074 075 076 077 078 079 080 081 082 083 084' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_1  #ok
    runs='130 131 132 133' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_2  #ok
  fi
# 44 V, summing board V2, Preamp settings 9 7 10 5
elif [ $2 = "FullSetC" ]; then 
  if [ $3 = "convert" ]; then 
  #     runs='134 135 137 188 278 147 148 149 146 145 144 141 142 143 140 139 138 189 194 190 191 193 192 289 290 291 292 293 150 151 152 155 156 157 159 160 161 279 280 162 163 164 165 166 167 168 169 170 171 172 173 281 174 175 176 177 178 179 181 183 186 187 282 283 284 285 286 287 288' #full list
#       runs='134 135 137 188 278' #pedestals
#       runs='147 148 149 146 145 144 141 142 143 140 139 138' #muons set 1
#       runs='189 194 190 191 193 192' #muons set 2
#       runs='289 290 291 292 293' #muons set 3
#       runs='150 151 152 155 ' #e-
#       runs='156 157 159 160 161 279 280' #e+
#       runs='162 163 164 165 166 167 168 169 170 171 172 173 281' #pi-
#       runs='174 175 176 177 178 179 181 183 186 187 282 283 284 285 286 287 288 ' #h+
    runs='278 279 280 281 282 283 284 285 286 287 288 289 290 291 292 293' #make up runs
#       runs='184 185' #broken (no events)
#       runs='180 182 158 154 153 144' #out of sync
    for runNr in $runs; do 
      echo $runNr
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='147 148 149 146 145 144 141 142 142 140 139 138' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetC_1  #ok
    runs='189 194 190 191 193 192' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetC_2  #ok
    runs='289 290 291 292 293' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetC_3  #ok
  fi
  
# 45 V, summing board V2, Preamp settings 9 7 10 4?
elif [ $2 = "FullSetD" ]; then
  if [ $3 = "convert" ]; then
#     runs='206 263 264 265 275 276 277 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224 266 267 268 269 270 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 256 257 258 259 260 261 262 271 272 273 274' #full list
#     runs='206 263 264 265 275 276 277' #pedestals
#     runs='208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224' #muons set 1
#     runs='266 267 268 269 270' #muons set 2
#     runs='225 226 227 228 229 230 231' #e-
#     runs='232 233 234 235 236 237' #e+
#     runs='238 239 240 241 242 243 244 245 246 247 248 249' #pi-
#     runs='250 251 252 253 254 255 256 257 258 259 260 261 262 271 272 273 274 ' #h+
#     runs='231 245 267' #out of sync
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then
    runs='208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetD_1  #ok
    runs='266 267 268 269 270' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetD_2  #ok
  fi

# 44 V, summing board V2, Preamp settings 12 7 3 1
elif [ $2 = "FullSetE" ]; then 
  if [ $3 = "convert" ]; then 
#       runs='372 420 374 375 376 377 378 421 422 423 425 424 379 380 381 382 383 384 391 39 385 386 387 388 389 390 393 394 395 396 397 398 399 400 401 402 403 404 405 406 407 408 409 410 411 412 413 414 415 416 417 418 419' #full list
#       runs='372 420' #pedestals
#       runs='374 375 376 377 378' #muons set 1
#       runs='421 422 423 425 424' #muons set 2
#       runs='473 474 477 478 481 482' #muons set 3
#       runs='379 380 381 382 383 384 391 392' #e-
#       runs='379 380' #e-
#       runs='385 386 387 388 389 390' #e+
#       runs='393 394 395 396 397 398 399 400 401 402 403 404' #pi-
#       runs='405 406 407 408 409 410 411 412 413 414 415 416 417 418 419' #h+
    runs='371 373' #additional runs
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='374 375 376 377 378' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetE_1  #ok
    runs='421 422 423 425 424' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetE_2  #ok
    runs='473 474 477 478 481 482' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetE_2  #ok
  fi

# 45 V, summing board V2, Preamp settings 12 7 3 1
elif [ $2 = "FullSetF" ]; then 
  if [ $3 = "convert" ]; then 
#       runs='431 471 426 427 428 429 430 472 475 476 479 480 483 432 433 434 435 436 437 438 439 440 441 442 443 444 445 446 447 448 449 450 451 452 453 454 455 456 457 458 459 460 461 462 463 464 465 466 467 468 469 470' #full list
#       runs='431 471' #pedestals
#       runs='426 427 428 429 430' #muons set 1
#       runs='472 475 476 479 480 483' #muons set 2
#       runs='432 433 434 435 436 437' #e-
#       runs='438 439 440 441 442 443' #e+
#       runs='444 445 446 447 448 449 450 451 452 453 454 455' #pi-
      runs='456 457 458 459 460 461 462 463 464 465 466 467 468 469 470' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='426 427 428 429 430' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetF_1  #ok
    runs='472 475 476 479 480 483' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetF_2  #ok
  fi
  
# 45 V, summing board V1, Preamp settings 12 7 3 1
elif [ $2 = "FullSetG" ]; then 
  if [ $3 = "convert" ]; then 
      runs='485 529 484 486 487 488 489 490 491 530 531 532 533 534 535 536 537 517 519 521 523 525 527 518 520 522 524 526 528 492 494 496 498 500 502 504 506 508 510 512 514 493 495 497 499 501 503 505 507 509 511 513 515 516' #full list
#       runs='485 529 ' #pedestals
#       runs='484 486 487 488 489 490 491' #muons set 1
#       runs='530 531 532 533 534 535 536 537' #muons set 2
#       runs='517 519 521 523 525 527' #e-
#       runs='518 520 522 524 526 528' #e+
#       runs='492 494 496 498 500 502 504 506 508 510 512 514 ' #pi-
#       runs='493 495 497 499 501 503 505 507 509 511 513 515 516' #h+
    for runNr in $runs; do 
      echo $runNr
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='484 486 487 488 489 490 491' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetG_1  #ok
    runs='530 531 532 533 534 535 536 537' # set 2
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetG_2  #ok
  fi
  
elif [ $2 = "ParameterScan" ]; then
  if [ $3 = "convert" ]; then
    runs='294 295 296 297 298 299 300 301 302 303 304 305 306 307 308 309 328 329 330 310 331 332 333 334 335 336 337 338 339 340 341 342 343 344 345 346 347 348 349 350 351 352 353 354 355 356 357 358 359 360 361 362 363 364 365 366 367 368 369' #full list
    for runNr in $runs; do
      echo $runNr
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  fi
elif [ $2 = "HVScan" ]; then
  if [ $3 = "convert" ]; then
    runs='194 195 196 197 198 199 200 201 202' #mu
#     runs='188' #pedestal
    for runNr in $runs; do
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  fi

fi
