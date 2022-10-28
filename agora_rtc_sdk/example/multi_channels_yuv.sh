#!/bin/bash
if [ $# -eq 0 ]
  then
    echo "needs an arg: the number of channels"
    exit
fi
OUTDIR=out
cd $OUTDIR
export LD_LIBRARY_PATH=../../agora_sdk:$LD_LIBRARY_PATH
BASE_CHANNEL_NAME=tmgscreencapture
USER_ID=5678
echo "Local user id is $USER_ID"
VIDEODIR=video
echo "video frames are saved under $VIDEODIR"
if [ ! -d "$VIDEODIR" ]; then
   mkdir $VIDEODIR
fi	
echo "
./sample_receive_yuv_pcm --token STRING --channelId STRING --userId STRING --remoteUserId STRING --audioFile STRING --videoFile STRING --sampleRate INTEGER32 --numOfChannels INTEGER32 --streamtype STRING
                           --token     (The token for authentication)
                           --channelId     (Channel Id)
                           --userId     (User Id / default is 0)
                           --remoteUserId     (The remote user to receive stream from)
                           --audioFile     (Output audio file)
                           --videoFile     (Output video file)
                           --sampleRate     (Sample rate for received audio)
                           --numOfChannels     (Number of channels for received audio)
                           --streamtype     (the stream type)
"
#APP ID or token
TOKEN=d6xxxxxxxxxxxxxxxxxx
REMOTE_UID=1234
  for (( i=1 ; i<=$1 ; i++ )); 
   do	  
   CHANNEL_NAME=$BASE_CHANNEL_NAME$i	  
   ./sample_receive_yuv_pcm --token $TOKEN --channelId $CHANNEL_NAME --remoteUserId $REMOTE_UID --userId $USER_ID > /dev/null 2>&1 &
   current_pid=`pidof ./sample_receive_yuv_pcm`
done
#echo $current_pid
pids=`pgrep sample_`
echo "pids are below:"
echo $pids
read -p "kill the processes: Y/N " KILLFLAG
echo $KILLFLAG
if [ "$KILLFLAG" = "Y" ]; then
   `kill -9 $pids`
else 
     if [ "$KILLFLAG" = "y" ]; then
     `kill -9 $pids`
     fi     
fi
