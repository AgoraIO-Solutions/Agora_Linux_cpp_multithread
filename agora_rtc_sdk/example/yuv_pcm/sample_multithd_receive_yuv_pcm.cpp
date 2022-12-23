//  Agora RTC/MEDIA SDK
//
//  Created by Jay Zhang in 2020-04.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include <csignal>
#include <cstring>
#include <sstream>
#include <string>
#include <thread>
#include <ctime>
#include <cstdlib>

#include "AgoraRefCountedObject.h"
#include "IAgoraService.h"
#include "NGIAgoraRtcConnection.h"
#include "common/log.h"
#include "common/opt_parser.h"
#include "common/sample_common.h"
#include "common/sample_connection_observer.h"
#include "common/sample_local_user_observer.h"

#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraLocalUser.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "NGIAgoraMediaNode.h"
#include "NGIAgoraVideoTrack.h"
#include "jpeglib.h"

#define DEFAULT_SAMPLE_RATE (16000)
#define DEFAULT_NUM_OF_CHANNELS (1)
#define DEFAULT_AUDIO_FILE "received_audio.pcm"
#define DEFAULT_VIDEO_FILE "video/received_video"
#define DEFAULT_FILE_LIMIT (100 * 1024 * 1024)
#define STREAM_TYPE_HIGH "high"
#define STREAM_TYPE_LOW "low"

int time_20_s = 20;
int time_2_s = 2;

static bool exitFlag = false;
static void SignalHandler(int sigNo) { exitFlag = true; }
agora::rtc::RtcConnectionConfiguration ccfg;

struct SampleOptions
{
  std::string appId;
  std::string channelId;
  std::string userId = "5678";
  std::string remoteUserId;
  std::string streamType = STREAM_TYPE_HIGH;
  std::string audioFile = DEFAULT_AUDIO_FILE;
  std::string videoFile = DEFAULT_VIDEO_FILE;
  int multiChannels = 1;

  struct
  {
    int sampleRate = DEFAULT_SAMPLE_RATE;
    int numOfChannels = DEFAULT_NUM_OF_CHANNELS;
  } audio;
};

SampleOptions options;
struct VideoControl
{
  bool video_frame_saved_flag = 0;
};

class PcmFrameObserver : public agora::media::IAudioFrameObserverBase
{
public:
  PcmFrameObserver(const std::string &outputFilePath)
      : outputFilePath_(outputFilePath),
        pcmFile_(nullptr),
        fileCount(0),
        fileSize_(0) {}

  bool onPlaybackAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return true; };

  bool onRecordAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return true; };

  bool onMixedAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return true; };

  bool onPlaybackAudioFrameBeforeMixing(const char *channelId, agora::media::base::user_id_t userId, AudioFrame &audioFrame) override;

  bool onEarMonitoringAudioFrame(AudioFrame &audioFrame) { return true; };

private:
  std::string outputFilePath_;
  FILE *pcmFile_;
  int fileCount;
  int fileSize_;
};

class YuvFrameObserver : public agora::rtc::IVideoFrameObserver2
{
public:
  YuvFrameObserver(const std::string &outputFilePath, bool video_frame_saved_flag)
      : outputFilePath_(outputFilePath),
        yuvFile_(nullptr),
        jpgFile_(nullptr),
        fileCount(0),
        fileSize_(0),
        video_frame_saved_flag_(video_frame_saved_flag) {}

  void onFrame(const char *channelId, agora::user_id_t remoteUid, const agora::media::base::VideoFrame *frame) override;

  virtual ~YuvFrameObserver() = default;

private:
  std::string outputFilePath_;
  FILE *yuvFile_;
  FILE *jpgFile_;
  int fileCount;
  int fileSize_;
  int video_frame_saved_flag_;
};

