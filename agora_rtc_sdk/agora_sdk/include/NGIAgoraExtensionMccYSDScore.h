
 //Agora SDK

 //Copyright (c) 2021 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class IMccResponseCallback {
public:
  virtual ~IMccResponseCallback() {}
  virtual void onResponse(void* data, int length) = 0;
};

class IMccHttp {
public:
  IMccHttp() {}
  virtual ~IMccHttp() {}
  virtual int get(const char* url, const char* header, IMccResponseCallback* callback) = 0;
  virtual int post(const char* url, const char* header, const char* body, IMccResponseCallback* callback) = 0;
};

class IMccYSDInitCallback {
public:
  virtual ~IMccYSDInitCallback() {}
  virtual void onInit(int errorCode) = 0;
};

class IMccYSDReportCallback {
public:
  virtual ~IMccYSDReportCallback() {}
  virtual void onReport(int errorCode) = 0;
};

class IMccYSDMediaResourceCallback {
public:
  virtual ~IMccYSDMediaResourceCallback() {}
  virtual void onMediaResource(int errorCode, const char* response) = 0;
};

enum MccYSDScoreState {
  MccYSDScoreStateNone = 0,
  MccYSDScoreStateStart,
  MccYSDScoreStateProcessing,
  MccYSDScoreStatePause,
  MccYSDScoreStateStop
};

struct MccYSDPitchData {
  int32_t startTime;
  int32_t duration;
  int32_t pitch;

  MccYSDPitchData() : startTime(0), duration(0), pitch(0) {}
  MccYSDPitchData(int32_t startTime, int32_t duration, int32_t pitch)
      : startTime(startTime), duration(duration), pitch(pitch) {}
};

struct MccYSDScoreInfo {
  int32_t index;
  int32_t score;

  MccYSDScoreInfo() : index(0), score(0) {}
  MccYSDScoreInfo(int32_t index, int32_t score) : index(index), score(score) {}
};

enum MccYSDScoreVersion {
    Version_OLD = 0,
    Version_NEW
};

struct YSDScoreConfig {
  const char* pitchData;
  const char* lyric;
  const char* urlToken;
  int offset;
  int segmentOffset;
  IMccYSDInitCallback* callback;
  void* httpImpl;
  MccYSDScoreVersion version;
};

class MccYSDExtension {
 public:
  enum ScoreHardLevel
  {
      SCORE_HARD_LEVEL1,
      SCORE_HARD_LEVEL2,
      SCORE_HARD_LEVEL3,
      SCORE_HARD_LEVEL4,
      SCORE_HARD_LEVEL5,
  };

  enum ChargeMode
  {
      CHARGE_MODE_MONTHLY = 1,
      CHARGE_MODE_ONCE = 2,
  };

  enum MediaType
  {
      MEDIA_TYPE_SONG = 1,
      MEDIA_TYPE_ACCOMPANY = 2,
      MEDIA_TYPE_ACCOMPANY_FRAGMENT = 3,
  };

  MccYSDExtension() {}
  virtual ~MccYSDExtension() {}

  virtual int initialize(const char* pitchData, const char* lyric, int offset, const char* urlToken, IMccYSDInitCallback* callback, MccYSDScoreVersion version = Version_OLD) = 0;
  virtual void release() = 0;
  virtual bool isInitialized() = 0;
  virtual void setHttpImpl(IMccHttp* httpClient) = 0;
  virtual int setAudioParams(int sampleRate, int channels) = 0;
  virtual bool getPitchData(MccYSDPitchData** pitchData, int* count) = 0;
  virtual void enableScore(bool enable) = 0;
  virtual void setPitch(short pitch) = 0;
  virtual const char* getLyric() = 0;
  virtual const char* decryptLyric(const char* lyric) = 0;
  virtual int immediatelyDisplay() = 0;
  virtual int updateScore(int pts, MccYSDScoreInfo& scoreInfo) = 0;
  virtual int updateAverageScore(int pts) = 0;
  virtual int updateTotalScore(int pts) = 0;
  virtual int calculateTotalScore() = 0;
  virtual void process(const void* buffer, int size, int pts) = 0;
  virtual void reset() = 0;
  virtual void setScoreHardLevel(ScoreHardLevel level) = 0;
  virtual int report(IMccYSDReportCallback* callback) = 0;
  virtual const char* version() = 0;
  virtual int getMediaResource(const char* deviceId, const char* pid, const char* userId, const char* token, 
                                const char* songId, bool ssl, ChargeMode chargeMode, MediaType mediaType, 
                                IMccYSDMediaResourceCallback* response) = 0;
  virtual int getMusicSegmentOffset() = 0;
  virtual void setCurrentSongCode(int64_t songCode) {
    songCode_ = songCode;
  }

 protected:
  IMccHttp* m_httpClient;
  int64_t songCode_;
};


}  // namespace rtc
}  // namespace agora
