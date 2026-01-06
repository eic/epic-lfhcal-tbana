#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/202511_PST09/raw/TBMain2025/      # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/202511_PST09/HGCROCData           # base directory for root trees
fi

# different mapping files for layering
mapConDef=../configs/TB2025/mapping_HGCROC_PSTB2025_default.txt   # layers 0-32 equipped
mapCon2=../configs/TB2025/mapping_HGCROC_PSTB2025_config2.txt     # layers 0-24, 33-40 equipped
mapCon3=../configs/TB2025/mapping_HGCROC_PSTB2025_config3.txt     # layers 0-16, 25-32, 41-49
mapCon4=../configs/TB2025/mapping_HGCROC_PSTB2025_config4.txt     # layers 0-16, 33-40, 50-58
  
# global run list for 2025 TB
runList=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

# default path for the HGCROC analysis
if [ $2 = "pedestals" ]; then 
	mkdir -p $dataDir/
	# pedestals with different Nr of asics
# 	runs='122 123 124 125 126 128 129'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# 	# pedestals with different Nr of asics
# 	runs='036 055 056 058 059 068 130 141 156 161 207 208 209 210 259 269 270 291 292 293 295 314 315 316 317 318 321 322 323 351 381 391'
  runs='351'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
	

elif [ $2 = "muons" ]; then 
# #   # position scan
# 	mkdir -p $dataRaw
# 	runs='007 008 009 010 011 012 013 014 015 016 017 018 019 020 021 022 023 024 025 026'
# # 	runs='007 008 009 010'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# 	# HV Scan
# 	runs='027 028 029 030 031 032 033 034 035'
# # 	runs='033 034 035'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# 	
# 	# default mapping 44 V
# 	runs='069 070 071 072 073 074 075 076 157 164 201 202 203 204 205 206'
# # 	runs='076'
# # 	runs='164'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# 	
	# default mapping 45 V
# 	runs='211 '
#   runs='211 212 213 214 215 216 217 252 253 254 255 256 257 258 260 '
# #   runs='256 257 258 '
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# # 	
# 	# HV Scan
# 	runs='260 261 262 263 264 265 266 267 268 271 272'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
	
	# Hadron scan 1 44V
	runs='273 274 275 276 289'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
	
	# Hadron scan 2 44 V
	runs='296 297 298 299 300'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
	
	# Hadron scan 3 44V
	runs='319 320 324 325 326 327 328 329 330'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done

	# Hadron scan 4 44V
	runs='352 353 354 355 356'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done

	# Electron scan 42.5V
	runs='382 383 384 385'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
	
	# muons last run 44V
	runs='395'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
	
	
elif [ $2 = "MergeMuons" ]; then 	
  echo "nothing to be done"
  
  
  runs='069 070 071 072 073 074 075 076 164' # Full Set A - muon set 1
#   runs='201 202 203 204 205 206' # Full Set A - muon set 2
# 	runs='069 070 071 072 073 074 075 076 164 201 202 203 204 205 206'
	echo "" > listMerge.txt
	for runNr in $runs; do 
		ls $dataDir/rawHGCROC_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $dataDir/rawHGCROC_FullSetA_1.root $fileList

elif [ $2 = "electrons" ]; then 
	mkdir -p $dataRaw
# default mapping 44 V
# 	runs='165 166 167 168 169 170 191 192 193 194 195'
#   runs='191 192 193 194'
  runs='195'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
# # default mapping 45 V
# 	runs='189 190 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
	
# 	# electron set 42.5V
# 	runs='386 387 388 389 390'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
	
	
elif [ $2 = "positrons" ]; then 
	mkdir -p $dataRaw
# default mapping 44 V
	runs='171 172 173 174 175 196 200 199 198 197'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
elif [ $2 = "hadrons" ]; then 
  echo "converting hadrons"
  # 15 GeV h+ beginning
# 	runs='037 038 039 040 041 042 043 046 047 048 049 050 051 052 053 054'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done

#   # default mapping 44 V, h- & h+
#   runs='176 177 178 179 180 181 182 183 184 184 185 186 187 188'
# #   runs='188'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
	
# 	# default mapping 45 V, h- & h+
#   runs='237 238 239 240 241 242 243 244 245 246 247 248 249 250 251'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done

	# Hadron set 1 
#   runs='277 278 279 280 281 282 283 284 285 286 287 288'
#   runs='288'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# 
# 	# Hadron set 2 
#   runs='301 302 303 304 305 306 307 308 309 310 311 312 313'
  runs='308 309 310 311 312 313'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
# 
	# Hadron set 3 
#   runs='331 332 333 334 335 336 337 339 340 341 342 343 344'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# 	
# 	# Hadron set 4 
#   runs='357 358 359 360 361 362 363 364 365 366 367 368 369 370'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
	
fi
