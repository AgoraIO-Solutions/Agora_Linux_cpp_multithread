//
//  Agora Media SDK
//
//  Created by Xiaosen Wang in 2020.
//  Copyright (c) 2020 Agora IO. All rights reserved.
//
#pragma once

#include "AgoraRefPtr.h"
#include "IAgoraMediaRecorder.h"
#include "NGIAgoraAudioTrack.h"
#include "NGIAgoraRtcConnection.h"
#include "NGIAgoraMediaNode.h"
#include <api/aosl_ref.h>

namespace agora {
namespace rtc {

class IMediaRecorder2 : public IMediaRecorder {
 public:
  virtual int setRecorderConfig(const media::MediaRecorderConfiguration& config,
                                aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int setEnabledRecordeRtcEncodedSource(bool enable,aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int setVideoTrack(agora_refptr<IVideoTrack> track,
                            aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int removeVideoTrack(aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  virtual int setAudioTrack(agora_refptr<IAudioTrack> track,
                            aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int removeAudioTrack(aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  virtual int startRecording(aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int stopRecording(aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int setRtcConnection(agora_refptr<rtc::IRtcConnection> connection,aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  virtual ~IMediaRecorder2(){};
};

}  // namespace rtc
}  // namespace agora