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
#include "common/opt_parser.h"
#include "common/sample_common.h"
#include "common/sample_local_user_observer.h"
#include "common/log.h"

#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraLocalUser.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "NGIAgoraMediaNode.h"
#include "NGIAgoraVideoTrack.h"

#define DEFAULT_AUDIO_FILE "received_audio.aac"
#define DEFAULT_FILE_LIMIT (100 * 1024 * 1024)

struct SampleOptions
{
    std::string appId;
    std::string channelId;
    std::string userId;
    std::string audioFile = DEFAULT_AUDIO_FILE;
};

class EncodedAudioReceiver : public agora::rtc::IAudioEncodedFrameReceiver
{
public:
    EncodedAudioReceiver(const std::string &outputFilePath) : outputFilePath_(outputFilePath) {}

    bool onEncodedAudioFrameReceived(
        const uint8_t *packet, size_t length, const agora::media::base::AudioEncodedFrameInfo &info) override;

private:
    std::string outputFilePath_;
    FILE *file_ = nullptr;
    int fileCount = 0;
    int fileSize_;
};

bool EncodedAudioReceiver::onEncodedAudioFrameReceived(
    const uint8_t *packet, size_t length, const agora::media::base::AudioEncodedFrameInfo &info)
{
    if (!file_)
    {
        std::string fileName = (++fileCount > 1)
                                   ? (outputFilePath_ + to_string(fileCount))
                                   : outputFilePath_;
        if (!(file_ = fopen(fileName.c_str(), "w")))
        {
            AG_LOG(ERROR, "Failed to create received audio file %s",
                   fileName.c_str());
            return false;
        }
        AG_LOG(INFO, "Created file %s to save received PCM samples",
               fileName.c_str());
    }

    printf("length is %d \n",length);

    if (fwrite(packet, 1, length, file_) != length)
    {
        AG_LOG(ERROR, "Error writing encoded audio data: %s", std::strerror(errno));
        return false;
    }
    fileSize_ += length;
    if (fileSize_ >= DEFAULT_FILE_LIMIT)
    {
        fclose(file_);
        file_ = nullptr;
        fileSize_ = 0;
    }
    return true;
}

static bool exitFlag = false;
static void SignalHandler(int sigNo) { exitFlag = true; }

int main(int argc, char *argv[])
{
    SampleOptions options;
    opt_parser optParser;

    optParser.add_long_opt("token", &options.appId,
                           "The token for authentication");
    optParser.add_long_opt("channelId", &options.channelId, "Channel Id");
    optParser.add_long_opt("userId", &options.userId, "User Id / default is 0");
    optParser.add_long_opt("audioFile", &options.audioFile, "audioFile ");

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

    agora::rtc::RtcConnectionConfiguration ccfg;
    ccfg.clientRoleType = agora::rtc::CLIENT_ROLE_AUDIENCE;
    ccfg.autoSubscribeAudio = false;
    ccfg.autoSubscribeVideo = false;
    ccfg.enableAudioRecordingOrPlayout =
        false; // Subscribe audio but without playback
    ccfg.audioRecvEncodedFrame = true;

    agora::agora_refptr<agora::rtc::IRtcConnection> connection =
        service->createRtcConnection(ccfg);
    if (!connection)
    {
        AG_LOG(ERROR, "Failed to creating Agora connection!");
        return -1;
    }

    connection->getLocalUser()->subscribeAllAudio();

      // Connect to Agora channel
  if (connection->connect(options.appId.c_str(), options.channelId.c_str(),
                          options.userId.c_str())) {
    AG_LOG(ERROR, "Failed to connect to Agora channel!");
    return -1;
  }

    auto localUserObserver =
        std::make_shared<SampleLocalUserObserver>(connection->getLocalUser());
    
    auto audioReceiver = std::make_shared<EncodedAudioReceiver>(options.audioFile);

    localUserObserver->setEncodedAudioFrameObserver(audioReceiver.get());

    // Start receiving incoming media data
    AG_LOG(INFO, "Start receiving audio & video data ...");

    // Periodically check exit flag
    while (!exitFlag)
    {
        usleep(10000);
    }
     if (connection->disconnect()) {
    AG_LOG(ERROR, "Failed to disconnect from Agora channel!");
    return -1;
  }
  AG_LOG(INFO, "Disconnected from Agora channel successfully");

  // Destroy Agora connection and related resources
  localUserObserver.reset();
  audioReceiver.reset();

  connection = nullptr;

  // Destroy Agora Service
  service->release();
  service = nullptr;

    return 0;
}