static int connectWorker(agora::base::IAgoraService *service, int channel_index, bool &exitFlag)
// static int connectWorker(agora::base::IAgoraService *service, int channel_index)
{
  time_t current_conn_time;
  VideoControl saveVideoControl;
  agora::agora_refptr<agora::rtc::IRtcConnection> connection;

  // AG_LOG(INFO, "!!!!!channel index: %d", channel_index);
  while (!exitFlag)
  {
    connection = service->createRtcConnection(ccfg);
    if (!connection)
    {
      AG_LOG(ERROR, "Failed to creating Agora connection!");
      return -1;
    }

    // Subcribe streams from all remote users or specific remote user
    agora::rtc::VideoSubscriptionOptions subscriptionOptions;
    if (options.streamType == STREAM_TYPE_HIGH)
    {
      subscriptionOptions.type = agora::rtc::VIDEO_STREAM_HIGH;
    }
    else if (options.streamType == STREAM_TYPE_LOW)
    {
      subscriptionOptions.type = agora::rtc::VIDEO_STREAM_LOW;
    }
    else
    {
      AG_LOG(ERROR, "It is a error stream type");
      return -1;
    }
    if (options.remoteUserId.empty())
    {
      AG_LOG(INFO, "Subscribe streams from all remote users");
      connection->getLocalUser()->subscribeAllAudio();
      connection->getLocalUser()->subscribeAllVideo(subscriptionOptions);
    }
    else
    {
      connection->getLocalUser()->subscribeAudio(options.remoteUserId.c_str());
      connection->getLocalUser()->subscribeVideo(options.remoteUserId.c_str(),
                                                 subscriptionOptions);
    }
    // Register connection observer to monitor connection event
    auto connObserver = std::make_shared<SampleConnectionObserver>();
    connection->registerObserver(connObserver.get());

    // Create local user observer
    auto localUserObserver =
        std::make_shared<SampleLocalUserObserver>(connection->getLocalUser());
#if 0
    // Register audio frame observer to receive audio stream
    auto pcmFrameObserver = std::make_shared<PcmFrameObserver>(options.audioFile);

    if (connection->getLocalUser()->setPlaybackAudioFrameBeforeMixingParameters(
            options.audio.numOfChannels, options.audio.sampleRate))
    {
      AG_LOG(ERROR, "Failed to set audio frame parameters!");
      return -1;
    }
    localUserObserver->setAudioFrameObserver(pcmFrameObserver.get());
#endif
    // Register video frame observer to receive video stream
    std::shared_ptr<YuvFrameObserver> yuvFrameObserver =
        // std::make_shared<YuvFrameObserver>(options.videoFile);
        std::make_shared<YuvFrameObserver>(options.videoFile, saveVideoControl.video_frame_saved_flag);
    localUserObserver->setVideoFrameObserver(yuvFrameObserver.get());

    // Connect to Agora channel
    if (connection->connect(options.appId.c_str(), (options.channelId + to_string(channel_index)).c_str(),
                            options.userId.c_str()))
    {
      AG_LOG(ERROR, "Failed to connect to Agora channel!");
      return -1;
    }

    // reset timer and flag
    current_conn_time = time(0);
    saveVideoControl.video_frame_saved_flag = 0;
    // Periodically check if in the channel for 2s
    while ((time(0) - current_conn_time <= time_2_s) && (!exitFlag))
    {
      usleep(500000);
    }
    // Unregister audio & video frame observers
    //localUserObserver->unsetAudioFrameObserver();
    localUserObserver->unsetVideoFrameObserver();

    // Unregister connection observer
    connection->unregisterObserver(connObserver.get());

    // Disconnect from Agora channel
    if (connection->disconnect())
    {
      AG_LOG(ERROR, "Failed to disconnect from Agora channel!");
      return -1;
    }
    AG_LOG(INFO, "Disconnected from Agora channel successfully");

    // Destroy Agora connection and related resources
    localUserObserver.reset();
    //pcmFrameObserver.reset();
    yuvFrameObserver.reset();
    connection = nullptr;

    // Periodically check if it has been 20s
    while (((time(0) - current_conn_time) < time_20_s) && (!exitFlag))
    {
      // AG_LOG(INFO, "channel index: %d", channel_index);
      // usleep(5000000); // 5s
      sleep(2); // 5s
    }
  };
  return 0;
}

bool PcmFrameObserver::onPlaybackAudioFrameBeforeMixing(const char *channelId, agora::media::base::user_id_t userId, AudioFrame &audioFrame)
{
  // Create new file to save received PCM samples
  if (!pcmFile_)
  {
    std::string fileName = (++fileCount > 1)
                               ? (outputFilePath_ + to_string(fileCount))
                               : outputFilePath_;
    if (!(pcmFile_ = fopen(fileName.c_str(), "w")))
    {
      AG_LOG(ERROR, "Failed to create received audio file %s",
             fileName.c_str());
      return false;
    }
    AG_LOG(INFO, "Created file %s to save received PCM samples",
           fileName.c_str());
  }

  // Write PCM samples
  size_t writeBytes =
      audioFrame.samplesPerChannel * audioFrame.channels * sizeof(int16_t);
  if (fwrite(audioFrame.buffer, 1, writeBytes, pcmFile_) != writeBytes)
  {
    AG_LOG(ERROR, "Error writing decoded audio data: %s", std::strerror(errno));
    return false;
  }
  fileSize_ += writeBytes;

  // Close the file if size limit is reached
  if (fileSize_ >= DEFAULT_FILE_LIMIT)
  {
    fclose(pcmFile_);
    pcmFile_ = nullptr;
    fileSize_ = 0;
  }
  return true;
}

