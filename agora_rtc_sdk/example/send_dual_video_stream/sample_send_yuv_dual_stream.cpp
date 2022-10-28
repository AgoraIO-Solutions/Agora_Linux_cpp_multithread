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

#include "IAgoraService.h"
#include "NGIAgoraRtcConnection.h"
#include "common/helper.h"
#include "common/log.h"
#include "common/opt_parser.h"
#include "common/sample_common.h"
#include "common/sample_connection_observer.h"

#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraLocalUser.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "NGIAgoraMediaNode.h"
#include "NGIAgoraVideoTrack.h"

#define DEFAULT_CONNECT_TIMEOUT_MS (3000)
#define DEFAULT_TARGET_BITRATE (1 * 1000 * 1000)
#define DEFAULT_VIDEO_WIDTH (352)
#define DEFAULT_VIDEO_HEIGHT (288)
#define DEFAULT_FRAME_RATE (15)
#define DEFAULT_VIDEO_FILE "test_data/send_video_cif.yuv"

struct SampleOptions {
  std::string appId;
  std::string channelId;
  std::string userId;
  std::string videoFile = DEFAULT_VIDEO_FILE;

  struct {
    int targetBitrate = DEFAULT_TARGET_BITRATE;
    int width = DEFAULT_VIDEO_WIDTH;
    int height = DEFAULT_VIDEO_HEIGHT;
    int frameRate = DEFAULT_FRAME_RATE;
  } video;
};

static void sendOneYuvFrame(const SampleOptions& options,
                            agora::agora_refptr<agora::rtc::IVideoFrameSender> videoFrameSender) {
  static FILE* file = nullptr;
  const char* fileName = options.videoFile.c_str();

  // Calculate byte size for YUV420 image
  int sendBytes = options.video.width * options.video.height * 3 / 2;

  if (!file) {
    if (!(file = fopen(fileName, "rb"))) {
      AG_LOG(ERROR, "Failed to open video file %s", fileName);
      return;
    }
    AG_LOG(INFO, "Open video file %s successfully", fileName);
  }

  uint8_t frameBuf[sendBytes];

  if (fread(frameBuf, 1, sizeof(frameBuf), file) != sizeof(frameBuf)) {
    if (feof(file)) {
      fclose(file);
      file = nullptr;
      AG_LOG(INFO, "End of video file");
    } else {
      AG_LOG(ERROR, "Error reading video data: %s", std::strerror(errno));
    }
    return;
  }

  agora::media::base::ExternalVideoFrame videoFrame;
  videoFrame.type = agora::media::base::ExternalVideoFrame::VIDEO_BUFFER_RAW_DATA;
  videoFrame.format = agora::media::base::VIDEO_PIXEL_I420;
  videoFrame.buffer = frameBuf;
  videoFrame.stride = options.video.width;
  videoFrame.height = options.video.height;
  videoFrame.cropLeft = 0;
  videoFrame.cropTop = 0;
  videoFrame.cropRight = 0;
  videoFrame.cropBottom = 0;
  videoFrame.rotation = 0;
  videoFrame.timestamp = 0;

  if (videoFrameSender->sendVideoFrame(videoFrame) < 0) {
    AG_LOG(ERROR, "Failed to send video frame!");
  }
}

static void SampleSendVideoTask(const SampleOptions& options,
                                agora::agora_refptr<agora::rtc::IVideoFrameSender> videoFrameSender,
                                bool& exitFlag) {
  // Calculate send interval based on frame rate. H264 frames are sent at this interval
  PacerInfo pacer = {0, 1000 / options.video.frameRate,0, std::chrono::steady_clock::now()};

  while (!exitFlag) {
    sendOneYuvFrame(options, videoFrameSender);
    waitBeforeNextSend(pacer);  // sleep for a while before sending next frame
  }
}

static bool exitFlag = false;
static void SignalHandler(int sigNo) { exitFlag = true; }

