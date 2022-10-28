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

#define DEFAULT_VIDEO_FILE "received_video.yuv"
#define DEFAULT_FILE_LIMIT (100 * 1024 * 1024)
#define STREAM_TYPE_HIGH "high"
#define STREAM_TYPE_LOW "low"

struct SampleOptions {
	std::string appId;
	std::string channelId;
	std::string userId;
	std::string remoteUserId;
	std::string streamType = STREAM_TYPE_HIGH;
	std::string videoFile = DEFAULT_VIDEO_FILE;
};

class YuvFrameObserver : public agora::rtc::IVideoSinkBase {
public:
	YuvFrameObserver(const std::string &outputFilePath)
			: outputFilePath_(outputFilePath), yuvFile_(nullptr), fileCount(0), fileSize_(0)
	{
	}

	int onFrame(const agora::media::base::VideoFrame &videoFrame) override;

	virtual ~YuvFrameObserver() = default;

private:
	std::string outputFilePath_;
	FILE *yuvFile_;
	int fileCount;
	int fileSize_;
};

int YuvFrameObserver::onFrame(const agora::media::base::VideoFrame &videoFrame)
{
	// Create new file to save received YUV frames
	// printf("it is %d %d\n",videoFrame.height,videoFrame.width);
	if (!yuvFile_) {
		std::string fileName =
				(++fileCount > 1) ? (outputFilePath_ + to_string(fileCount)) : outputFilePath_;
		if (!(yuvFile_ = fopen(fileName.c_str(), "w+"))) {
			AG_LOG(ERROR, "Failed to create received video file %s", fileName.c_str());
			return 0;
		}
		AG_LOG(INFO, "Created file %s to save received YUV frames", fileName.c_str());
	}

	// Write Y planar
	size_t writeBytes = videoFrame.yStride * videoFrame.height;
	if (fwrite(videoFrame.yBuffer, 1, writeBytes, yuvFile_) != writeBytes) {
		AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
		return 0;
	}
	fileSize_ += writeBytes;

	// Write U planar
	writeBytes = videoFrame.uStride * videoFrame.height / 2;
	if (fwrite(videoFrame.uBuffer, 1, writeBytes, yuvFile_) != writeBytes) {
		AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
		return 0;
	}
	fileSize_ += writeBytes;

	// Write V planar
	writeBytes = videoFrame.vStride * videoFrame.height / 2;
	if (fwrite(videoFrame.vBuffer, 1, writeBytes, yuvFile_) != writeBytes) {
		AG_LOG(ERROR, "Error writing decoded video data: %s", std::strerror(errno));
		return 0;
	}
	fileSize_ += writeBytes;

	// Close the file if size limit is reached
	if (fileSize_ >= DEFAULT_FILE_LIMIT) {
		fclose(yuvFile_);
		yuvFile_ = nullptr;
		fileSize_ = 0;
	}
	return 0;
};

static bool exitFlag = false;
static void SignalHandler(int sigNo)
{
	exitFlag = true;
}

