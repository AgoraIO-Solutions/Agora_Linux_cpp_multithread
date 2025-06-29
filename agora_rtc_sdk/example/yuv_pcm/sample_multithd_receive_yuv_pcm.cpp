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
#define COMPOSITE 1

int time_in_s = 5;
int time_ss_s = 1;

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
  bool *video_frame_saved_flag;
};

class PcmFrameObserver : public agora::media::IAudioFrameObserverBase
{
public:
  PcmFrameObserver(const std::string &outputFilePath)
      : outputFilePath_(outputFilePath),
        pcmFile_(nullptr),
        fileCount(0),
        fileSize_(0)
  {
  }

  bool onPlaybackAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return true; };

  bool onRecordAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return true; };

  bool onMixedAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return true; };

  bool onPlaybackAudioFrameBeforeMixing(const char *channelId, agora::media::base::user_id_t userId, AudioFrame &audioFrame) override;

  bool onEarMonitoringAudioFrame(AudioFrame &audioFrame) override { return true; };

  AudioParams getEarMonitoringAudioParams() override { return AudioParams(); };

  int getObservedAudioFramePosition() override { return 0; };

  AudioParams getPlaybackAudioParams() override { return AudioParams(); };

  AudioParams getRecordAudioParams() override { return AudioParams(); };

  AudioParams getMixedAudioParams() override { return AudioParams(); };

private:
  std::string outputFilePath_;
  FILE *pcmFile_;
  int fileCount;
  int fileSize_;
};

class YuvFrameObserver : public agora::rtc::IVideoFrameObserver2
{
public:
  YuvFrameObserver(const std::string &outputFilePath, bool *video_frame_saved_flag)
      : outputFilePath_(outputFilePath),
#if COMPOSITE
        yuvFileCom_(nullptr),
        jpgFileCom_(nullptr),
        fileCountCom(0),
        fileSizeCom_(0),
        ybufCom_(nullptr),
        ubufCom_(nullptr),
        vbufCom_(nullptr),
        hostProcessedNum_(0),
#endif
        yuvFile_(nullptr),
        jpgFile_(nullptr),
        fileCount(0),
        fileSize_(0),
        video_frame_saved_flag_(video_frame_saved_flag)
  {
  }

  void onFrame(const char *channelId, agora::user_id_t remoteUid, const agora::media::base::VideoFrame *frame) override;

  virtual ~YuvFrameObserver() = default;

private:
  std::string outputFilePath_;
#if COMPOSITE
  FILE *yuvFileCom_;
  FILE *jpgFileCom_;
  std::string fileNameJpgCom_;
  std::string fileNameYUVCom_;
  unsigned char *ybufCom_;
  unsigned char *ubufCom_;
  unsigned char *vbufCom_;
  int fileCountCom;
  int fileSizeCom_;
  int hostProcessedNum_;
#endif
  FILE *yuvFile_;
  FILE *jpgFile_;
  int fileCount;
  int fileSize_;
  bool *video_frame_saved_flag_;
};