int main(int argc, char* argv[]) {
  SampleOptions options;
  opt_parser optParser;

  optParser.add_long_opt("token", &options.appId, "The token for authentication / must");
  optParser.add_long_opt("channelId", &options.channelId, "Channel Id / must");
  optParser.add_long_opt("userId", &options.userId, "User Id / default is 0");
  optParser.add_long_opt("videoFile", &options.videoFile,
                         "The video file in YUV420 format to be sent");
  optParser.add_long_opt("fps", &options.video.frameRate,
                         "Target frame rate for sending the video stream");
  optParser.add_long_opt("width", &options.video.width, "Image width for the YUV file to be sent");
  optParser.add_long_opt("height", &options.video.height,
                         "Image height for the YUV file to be sent");
  optParser.add_long_opt("bitrate", &options.video.targetBitrate,
                         "Target bitrate (bps) for encoding the YUV stream");

  if ((argc <= 1) || !optParser.parse_opts(argc, argv)) {
    std::ostringstream strStream;
    optParser.print_usage(argv[0], strStream);
    std::cout << strStream.str() << std::endl;
    return -1;
  }

  if (options.appId.empty()) {
    AG_LOG(ERROR, "Must provide appId!");
    return -1;
  }

  if (options.channelId.empty()) {
    AG_LOG(ERROR, "Must provide channelId!");
    return -1;
  }

  std::signal(SIGQUIT, SignalHandler);
  std::signal(SIGABRT, SignalHandler);
  std::signal(SIGINT, SignalHandler);

  // Create Agora service
  auto service = createAndInitAgoraService(false, true, true);
  if (!service) {
    AG_LOG(ERROR, "Failed to creating Agora service!");
  }

  // Create Agora connection
  agora::rtc::RtcConnectionConfiguration ccfg;
  ccfg.autoSubscribeAudio = false;
  ccfg.autoSubscribeVideo = false;
  ccfg.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;

  agora::agora_refptr<agora::rtc::IRtcConnection> connection = service->createRtcConnection(ccfg);
  if (!connection) {
    AG_LOG(ERROR, "Failed to creating Agora connection!");
    return -1;
  }

  // Register connection observer to monitor connection event
  auto connObserver = std::make_shared<SampleConnectionObserver>();
  connection->registerObserver(connObserver.get());

  // Connect to Agora channel
  if (connection->connect(options.appId.c_str(), options.channelId.c_str(),
                          options.userId.c_str())) {
    AG_LOG(ERROR, "Failed to connect to Agora channel!");
    return -1;
  }

  // Create media node factory
  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory = service->createMediaNodeFactory();
  if (!factory) {
    AG_LOG(ERROR, "Failed to create media node factory!");
  }

  // Create video frame sender
  agora::agora_refptr<agora::rtc::IVideoFrameSender> videoFrameSender =
      factory->createVideoFrameSender();
  if (!videoFrameSender) {
    AG_LOG(ERROR, "Failed to create video frame sender!");
    return -1;
  }

  // Create video track
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> customVideoTrack =
      service->createCustomVideoTrack(videoFrameSender);
  if (!customVideoTrack) {
    AG_LOG(ERROR, "Failed to create video track!");
    return -1;
  }

  // Configure video encoder
  agora::rtc::VideoEncoderConfiguration encoderConfig(
      options.video.width, options.video.height, options.video.frameRate,
      options.video.targetBitrate, agora::rtc::ORIENTATION_MODE_ADAPTIVE);
  customVideoTrack->setVideoEncoderConfiguration(encoderConfig);
  // Set the dual_model ahd low_stream
  agora::rtc::VideoDimensions low_dimensions(options.video.width / 2, options.video.height / 2);
  agora::rtc::SimulcastStreamConfig Low_streamConfig;
  Low_streamConfig.dimensions = low_dimensions;
  Low_streamConfig.bitrate = options.video.targetBitrate / 2;
  customVideoTrack->enableSimulcastStream(true, Low_streamConfig);
  // Publish video track
  customVideoTrack->setEnabled(true);
  connection->getLocalUser()->publishVideo(customVideoTrack);

  // Wait until connected before sending media stream
  connObserver->waitUntilConnected(DEFAULT_CONNECT_TIMEOUT_MS);

  // Start sending media data
  AG_LOG(INFO, "Start sending video data ...");

  std::thread sendVideoThread(SampleSendVideoTask, options, videoFrameSender, std::ref(exitFlag));

  sendVideoThread.join();

  // Unpublish video track
  connection->getLocalUser()->unpublishVideo(customVideoTrack);

  // Unregister connection observer
  connection->unregisterObserver(connObserver.get());

  // Disconnect from Agora channel
  if (connection->disconnect()) {
    AG_LOG(ERROR, "Failed to disconnect from Agora channel!");
    return -1;
  }
  AG_LOG(INFO, "Disconnected from Agora channel successfully");

  // Destroy Agora connection and related resources
  connObserver.reset();
  videoFrameSender = nullptr;
  customVideoTrack = nullptr;
  factory = nullptr;
  connection = nullptr;

  // Destroy Agora Service
  service->release();
  service = nullptr;

  return 0;
}
