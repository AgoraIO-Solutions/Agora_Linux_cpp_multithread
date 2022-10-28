//  Agora RTC/MEDIA SDK
//
//  Created by Jay Zhang in 2020-04.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
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
#include "common/sample_local_user_observer.h"

#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraLocalUser.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "NGIAgoraMediaNode.h"
#include "NGIAgoraVideoTrack.h"

#define DEFAULT_CONNECT_TIMEOUT_MS (3000)
#define DEFAULT_SAMPLE_RATE (16000)
#define DEFAULT_NUM_OF_CHANNELS (1)
#define DEFAULT_FRAME_RATE (30)
#define DEFAULT_VIDEO_FILE "test_data/test.vp8.ivf"


struct IVF_PAYLOAD {
  uint32_t size;
  char timestamp[8];
 // uint32_t frame_type;
  //uint64_t timestamp;
};

struct IVF_HEADER {
  uint32_t signature;
  uint16_t version;
  uint16_t head_len;
  uint32_t codec;
  uint16_t width;
  uint16_t height;
  uint32_t time_scale;
  uint32_t frame_rate;
  uint32_t frames;
  uint32_t unused;
};

char* frame;
IVF_HEADER* ivf_header_;
int fd;
struct stat sb;
void* mapped;
int offsetsize = 0;

void reset() {
  frame = (char*)mapped;
  ivf_header_ = (IVF_HEADER*)frame;
  frame += sizeof(IVF_HEADER);
  offsetsize = sizeof(IVF_HEADER);
  AG_LOG(INFO, "Reset the videoFile !");
}

bool IsKeyFrame(const uint8_t * buf ){
int tmp = (buf[2] << 16) | (buf[1]<< 8) |buf[0];
int key_frame = tmp & 0x1;
return !key_frame;
}

struct SampleOptions {
  std::string appId;
  std::string channelId;
  std::string userId;
  std::string videoFile = DEFAULT_VIDEO_FILE;
  struct {
    int frameRate = DEFAULT_FRAME_RATE;
    bool showBandwidthEstimation = false;
  } video;
};

static void sendOneFrame(agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> videoFrameSender) {
  IVF_PAYLOAD* payload = (IVF_PAYLOAD*)frame;
  const uint8_t* buf = (uint8_t*)frame;
  agora::rtc::VIDEO_FRAME_TYPE frame_type;
  if (IsKeyFrame(buf+ sizeof(IVF_PAYLOAD))) {
    frame_type = agora::rtc::VIDEO_FRAME_TYPE_KEY_FRAME;
  } else {
    frame_type = agora::rtc::VIDEO_FRAME_TYPE_DELTA_FRAME;
  } 

  agora::rtc::EncodedVideoFrameInfo videoEncodedFrameInfo;
  videoEncodedFrameInfo.rotation = agora::rtc::VIDEO_ORIENTATION_0;
  videoEncodedFrameInfo.codecType = agora::rtc::VIDEO_CODEC_VP8;
  videoEncodedFrameInfo.frameType = frame_type;
  videoEncodedFrameInfo.width = ivf_header_->width;
  videoEncodedFrameInfo.height = ivf_header_->height;
  
  videoFrameSender->sendEncodedVideoImage(
      buf + sizeof(IVF_PAYLOAD), payload->size, videoEncodedFrameInfo);

  frame += (payload->size + sizeof(IVF_PAYLOAD));
  offsetsize += payload->size + sizeof(IVF_PAYLOAD);
  if (offsetsize >= sb.st_size) {
    reset();
  }
}

static void SampleSendVideoTask(
    const SampleOptions& options,
    agora::agora_refptr<agora::rtc::IVideoEncodedImageSender>
        videoFrameSender,
    bool& exitFlag) {
  // interval
  PacerInfo pacer = {0, 1000 / options.video.frameRate,0,
                     std::chrono::steady_clock::now()};

  while (!exitFlag) {
      sendOneFrame(videoFrameSender);
      waitBeforeNextSend(pacer);  // sleep for a while before sending next frame
    }
  };


static bool exitFlag = false;
static void SignalHandler(int sigNo) { exitFlag = true; }