void YuvFrameObserver::onFrame(const char *channelId, agora::user_id_t remoteUid, const agora::media::base::VideoFrame *videoFrame)
{
  // check to see if frame is already saved
  if (video_frame_saved_flag_)
    return;
  // Create new file to save received YUV frames
  std::string fileName;
  std::string fileNameJpg;
#if 0
  std::string fileNameYUV;
  std::string command;
  if (!yuvFile_)
  {
    fileName = (++fileCount > 1)
                   ? (outputFilePath_ + "_" + channelId + "_" + to_string(fileCount))
                   : outputFilePath_ + "_" + channelId + "_" + to_string(time(0));
    fileNameYUV = fileName + ".yuv";
    fileNameJpg = fileName + ".jpg";
    if (!(yuvFile_ = fopen(fileNameYUV.c_str(), "w+")))
    {
      AG_LOG(ERROR, "Failed to create received video file %s",
             fileNameYUV.c_str());
      return;
    }
    AG_LOG(INFO, "Created file %s to save received YUV frames",
           fileNameYUV.c_str());
  }

  // Write Y planar
  size_t writeBytes = videoFrame->yStride * videoFrame->height;
  if (fwrite(videoFrame->yBuffer, 1, writeBytes, yuvFile_) != writeBytes)
  {
    AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
    return;
  }
  fileSize_ += writeBytes;

  // Write U planar
  writeBytes = videoFrame->uStride * videoFrame->height / 2;
  if (fwrite(videoFrame->uBuffer, 1, writeBytes, yuvFile_) != writeBytes)
  {
    AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
    return;
  }
  fileSize_ += writeBytes;

  // Write V planar
  writeBytes = videoFrame->vStride * videoFrame->height / 2;
  if (fwrite(videoFrame->vBuffer, 1, writeBytes, yuvFile_) != writeBytes)
  {
    AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
    return;
  }
  fileSize_ += writeBytes;

  // Close the file if size limit is reached
  // if (fileSize_ >= DEFAULT_FILE_LIMIT)
  {
    fclose(yuvFile_);
    yuvFile_ = nullptr;
    fileSize_ = 0;
  }

  // convert to jpg format
  command = "ffmpeg -f rawvideo -vcodec rawvideo -s " + to_string(videoFrame->yStride) + "x" + to_string(videoFrame->height) + " -r 1 -pix_fmt yuv420p -i " + fileNameYUV + " -preset ultrafast -qp 0 " + fileNameJpg;
  //system(command.c_str());
  //AG_LOG(INFO, "ffmpeg conver YUV to jpeg, command: %s", command.c_str());

#else
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  if (!jpgFile_)
  {
    fileName = (++fileCount > 1)
                   ? (outputFilePath_ + "_" + channelId + "_" + to_string(fileCount))
                   : outputFilePath_ + "_" + channelId + "_" + to_string(time(0));
    fileNameJpg = fileName + ".jpg";
    if (!(jpgFile_ = fopen(fileNameJpg.c_str(), "wb")))
    {
      AG_LOG(ERROR, "Failed to create received video file %s",
             fileNameJpg.c_str());
      return;
    }
    AG_LOG(INFO, "Created file %s to save received JPEG frames",
           fileNameJpg.c_str());
  }
  int width = videoFrame->yStride;
  unsigned char *yuvbuf = NULL;
  if ((yuvbuf = (unsigned char *)malloc(width * 3)) == NULL)
  {
    AG_LOG(ERROR, "yuv buf malloc failed: %s", std::strerror(errno));
    return;
  }
  //memset(yuvbuf, 0, width * 3);

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  /* save to a file */
  jpeg_stdio_dest(&cinfo, jpgFile_);
  cinfo.image_width = videoFrame->yStride;
  cinfo.image_height = videoFrame->height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_YCbCr; /*  YUV444  */
  cinfo.dct_method = JDCT_FLOAT;
  jpeg_set_defaults(&cinfo);

  /* set jpeg image quality，range [0,100] */
  jpeg_set_quality(&cinfo, 40, TRUE);

  /* start */
  jpeg_start_compress(&cinfo, TRUE);

  unsigned char *ybase, *ubase, *vbase;
  ybase = videoFrame->yBuffer;
  ubase = videoFrame->uBuffer;
  vbase = videoFrame->vBuffer;
  /* process data */
  JSAMPROW row_pointer[1];
  int j = 0;
  int idx;
  while (cinfo.next_scanline < cinfo.image_height)
  {
    idx = 0;
    for (int i = 0; i < width; i++) /* convert yuv420p to yuv444 */
    {
      yuvbuf[idx++] = ybase[i + j * width];
      yuvbuf[idx++] = ubase[j / 4 * width + (i / 2)];
      yuvbuf[idx++] = vbase[j / 4 * width + (i / 2)];
    }
    row_pointer[0] = yuvbuf;
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
    j++;
  }

  /* stop */
  jpeg_finish_compress(&cinfo);
  /* destroy */
  jpeg_destroy_compress(&cinfo);
  fclose(jpgFile_);
  jpgFile_ = nullptr;
  if (yuvbuf)
  {
    free(yuvbuf);
    yuvbuf = NULL;
  }
  //AG_LOG(INFO, "libjpeg convert YUV to jpeg");
#endif
  video_frame_saved_flag_ = 1;
  return;
};

