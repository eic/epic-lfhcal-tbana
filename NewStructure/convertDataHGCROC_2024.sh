#! /bin/bash

if [ $1 = "fbockExt2" ]; then 
	dataDir=/media/fbock/T7/Sep2024_PS_DataBKP/dump/hadron0830/
	dataRaw=/media/fbock/T7/202408_PST09/HGCROCData
elif [ $1 = "fbockCosm" ]; then 
  dataDir=/media/fbock/T7/cosmics
	dataRaw=/media/fbock/T7/cosmics/converted
fi

# runs='306'
# runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321 276 277 278 279 280 281 282 283 299 300'
# for runNr in $runs; do 
#   ./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_PSTB_From_01092024.txt -r ../configs/DataTakingDB_202409_HGCROC.csv
# done

# local cosmics
# runs='006'
runs='007'
for runNr in $runs; do 
  ./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_Cosmics_20250606.txt -r ../configs/DataTakingDB_ORNL_HGCROC.txt
done

#! /bin/bash

if [ $1 = "fbockExt2" ]; then 
	dataDir=/media/fbock/T7/Sep2024_PS_DataBKP/dump/hadron0830/
	dataRaw=/media/fbock/T7/202408_PST09/HGCROCData
fi

# runs='306'
runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321 276 277 278 279 280 281 282 283 299 300'
for runNr in $runs; do 
  ./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_PSTB_From_01092024.txt -r ../configs/DataTakingDB_202409_HGCROC.csv
done