int main(int argc, char *argv[])
{
	SampleOptions options;
	opt_parser optParser;

	optParser.add_long_opt("token", &options.appId, "The token for authentication");
	optParser.add_long_opt("channelId", &options.channelId, "Channel Id");
	optParser.add_long_opt("userId", &options.userId, "User Id / default is 0");
	optParser.add_long_opt("remoteUserId", &options.remoteUserId,
						   "The remote user to receive stream from");
	optParser.add_long_opt("videoFile", &options.videoFile, "Output video file");
	optParser.add_long_opt("streamtype", &options.streamType, "the stream type");

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
	ccfg.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
	ccfg.autoSubscribeAudio = false;
	ccfg.autoSubscribeVideo = false;
	ccfg.enableAudioRecordingOrPlayout = false; // Subscribe audio but without playback

	agora::agora_refptr<agora::rtc::IRtcConnection> connection = service->createRtcConnection(ccfg);
	if (!connection) {
		AG_LOG(ERROR, "Failed to creating Agora connection!");
		return -1;
	}

	// Subcribe streams from all remote users or specific remote user
	agora::rtc::VideoSubscriptionOptions subscriptionOptions;
	if (options.streamType == STREAM_TYPE_HIGH) {
		subscriptionOptions.type = agora::rtc::VIDEO_STREAM_HIGH;
	} else if (options.streamType == STREAM_TYPE_LOW) {
		subscriptionOptions.type = agora::rtc::VIDEO_STREAM_LOW;
	} else {
		AG_LOG(ERROR, "It is a error stream type");
		return -1;
	}
	if (options.remoteUserId.empty()) {
		AG_LOG(INFO, "Subscribe streams from all remote users");
		connection->getLocalUser()->subscribeAllAudio();
		connection->getLocalUser()->subscribeAllVideo(subscriptionOptions);

	} else {
		connection->getLocalUser()->subscribeAudio(options.remoteUserId.c_str());
		connection->getLocalUser()->subscribeVideo(options.remoteUserId.c_str(),
												   subscriptionOptions);
	}
	// Register connection observer to monitor connection event
	auto connObserver = std::make_shared<SampleConnectionObserver>();
	connection->registerObserver(connObserver.get());

	// Create local user observer
	auto localUserObserver = std::make_shared<SampleLocalUserObserver>(connection->getLocalUser());

	agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory = service->createMediaNodeFactory();
	if (!factory) {
		AG_LOG(ERROR, "Failed to create media node factory!");
	}

	agora::agora_refptr<agora::rtc::IVideoMixerSource> videoMixer = factory->createVideoMixer();
	if (!videoMixer) {
		AG_LOG(ERROR, "Failed to create video frame sender!");
		return -1;
	}
	videoMixer->setBackground(1920, 1080, 15);

	agora::agora_refptr<agora::rtc::ILocalVideoTrack> mixVideoTrack =
			service->createMixedVideoTrack(videoMixer);
	if (!mixVideoTrack) {
		AG_LOG(ERROR, "Failed to create video track!");
		return -1;
	}
	agora::rtc::VideoEncoderConfiguration encoderConfig;
	encoderConfig.codecType = agora::rtc::VIDEO_CODEC_H264;
	encoderConfig.dimensions.width = 1920;
	encoderConfig.dimensions.height = 1080;
	encoderConfig.frameRate = 15;
	//encoderConfig.bitrate = options.video.targetBitrate + 1000;

	mixVideoTrack->setVideoEncoderConfiguration(encoderConfig);

	// // Register video frame observer to receive video stream
	agora::agora_refptr<agora::rtc::IVideoSinkBase> yuvFrameObserver =
			new agora::RefCountedObject<YuvFrameObserver>(options.videoFile);

	mixVideoTrack->addRenderer(yuvFrameObserver.get());
	//printf("it is %d \n",b);
	mixVideoTrack->setEnabled(true);
	localUserObserver->setEnableVideoMix(true);
	localUserObserver->setVideoMixer(videoMixer);
  // add a Image to video mixer
	// {
	// 	agora::rtc::MixerLayoutConfig mixConfig;
	// 	mixConfig.height = 379;
	// 	mixConfig.width = 379;
	// 	mixConfig.image_path = "1.jpg";
	// 	int r = videoMixer->addImageSource("1", mixConfig, agora::rtc::kJpeg);
	// 	videoMixer->setStreamLayout("1", mixConfig);

	// 	videoMixer->refresh();
	// }
	connection->getLocalUser()->publishVideo(mixVideoTrack);
	// Connect to Agora channel
	if (connection->connect(options.appId.c_str(), options.channelId.c_str(),
							options.userId.c_str())) {
		AG_LOG(ERROR, "Failed to connect to Agora channel!");
		return -1;
	}

	// Start receiving incoming media data
	AG_LOG(INFO, "Start receiving audio & video data ...");

	// Periodically check exit flag
	while (!exitFlag) {
		usleep(10000);
	}

	// Unregister connection observer
	connection->unregisterObserver(connObserver.get());

	// Disconnect from Agora channel
	if (connection->disconnect()) {
		AG_LOG(ERROR, "Failed to disconnect from Agora channel!");
		return -1;
	}
	AG_LOG(INFO, "Disconnected from Agora channel successfully");

	// Destroy Agora connection and related resources
	localUserObserver.reset();
	yuvFrameObserver.reset();
	connection = nullptr;

	// Destroy Agora Service
	service->release();
	service = nullptr;

	return 0;
}
