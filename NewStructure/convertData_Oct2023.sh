#! /bin/bash
basedir=""
if [ $1 = "fbock" ]; then 
  basedir=/media/fbock/Samsung_T5/LFHCAL_TB/202310_PST9
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

if [ $2 == "single" ]; then 
  runs='200'
  dataDir=$basedir/231011N/RAW
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d  -f -m ../configs/mappingTBSetupNewStructure_11102024_PS.txt -r ../configs/DataTakingDB_TBOct2023_PST09-20231011.csv
  done;
else 
#   dataDir=$basedir/231011N/RAW
#   # 201 202 203 204 205  # not correct format
#   runs='200 206 207 208 210 212 213 214 215 216 217 218 219 220 221 222 223 224' 
#   for runNr in $runs; do 
#     ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m ../configs/mappingTBSetupNewStructure_11102024_PS.txt -r ../configs/DataTakingDB_TBOct2023_PST09-20231011.csv
#   done;
#   dataDir=$basedir/231012D/RAW
# #   runs='225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245' 
#   runs='225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 251 252 253 254 255 256 257 258 259 260 261 262 263 264 265 269 270 271 272 273 274 275 276 277 278 279 280 281 282 283 284 285 286 287 288 289 290 291 292 293 294 295 296 297 298 299 300 301 302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321 322 323 324 325 326 327 328 329 330 331 332 333 334 335 336 337 338 339 340 341 342 343 344 345 346 347 348 349 350 351 352 353 354 355 356 357 358 359 360 361 362 363 364 365 366 367 368 369 370 371 372 373 374 375 376 377 378 379 380 381 382' 
#   for runNr in $runs; do 
#     ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m ../configs/mappingTBSetupNewStructure_12102024_PS.txt -r ../configs/DataTakingDB_TBOct2023_PST09-20231012.csv
#   done;
  dataDir=$basedir/231016D/RAW
  runs='383 384 385 386 387 388 389 390 391 392 393 394 395 396 397 398 399 400 401 402 403 404 405 406 407 408 409 410 411 412 413 414 415 416 417 418 419 420 421 422 423 424 425 426 427 428 429 430 431 432 433 434 435 436 437 438 439 440 441 442 443 444 445 446 447 448 449 450 451 452 453 454 455 456 457 458 459 460 461 462 463 464 465 466 467 468 469 470 471'
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m ../configs/mappingTBSetupNewStructure_12102024_PS.txt -r ../configs/DataTakingDB_TBOct2023_PST09-20231016.csv
  done;
  
  
fi
