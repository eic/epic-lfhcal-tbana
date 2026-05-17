
#!/bin/bash

RED='\033[0;31m'
NC='\033[0m' # No Color

DAQ_HOST=tsukuba-pc-2022


# step1: copy lfhcaldaq:disk1 to lfhcaldaq:disk2 
#echo "Attempting to copy lfhcaldq:disk1 to lfhcaldaq:disk2"
#ssh $DAQ_HOST "rsync -av --info=progress2 /home/lfhcal/Data/202605_SPSH2/ /home/lfhcal/Data/disk2/202605_SPSH2/"
#echo "DAQ_HOST disk 1 copied to disk 2"

echo "Attempting to copy lfhcaldq:disk1 to lfhcaldaq:disk2"
ssh $DAQ_HOST "rsync -av --info=progress2 /home/lfhcal/Data/202605_SPSH2/ /media/lfhcal/Expansion/202605_SPSH2/"
echo "DAQ_HOST disk 1 copied to disk 3"

ssh $DAQ_HOST "rm -rf /home/lfhcal/Software/HGCROC/h2g_online_monitoring/monitoring_plots/*/*.root"

# # step2: copy lfhcaldaq:disk2 to here
# echo "Attempting to copy from lfhcaldaq:disk2 to this machine"
# #rsync -avz --info=progress2 lfhcaldaq:/home/lfhcal/Data/disk2/202604_PST10/ /home/lfhcal/Data/202604_PST10/
# rsync -avz --info=progress2 DAQ_HOST:/home/lfhcal/Data/disk2/202604_PST10/ /media/lfhcal/RNP2/202604_PST10/
# echo "Copied data files from DAQ_HOST:disk2 to this machine"

# # step 3: check if USB1 exists

# echo copying to USB1
# # Source - https://stackoverflow.com/a/59839
# # Posted by Grundlefleck, modified by community. See post 'Timeline' for change history
# # Retrieved 2026-04-11, License - CC BY-SA 4.0
# #DIRECTORY1="/media/lfhcal/RNP1"
# #if [ -d "$DIRECTORY1" ]; then
# #  echo "$DIRECTORY1 does exist."
# #  rsync -av --info=progress2 /home/lfhcal/Data/202604_PST10 $DIRECTORY1
# #  echo copied to USB1
# #else 
# #  echo "$DIRECTORY1 does not exist"
# #fi
# # step 4: rsync client SSD to USB1

# #step 5: check if USB2 exists
# #echo copying to USB2
# #DIRECTORY2="/media/lfhcal/RNP2"
# #if [ -d "$DIRECTORY2" ]; then
# #  echo "$DIRECTORY2 does exist."
# #  rsync -av --info=progress2 /home/lfhcal/Data/202604_PST10 $DIRECTORY2
# #  echo copied to USB2
# #else 
# #  echo "$DIRECTORY2 does not exist"
# #fi


# # here is how to check all /media drives for a folder on it:
# for datadir in /media/lfhcal/*/202604_PST10; do 
#   if [ -d $datadir ]; then 
#     echo found 202604_PST10 folder here: $datadir. copying.; 
#     #rsync -av --info=progress2 /home/lfhcal/Data/202604_PST10/ $datadir
#     rsync -av --info=progress2 /media/lfhcal/RNP2/202604_PST10/ $datadir
#     echo copied to $datadir
#   else 
#     echo z; 
#   fi; 
# done



# step7: get free disk space on primary data drive:
avail=$(ssh $DAQ_HOST 'df --output=avail /home/lfhcal/Data/ |tail -n1')
if [ $avail -lt $((50*1024*1024)) ]; then
  echo -e "${RED}LFHCALDAQ PRIMARY DISK IS RUNNING LOW ($(($avail / 1024 / 1024))gb remaining) ${NC}"
else
  echo $(($avail / 1024 / 1024)) gb of disk space left on lfhcaldaq primary disk
fi 