#define MAX_NUM_OF_THREAD 100

int main(int argc, char *argv[])
{
  opt_parser optParser;
  std::thread *th_array = new std::thread[MAX_NUM_OF_THREAD];

  optParser.add_long_opt("token", &options.appId,
                         "The token for authentication");
  optParser.add_long_opt("channelId", &options.channelId, "Channel Id");
  optParser.add_long_opt("userId", &options.userId, "User Id / default is 0");
  optParser.add_long_opt("remoteUserId", &options.remoteUserId,
                         "The remote user to receive stream from");
  optParser.add_long_opt("audioFile", &options.audioFile, "Output audio file");
  optParser.add_long_opt("videoFile", &options.videoFile, "Output video file");
  optParser.add_long_opt("multiChannels", &options.multiChannels, "Num multithread channels, no more than 100");
  optParser.add_long_opt("sampleRate", &options.audio.sampleRate,
                         "Sample rate for received audio");
  optParser.add_long_opt("numOfChannels", &options.audio.numOfChannels,
                         "Number of channels for received audio");
  optParser.add_long_opt("streamtype", &options.streamType, "the stream type");

  if ((argc <= 1) || !optParser.parse_opts(argc, argv))
  {
    std::ostringstream strStream;
    optParser.print_usage(argv[0], strStream);
    std::cout << strStream.str() << std::endl;
    return -1;
  }

  if (options.appId.empty())
  {
    AG_LOG(ERROR, "Must provide appId!");
    return -1;
  }

  if (options.channelId.empty())
  {
    AG_LOG(ERROR, "Must provide channelId!");
    return -1;
  }

  std::signal(SIGQUIT, SignalHandler);
  std::signal(SIGABRT, SignalHandler);
  std::signal(SIGINT, SignalHandler);

  // Create Agora service
  auto service = createAndInitAgoraService(false, true, true);
  if (!service)
  {
    AG_LOG(ERROR, "Failed to creating Agora service!");
  }

  // Create Agora connection
  agora::rtc::AudioSubscriptionOptions audioSubOpt;
  audioSubOpt.bytesPerSample = sizeof(int16_t) * options.audio.numOfChannels;
  audioSubOpt.numberOfChannels = options.audio.numOfChannels;
  audioSubOpt.sampleRateHz = options.audio.sampleRate;

  //  agora::rtc::RtcConnectionConfiguration ccfg;
  ccfg.clientRoleType = agora::rtc::CLIENT_ROLE_AUDIENCE;
  ccfg.audioSubscriptionOptions = audioSubOpt;
  ccfg.autoSubscribeAudio = false;
  ccfg.autoSubscribeVideo = false;
  ccfg.enableAudioRecordingOrPlayout =
      false; // Subscribe audio but without playback

  //  start the connect -> save frame -> disconnect loop

  for (int i = 0; i < options.multiChannels; ++i)
  {
    // AG_LOG(INFO, "!!!!!!!!!! index: %d", i);
    th_array[i] = std::thread(connectWorker, service, i, std::ref(exitFlag));
    usleep(300000); // add a pacing
  }

  for (int i = 0; i < options.multiChannels; ++i)
  {
    th_array[i].join();
  }

  delete[] th_array;
  // Destroy Agora Service
  service->release();
  service = nullptr;

  return 0;
}
