
## Compile Agora SDK Samples（For Linux）

```
$ ./build.sh
$ ./sync-data.sh

Upon successful compilation, there are a couple of **sample_xxx"" excutables in out folder
Also, download the media files by running sync-data.sh script. These audio/video files will be used together with the sample code.
```


## Run Agora SDK Samples

**sample_send_aac** reads from an AAC file and sends the AAC stream to Agora channel

#### Example:
```
$cd out
$export LD_LIBRARY_PATH=../../agora_sdk:$LD_LIBRARY_PATH
To send aac stream to Agora channel named "demo_channel". Note that `xxxxxx` should be replaced with your own App ID or token
$ ./sample_send_aac --token XXXXXX --channelId demo_channel --audioFile test_data/send_audio.aac
```
To receive stream and take snapshots of video from remove user 1234, every 20s from multi-channels using multi-threading implementation
$./sample_multithd_receive_yuv_pcm --token XXXXXX --channelId multidemo --multiChannels 10 —remoteUserId 1234