static int connectWorker(agora::base::IAgoraService *service, int channel_index, bool &exitFlag)
// static int connectWorker(agora::base::IAgoraService *service, int channel_index)
{
  time_t current_conn_time;
  VideoControl saveVideoControl;
  agora::agora_refptr<agora::rtc::IRtcConnection> connection;
  bool save_file_flag;
  saveVideoControl.video_frame_saved_flag = &save_file_flag;

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
    *(saveVideoControl.video_frame_saved_flag) = 0;

    // Register video frame observer to receive video stream
    std::shared_ptr<YuvFrameObserver>
        yuvFrameObserver =
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

    // Periodically check if in the channel for 2s
    while ((!*saveVideoControl.video_frame_saved_flag || (time(0) - current_conn_time <= time_ss_s)) && (!exitFlag))
    {
      usleep(100000);
    }

    // Unregister audio & video frame observers
    // localUserObserver->unsetAudioFrameObserver();
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
    // pcmFrameObserver.reset();
    yuvFrameObserver.reset();
    connection = nullptr;

    // Periodically check if it has been 20s
    while (((time(0) - current_conn_time) < time_in_s) && (!exitFlag))
    {
      // AG_LOG(INFO, "channel index: %d", channel_index);
      // usleep(5000000); // 5s
      sleep(1); // 5s
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
  if (*video_frame_saved_flag_)
  {
    AG_LOG(INFO, "YUV or jpeg already saved, channel index %s", channelId);
    return;
  }
  else
  {
    AG_LOG(INFO, "Frame observer channel index %s, %s", channelId, remoteUid);
  }
  // Create new file to save received YUV frames
  std::string fileName;
  std::string fileNameJpg;
#if 1
  std::string fileNameYUV;
  std::string command;
#if 0
  if (!yuvFile_)
  {
    fileName = (++fileCount > 1)
                   ? (outputFilePath_ + "_" + channelId + "_" + remoteUid + "_" + to_string(fileCount))
                   : outputFilePath_ + "_" + channelId + "_" + remoteUid + "_" + to_string(time(0));
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
  // command = "ffmpeg -f rawvideo -vcodec rawvideo -s " + to_string(videoFrame->yStride) + "x" + to_string(videoFrame->height) 
  + " -r 1 -pix_fmt yuv420p -i " + fileNameYUV + " -preset ultrafast -qp 0 " + fileNameJpg;
  // system(command.c_str());
  // AG_LOG(INFO, "ffmpeg conver YUV to jpeg, command: %s", command.c_str());
#endif
#if COMPOSITE
#define NUM_OF_HOST 4
  // std::string fileNameCom;
  int verticalIndex[2] = {0, 1};
  int horizontalIndex[2] = {0, 1};
  int strideOffset, heightOffset; // assuming all video streams have the same size.

  
  if (!yuvFileCom_)
  {
    fileNameYUVCom_ = (outputFilePath_ + "_" + channelId + "_" + "multiuser" + "_" + to_string(time(0)) + ".yuv");

    fileNameJpgCom_ = fileNameYUVCom_ + ".jpg";
    AG_LOG(INFO, "Created a file %s to save received YUV frames for composite",
           fileNameYUVCom_.c_str());

    if (!(yuvFileCom_ = fopen(fileNameYUVCom_.c_str(), "w+")))
    {
      AG_LOG(ERROR, "Failed to create received video file for composite %s",
             fileNameYUVCom_.c_str());
      return;
    }
    else
    {
      AG_LOG(INFO, "writing composite YUV in Frame observer channel id %s, %s", channelId, fileNameYUVCom_.c_str());
    }
  }

  int width = videoFrame->yStride;
  int height = videoFrame->height;
  int YSize = width * height;
  int UVSize = YSize >> 2;
  int totalFrameSize = (YSize + (UVSize << 1));
  int totalSizeCom = totalFrameSize * NUM_OF_HOST; // assuming all video streams have the same size.

  if ((ybufCom_ == nullptr) && !hostProcessedNum_)
  {
    if ((ybufCom_ = (unsigned char *)malloc(YSize * NUM_OF_HOST)) == NULL)
    {
      AG_LOG(ERROR, "yuv composit buf malloc failed: %s", std::strerror(errno));
      return;
    }
    memset(ybufCom_, 0, YSize * NUM_OF_HOST);
    if ((ubufCom_ = (unsigned char *)malloc(UVSize * NUM_OF_HOST)) == NULL)
    {
      AG_LOG(ERROR, "u composit buf malloc failed: %s", std::strerror(errno));
      return;
    }
    memset(ubufCom_, 0, UVSize * NUM_OF_HOST);
    if ((vbufCom_ = (unsigned char *)malloc(UVSize * NUM_OF_HOST)) == NULL)
    {
      AG_LOG(ERROR, "v composit buf malloc failed: %s", std::strerror(errno));
      return;
    }
    memset(vbufCom_, 0, UVSize * NUM_OF_HOST);
  }
  AG_LOG(INFO, "writing composite YUV in a buffer first %p, %d, host num %d, ruid: %d stride %d, height %d", ybufCom_, YSize * NUM_OF_HOST,
         hostProcessedNum_, atoi(remoteUid), width, height);
  switch ((int)atoi(remoteUid))
  {
  case 1:
    strideOffset = horizontalIndex[0];
    heightOffset = verticalIndex[0];
    hostProcessedNum_ |= 1;
    break;
  case 2:
    strideOffset = horizontalIndex[1];
    heightOffset = verticalIndex[0];
    hostProcessedNum_ |= 1 << 1;
    break;
  case 3:
    strideOffset = horizontalIndex[0];
    heightOffset = verticalIndex[1];
    hostProcessedNum_ |= 1 << 2;
    break;
  case 4:
    strideOffset = horizontalIndex[1];
    heightOffset = verticalIndex[1];
    hostProcessedNum_ |= 1 << 3;
    break;
  default:
    strideOffset = horizontalIndex[0];
    heightOffset = verticalIndex[0];
    hostProcessedNum_ = 1;
    return;
  }

  unsigned char *ybase, *ubase, *vbase;
  ybase = videoFrame->yBuffer;
  ubase = videoFrame->uBuffer;
  vbase = videoFrame->vBuffer;
  if (hostProcessedNum_ <= (1 << NUM_OF_HOST) - 1)
  {
    unsigned char *StartP;
    int offset = strideOffset * width + heightOffset * YSize * 2; // offset for Y
    unsigned char *srcP = ybase;
    // Composite Y planar
    StartP = ybufCom_ + offset;
    AG_LOG(INFO, "writing composite Y in a buffer first %p, for host %d, start ptr %d, offset %d ", ybufCom_, hostProcessedNum_, (int)(StartP - ybufCom_), offset);

    for (int i = 0; i < height; i++)
    {

      memcpy(StartP, srcP, width);
      srcP += width;
      StartP += (width << 1);
    }
    AG_LOG(INFO, "writing composite Y in a buffer first %p, for host %d, start ptr %p", ybufCom_, hostProcessedNum_, StartP);

    // Composite U planar
    width = videoFrame->uStride;
    height = height >> 1;
    offset = strideOffset * width + heightOffset * UVSize * 2;
    StartP = ubufCom_ + offset;
    srcP = ubase;
    AG_LOG(INFO, "writing composite U in a buffer first %p, for host %d, start ptr %d, offset %d, ustride %d ", ubufCom_, hostProcessedNum_, (int)(StartP - ubufCom_), offset, width);

    for (int i = 0; i < height; i++)
    {
      memcpy(StartP, srcP, width);
      StartP += (width << 1);
      srcP += width;
    }

    // Composite V Planar
    width = videoFrame->vStride;
    offset = strideOffset * width + heightOffset * UVSize * 2;
    StartP = vbufCom_ + offset;
    srcP = vbase;
    AG_LOG(INFO, "writing composite V in a buffer first %p, for host %d, start ptr %d, offset %d, vstride: %d", vbufCom_, hostProcessedNum_, (int)(StartP - vbufCom_), offset, width);

    for (int i = 0; i < height; i++)
    {
      memcpy(StartP, srcP, width);
      srcP += width;
      StartP += (width << 1);
    }
  }

  AG_LOG(INFO, "how many hosts yet? writing composite Y in a buffer first %p, for host %d, %s ", ybufCom_, hostProcessedNum_, fileNameYUVCom_.c_str());

  if (hostProcessedNum_ >= (1 << NUM_OF_HOST) - 1)
  {
    // write composite buffer to the file
    *video_frame_saved_flag_ = 1;
    if (fwrite(ybufCom_, 1, YSize * NUM_OF_HOST, yuvFileCom_) != (YSize * NUM_OF_HOST))
    {
      AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
    }
    else
    {
      AG_LOG(ERROR, "writing Y raw video data: %d", YSize * NUM_OF_HOST);
    }

    if (fwrite(ubufCom_, 1, UVSize * NUM_OF_HOST, yuvFileCom_) != (UVSize * NUM_OF_HOST))
    {
      AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
    }
    else
    {
      AG_LOG(ERROR, "writing U raw video data: %d", UVSize * NUM_OF_HOST);
    }
    if (fwrite(vbufCom_, 1, UVSize * NUM_OF_HOST, yuvFileCom_) != (UVSize * NUM_OF_HOST))
    {
      AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
    }
    else
    {
      AG_LOG(ERROR, "writing V raw video data: %d", UVSize * NUM_OF_HOST);
    }
    if (ybufCom_)
    {
      free(ybufCom_);
      ybufCom_ = nullptr;
    }
    if (ubufCom_)
    {
      free(ubufCom_);
      ubufCom_ = nullptr;
    }
    if (vbufCom_)
    {
      free(vbufCom_);
      vbufCom_ = nullptr;
    }
    hostProcessedNum_ = 0;
    fileSizeCom_ += totalSizeCom;
    fileCountCom++;
    fclose(yuvFileCom_);
    yuvFileCom_ = nullptr;
    fileSizeCom_ = 0;

    {
      AG_LOG(INFO, "convert jpeg from yuv %s to %s ", fileNameYUVCom_.c_str(), fileNameJpgCom_.c_str());

      // convert the composit file to jpg format
      int hshift = 1;
      int vshift = (NUM_OF_HOST >> 2) ? 1 : 0;
      command = "ffmpeg -f rawvideo -vcodec rawvideo -s " + to_string(videoFrame->yStride << hshift) + "x" +
                to_string(videoFrame->height << vshift) + " -r 1 -pix_fmt yuv420p -i " + fileNameYUVCom_ + " -preset ultrafast -qp 0 " + fileNameJpgCom_;
      system(command.c_str());
      AG_LOG(INFO, "ffmpeg convert YUV to jpeg, command: %s", command.c_str());
      
    }
  }

#else  // COMPOSIT
  *video_frame_saved_flag_ = 1;
#endif // COMPOSIT
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
  // memset(yuvbuf, 0, width * 3);

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
  // AG_LOG(INFO, "libjpeg convert YUV to jpeg");
  *video_frame_saved_flag_ = 1;
#endif
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
  agora::rtc::RtcConnectionConfiguration ccfg;
  ccfg.clientRoleType = agora::rtc::CLIENT_ROLE_AUDIENCE;
  ccfg.autoSubscribeAudio = false;
  ccfg.autoSubscribeVideo = false;
  ccfg.enableAudioRecordingOrPlayout =
      false; // Subscribe audio but without playback

  //  start the connect -> save frame -> disconnect loop
  int pacing_interval = 20 * 1000000 / options.multiChannels;
  for (int i = 0; i < options.multiChannels; ++i)
  {
    // AG_LOG(INFO, "!!!!!!!!!! index: %d", i);
    th_array[i] = std::thread(connectWorker, service, i, std::ref(exitFlag));
    usleep(pacing_interval); // add a pacing
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