int main(int argc, char* argv[]) {
  SampleOptions options;
  opt_parser optParser;

  optParser.add_long_opt("token", &options.appId,
                         "The token for authentication / must");
  optParser.add_long_opt("channelId", &options.channelId, "Channel Id / must");
  optParser.add_long_opt("userId", &options.userId, "User Id / default is 0");
  optParser.add_long_opt("videoFile", &options.videoFile,
                         "The video file in YUV420 format to be sent");
  optParser.add_long_opt("fps", &options.video.frameRate,
                         "Target frame rate for sending the video stream");
  optParser.add_long_opt("bwe", &options.video.showBandwidthEstimation,
                         "show or hide bandwidth estimation info");

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

  //if ((fd = open("test_data/test.vp8.ivf", O_RDONLY)) < 0) {
  if ((fd = open(options.videoFile.c_str(), O_RDONLY)) < 0) {

    return -1;
  }

  // get the file property
  if ((fstat(fd, &sb)) == -1) {
    return -1;
  }

  // map the file to process address space
  if ((mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) ==
      (void*)-1) {
    close(fd);
    return -1;
  }
  reset();
  
  // Create Agora service
  auto service = createAndInitAgoraService(false, true, true);
  if (!service) {
    AG_LOG(ERROR, "Failed to creating Agora service!");
  }

  // Create Agora connection
  agora::rtc::RtcConnectionConfiguration ccfg;
  ccfg.autoSubscribeVideo = false;
  ccfg.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  agora::agora_refptr<agora::rtc::IRtcConnection> connection =
      service->createRtcConnection(ccfg);
  if (!connection) {
    AG_LOG(ERROR, "Failed to creating Agora connection!");
    return -1;
  }

  // Register connection observer to monitor connection event
  auto connObserver = std::make_shared<SampleConnectionObserver>();
  connection->registerObserver(connObserver.get());

  // Register network observer to monitor bandwidth estimation result
  if (options.video.showBandwidthEstimation) {
    connection->registerNetworkObserver(connObserver.get());
  }

  // Create local user observer to monitor intra frame request
  auto localUserObserver =
      std::make_shared<SampleLocalUserObserver>(connection->getLocalUser());

  // Connect to Agora channel
  if (connection->connect(options.appId.c_str(), options.channelId.c_str(),
                          options.userId.c_str())) {
    AG_LOG(ERROR, "Failed to connect to Agora channel!");
    return -1;
  }

  // Create media node factory
  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory =
      service->createMediaNodeFactory();
  if (!factory) {
    AG_LOG(ERROR, "Failed to create media node factory!");
  }

  // Create video frame sender
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> videoFrameSender =
      factory->createVideoEncodedImageSender();
  if (!videoFrameSender) {
    AG_LOG(ERROR, "Failed to create video frame sender!");
    return -1;
  }

  agora::rtc::SenderOptions option;
  option.ccMode = agora::rtc::TCcMode::CC_ENABLED;
  // Create video track
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> customVideoTrack =
      service->createCustomVideoTrack(videoFrameSender, option);
  if (!customVideoTrack) {
    AG_LOG(ERROR, "Failed to create video track!");
    return -1;
  }
  agora::rtc::VideoEncoderConfiguration encoder_config;
  encoder_config.codecType = agora::rtc::VIDEO_CODEC_VP8;
  customVideoTrack->setVideoEncoderConfiguration(encoder_config);

  // Publish video track
  connection->getLocalUser()->publishVideo(customVideoTrack);

  // Wait until connected before sending media stream
  connObserver->waitUntilConnected(DEFAULT_CONNECT_TIMEOUT_MS);

  // Start sending media data
  AG_LOG(INFO, "Start sending video data ...");
  std::thread sendVideoThread(SampleSendVideoTask, options,
                              videoFrameSender, std::ref(exitFlag));

  sendVideoThread.join();

  // Unpublish video track
  connection->getLocalUser()->unpublishVideo(customVideoTrack);

  // Unregister connection observer
  connection->unregisterObserver(connObserver.get());

  // Unregister network observer
  connection->unregisterNetworkObserver(connObserver.get());

  // Disconnect from Agora channel
  if (connection->disconnect()) {
    AG_LOG(ERROR, "Failed to disconnect from Agora channel!");
    return -1;
  }
  AG_LOG(INFO, "Disconnected from Agora channel successfully");

  // Destroy Agora connection and related resources
  connObserver.reset();
  localUserObserver.reset();
  videoFrameSender = nullptr;
  customVideoTrack = nullptr;
  factory = nullptr;
  connection = nullptr;

  // Destroy Agora Service
  service->release();
  service = nullptr;

  return 0;
}
