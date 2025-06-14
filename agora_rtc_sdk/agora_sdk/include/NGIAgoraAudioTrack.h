
// Copyright (c) 2018 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include <api/cpp/aosl_ares_class.h>

#ifndef OPTIONAL_OVERRIDE
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_OVERRIDE override
#else
#define OPTIONAL_OVERRIDE
#endif
#endif

// FIXME(Ender): use this class instead of AudioSendStream as local track
namespace agora {
namespace rtc {
class IAudioTrackStateObserver;
class IAudioFilter;
class IAudioSinkBase;
class IMediaPacketReceiver;
class IAudioEncodedFrameReceiver;
/**
 * Properties of audio frames expected by a sink.
 *
 * @note The source determines the audio frame to be sent to the sink based on a variety of factors,
 * such as other sinks or the capability of the source.
 *
 */
struct AudioSinkWants {
  /** The sample rate of the audio frame to be sent to the sink. */
  int samplesPerSec;

  /** The number of audio channels of the audio frame to be sent to the sink. */
  size_t channels;

  AudioSinkWants() : samplesPerSec(0),
                     channels(0) {}
  AudioSinkWants(int sampleRate, size_t chs) : samplesPerSec(sampleRate),
                                               channels(chs) {}
  AudioSinkWants(int sampleRate, size_t chs, int trackNum) : samplesPerSec(sampleRate), channels(chs) {}                                            
};

enum AudioTrackType {
  LOCAL_AUDIO_TRACK,
  REMOTE_AUDIO_TRACK,
};

/**
 * The IAudioTrack class.
 */
class IAudioTrack : public RefCountInterface {
 public:
  /**
   * The position of the audio filter in audio frame.
   */
  enum AudioFilterPosition {
    /**
     * Work on the local playback
     */
    RecordingLocalPlayback,
    /**
     * Work on the post audio processing.
     */
    PostAudioProcessing,
    /**
     * Work on the remote audio before mixing.
     */
    RemoteUserPlayback,
    /**
     * Work on the pcm source.
     */
    PcmSource,
    /**
     * Work on the sending branch of the pcm source.
     */
    PcmSourceSending,
    /**
     * Work on the local playback branch of the pcm source.
     */
    PcmSourceLocalPlayback,
    /**
     * Work on the playback after remote-audio mix.
     */
    RemoteMixedPlayback,
  };

 public:
  /**
   * Adjusts the playback volume.
   * @param volume The playback volume. The value ranges between 0 and 100 (default).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPlayoutVolume(int volume, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Gets the current playback volume.
   * @param volume A pointer to the playback volume.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlayoutVolume(int* volume) = 0;

  /**
   * Adds an audio filter.
   *
   * By adding an audio filter, you can apply various audio effects to the audio, for example, voice change.
   * @param filter A pointer to the audio filter. See \ref agora::rtc::IAudioFilter "IAudioFilter".
   * @param position The position of the audio filter. See \ref agora::rtc::IAudioTrack::AudioFilterPosition "AudioFilterPosition".
   * @param extContext The context of current filter. See \ref agora::rtc::ExtensionContext "ExtensionContext".
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool addAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position, ExtensionContext *extContext = NULL, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  /**
   * Removes the audio filter added by callling `addAudioFilter`.
   *
   * @param filter The pointer to the audio filter that you want to remove. See \ref agora::rtc::IAudioFilter "IAudioFilter".
   * @param position The position of the audio filter. See #AudioFilterPosition.
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool removeAudioFilter(agora_refptr<IAudioFilter> filter, AudioFilterPosition position, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Enable / Disable specified audio filter
   * @param id id of the filter
   * @param enable enable / disable the filter with given id
   * @param position The position of the audio filter. See #AudioFilterPosition.
   * @return
   * - 0: success
   * - <0: failure
   */
  virtual int enableAudioFilter(const char* id, bool enable, AudioFilterPosition position, aosl_ref_t ares = AOSL_REF_INVALID) {
    (void)id;
    (void)enable;
    (void)position;
    return -1;
  }

  /**
   * set the properties of the specified audio filter
   * @param id id of the filter
   * @param key key of the property
   * @param jsonValue json str value of the property
   * @param position The position of the audio filter. See #AudioFilterPosition.
   * @return
   * - 0: success
   * - <0: failure
   */
  virtual int setFilterProperty(const char* id, const char* key, const char* jsonValue, AudioFilterPosition position, aosl_ref_t ares = AOSL_REF_INVALID) {
    (void)id;
    (void)key;
    (void)jsonValue;
    (void)position;
    return -1;
  }

  /**
   * get the properties of the specified video filter
   * @param id id of the filter
   * @param key key of the property
   * @param jsonValue json str value of the property
   * @param bufSize max length of the json value buffer
   * @param position The position of the audio filter. See #AudioFilterPosition.
   * @return
   * - 0: success
   * - <0: failure
   */
  virtual int getFilterProperty(const char* id, const char* key, char* jsonValue, size_t bufSize, AudioFilterPosition position) {
    (void)id;
    (void)key;
    (void)jsonValue;
    (void)bufSize;
    (void)position;
    return -1;
  }

  /**
   * Gets the audio filter by its name.
   *
   * @param name The name of the audio filter.
   * @param position The position of the audio filter. See #AudioFilterPosition.
   * @return
   * - The pointer to the audio filter: Success.
   * - A null pointer: Failure.
   */
  virtual agora_refptr<IAudioFilter> getAudioFilter(const char *name, AudioFilterPosition position) const = 0;

  /**
   * Adds an audio sink to get PCM data from the audio track.
   *
   * @param sink The pointer to the audio sink. See \ref agora::rtc::IAudioSinkBase "IAudioSinkBase".
   * @param wants The properties an audio frame should have when it is delivered to the sink. See \ref agora::rtc::AudioSinkWants "AudioSinkWants".
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool addAudioSink(agora_refptr<IAudioSinkBase> sink, const AudioSinkWants& wants, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Removes an audio sink.
   *
   * @param sink The pointer to the audio sink to be removed. See \ref agora::rtc::IAudioSinkBase "IAudioSinkBase".
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool removeAudioSink(agora_refptr<IAudioSinkBase> sink, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  /**
   * Get the track type of the audio track
   * @return
   * - AudioTrackType
   */
  virtual AudioTrackType getType() = 0;

};

/**
 * The observer of the local audio track.
 */
class ILocalAudioTrackObserver {
 public:
  virtual ~ILocalAudioTrackObserver() {}

  /**
   * Occurs when the state of a local audio track changes.
   *
   * @param state The state of the local audio track.
   * @param reasonCode The error information for a state failure: \ref agora::rtc::LOCAL_AUDIO_STREAM_REASON "LOCAL_AUDIO_STREAM_REASON".
   */
  virtual void onLocalAudioTrackStateChanged(LOCAL_AUDIO_STREAM_STATE state,
                                             LOCAL_AUDIO_STREAM_REASON reasonCode) = 0;
};

/**
 * `ILocalAudioTrack` is the basic class for local audio tracks, providing main methods of local audio tracks.
 *
 * You can create a local audio track by calling one of the following methods:
 * - `createLocalAudioTrack`
 * - `createCustomAudioTrack`
 * - `createMediaPlayerAudioTrack`
 * @if (!Linux)
 * You can also use the APIs in the \ref agora::rtc::INGAudioDeviceManager "IAudioDeviceManager" class if multiple recording devices are available in the system.
 * @endif
 *
 * After creating local audio tracks, you can publish one or more local audio
 * tracks by calling \ref agora::rtc::ILocalUser::publishAudio "publishAudio".
 */
class ILocalAudioTrack : public IAudioTrack {
 public:
  /**
   * Statistics of a local audio track.
   */
  struct LocalAudioTrackStats {
    /**
     * The source ID of the local audio track.
     */
    uint32_t source_id;
    /**
     * The number of audio frames in the buffer.
     *
     * When sending PCM data, the PCM data is first stored in a buffer area.
     * Then a thread gets audio frames from the buffer and sends PCM data.
     */
    uint32_t buffered_pcm_data_list_size;
    /**
     * The number of audio frames missed by the thread that gets PCM data from the buffer.
     */
    uint32_t missed_audio_frames;
    /**
     * The number of audio frames sent by the thread that gets PCM data from the buffer.
     */
    uint32_t sent_audio_frames;
    /**
     * The number of audio frames sent by the user.
     */
    uint32_t pushed_audio_frames;
    /**
     * The number of dropped audio frames caused by insufficient buffer size.
     */
    uint32_t dropped_audio_frames;
    /**
     * The number of playout audio frames.
     */
    uint32_t playout_audio_frames;
    /**
     * The type of audio effect.
     */
    uint32_t effect_type;
    /**
     * Whether the hardware ear monitor is enabled.
     */
    uint32_t hw_ear_monitor;
    /**
     * Whether the local audio track is enabled.
     */
    bool enabled;
    /**
     * The volume that ranges from 0 to 255.
     */
    uint32_t audio_volume;  // [0,255]

    LocalAudioTrackStats() : source_id(0),
                             buffered_pcm_data_list_size(0),
                             missed_audio_frames(0),
                             sent_audio_frames(0),
                             pushed_audio_frames(0),
                             dropped_audio_frames(0),
                             playout_audio_frames(0),
                             effect_type(0),
                             hw_ear_monitor(0),
                             enabled(false),
                             audio_volume(0) {}
  };

 public:
  /**
   * Enables or disables the local audio track.
   *
   * Once the local audio is enabled, the SDK allows for local audio capturing, processing, and encoding.
   *
   * @param enable Whether to enable the audio track:
   * - `true`: Enable the local audio track.
   * - `false`: Disable the local audio track.
   */
  virtual int setEnabled(bool enable, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Gets whether the local audio track is enabled.
   * @return Whether the local audio track is enabled:
   * - `true`: The local track is enabled.
   * - `false`: The local track is disabled.
   */
  virtual bool isEnabled() const = 0;

  /**
   * Gets the state of the local audio.
   * @return The state of the local audio: #LOCAL_AUDIO_STREAM_STATE: Success.
   */
  virtual LOCAL_AUDIO_STREAM_STATE getState() = 0;

  /**
   * Gets the statistics of the local audio track: LocalAudioTrackStats.
   * @return The statistics of the local audio: LocalAudioTrackStats: Success.
   */
  virtual LocalAudioTrackStats GetStats() = 0;

  /**
   * Adjusts the audio volume for publishing.
   *
   * @param volume The volume for publishing. The value ranges between 0 and 100 (default).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPublishVolume(int volume, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Gets the current volume for publishing.
   * @param volume A pointer to the publishing volume.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPublishVolume(int* volume) = 0;

  /**
   * Enables or disables local playback.
   * @param enable Whether to enable local playback:
   * - `true`: Enable local playback.
   * - `false`: Disable local playback.
   * @param sync Whether to destroy local playback synchronously:
   * - `true`: Destroy local playback synchronously.
   * - `false`: Destroy local playback asynchronously.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableLocalPlayback(bool enable, bool sync = true, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  
  /**
   * Enables in-ear monitoring (for Android and iOS only).
   *
   * @param enabled Determines whether to enable in-ear monitoring.
   * - true: Enable.
   * - false: (Default) Disable.
   * @param includeAudioFilters The type of the ear monitoring: #EAR_MONITORING_FILTER_TYPE
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableEarMonitor(bool enable, int includeAudioFilters, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  /** Register an local audio track observer
   *
   * @param observer A pointer to the local audio track observer: \ref agora::rtc::ILocalAudioTrackObserver
   * "ILocalAudioTrackObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerTrackObserver(ILocalAudioTrackObserver* observer, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  /** Releases the local audio track observer
   *
   * @param observer A pointer to the local audio track observer: \ref agora::rtc::ILocalAudioTrackObserver
   * "ILocalAudioTrackObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterTrackObserver(ILocalAudioTrackObserver* observer) = 0;

  /** set Max buffered audio frame number 
   *
   * @param number : the buffer number set，unit is 10ms 
   *
   */
  virtual void setMaxBufferedAudioFrameNumber(int number, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /** clear sender buffer
   *
   * @return
   * - >= 0: Frame number in sender buffer.
   * - < 0: Failure.
   */
  virtual int ClearSenderBuffer() = 0;
  /**
   * Get the track type of the audio track
   * @return
   * - AudioTrackType
   */
  virtual AudioTrackType getType() OPTIONAL_OVERRIDE { return LOCAL_AUDIO_TRACK; }


  virtual void setAudioFrameSendDelayMs(int32_t delay_ms) = 0;

 protected:
  ~ILocalAudioTrack() {}
};

/**
 * The statistics of a remote audio track.
 */
struct RemoteAudioTrackStats {
  /**
   * The user ID of the remote user who sends the audio track.
   */
  uid_t uid;
  /**
   * The audio quality of the remote audio track: #QUALITY_TYPE.
   */
  int quality;
  /**
   * The network delay (ms) from the sender to the receiver.
   */
  int network_transport_delay;
  /**
   * The delay (ms) from the receiver to the jitter buffer.
   */
  uint32_t jitter_buffer_delay;
  /**
   * The audio frame loss rate in the reported interval.
   */
  int audio_loss_rate;
  /**
   * The number of audio channels.
   */
  int num_channels;
  /**
   * The sample rate (Hz) of the received audio track in the reported interval.
   */
  int received_sample_rate;
  /**
   * The average bitrate (Kbps) of the received audio track in the reported interval.
   * */
  int received_bitrate;
  /**
   * The total freeze time (ms) of the remote audio track after the remote user joins the channel.
   * In a session, audio freeze occurs when the audio frame loss rate reaches 4%.
   * The total audio freeze time = The audio freeze number × 2 seconds.
   */
  int total_frozen_time;
  /**
   * The total audio freeze time as a percentage (%) of the total time when the audio is available.
   * */
  int frozen_rate;
  /**
   * The number of audio bytes received.
   */
  int64_t received_bytes;
  /**
   * The average packet waiting time (ms) in the jitter buffer.
   */
  int mean_waiting_time;
  /**
   * The samples of expanded speech.
   */
  size_t expanded_speech_samples;
  /**
   * The samples of expanded noise.
   */
  size_t expanded_noise_samples;
  /**
   * The timestamps since last report.
   */
  uint32_t timestamps_since_last_report;
  /**
   * The minimum sequence number.
   */
  uint16_t min_sequence_number;
  /**
   * The maximum sequence number.
   */
  uint16_t max_sequence_number;
  /**
   * The audio energy.
   */
  int32_t audio_level;
  /**
   * audio downlink average process time
   */
  uint32_t downlink_process_time_ms;
  /**
   * audio neteq loss because of expired
   */
  uint32_t packet_expired_loss;
  /**
   * audio neteq packet arrival expired time ms
   */
  uint32_t packet_max_expired_ms;
  /**
   * audio neteq jitter peak num in two second
   */
  uint32_t burst_peak_num;
  /**
   * audio neteq jitter calc by burst opti feature
   */
  uint32_t burst_jitter;
  /**
   * audio base target level
   */
  uint32_t target_level_base_ms;
  /**
   * audio average target level
   */
  uint32_t target_level_prefered_ms;
  /**
   * audio average accelerate ratio in 2s
   */
  uint16_t accelerate_rate;
  /**
   * audio average preemptive expand ratio in 2s
   */
  uint16_t preemptive_expand_rate;
  /**
   *  The count of 80 ms frozen in 2 seconds
   */
  uint16_t frozen_count_80_ms;
  /**
   *  The time of 80 ms frozen in 2 seconds
   */
  uint16_t frozen_time_80_ms;
  /**
   *  The count of 200 ms frozen in 2 seconds
   */
  uint16_t frozen_count_200_ms;
  /**
   *  The time of 200 ms frozen in 2 seconds
   */
  uint16_t frozen_time_200_ms;
  /**
   *  The full time of 80 ms frozen in 2 seconds
   */
  uint16_t full_frozen_time_80_ms;
  /**
   *  The full time of 200 ms frozen in 2 seconds
   */
  uint16_t full_frozen_time_200_ms;
  /**
   *  The estimate delay
   */
  uint32_t delay_estimate_ms;
  /**
   *  The MOS value
   */
  uint32_t mos_value;
  /**
   * If the packet loss concealment (PLC) occurs for N consecutive times, freeze is considered as PLC occurring for M consecutive times.
   * freeze cnt = (n_plc - n) / m
   */
  uint32_t frozen_rate_by_custom_plc_count;
  /**
   * The number of audio packet loss concealment
   */
  uint32_t plc_count;
  /**
   *  Duration of inbandfec
   */
  int32_t fec_decode_ms;
  /**
   *  The count of 10 ms frozen in 2 seconds
   */
  uint16_t frozen_count_10_ms;
  /**
   * The total time (ms) when the remote user neither stops sending the audio
   * stream nor disables the audio module after joining the channel.
   */
  uint64_t total_active_time;
  /**
   * The total publish duration (ms) of the remote audio stream.
   */
  uint64_t publish_duration;

  int32_t e2e_delay_ms;
  /**
   * Quality of experience (QoE) of the local user when receiving a remote audio stream. See #EXPERIENCE_QUALITY_TYPE.
   */
  int qoe_quality;
  /**
   * The reason for poor QoE of the local user when receiving a remote audio stream. See #EXPERIENCE_POOR_REASON.
   */
  int32_t quality_changed_reason;

  /**
   * The type of downlink audio effect.
   */
  int32_t downlink_effect_type;

  RemoteAudioTrackStats() :
    uid(0),
    quality(0),
    network_transport_delay(0),
    jitter_buffer_delay(0),
    audio_loss_rate(0),
    num_channels(0),
    received_sample_rate(0),
    received_bitrate(0),
    total_frozen_time(0),
    frozen_rate(0),
    received_bytes(0),
    mean_waiting_time(0),
    expanded_speech_samples(0),
    expanded_noise_samples(0),
    timestamps_since_last_report(0),
    min_sequence_number(0xFFFF),
    max_sequence_number(0),
    audio_level(0),
    downlink_process_time_ms(0),
    packet_expired_loss(0),
    packet_max_expired_ms(0),
    burst_peak_num(0),
    burst_jitter(0),
    target_level_base_ms(0),
    target_level_prefered_ms(0),
    accelerate_rate(0),
    preemptive_expand_rate(0),
    frozen_count_80_ms(0),
    frozen_time_80_ms(0),
    frozen_count_200_ms(0),
    frozen_time_200_ms(0),
    full_frozen_time_80_ms(0),
    full_frozen_time_200_ms(0),
    delay_estimate_ms(0),
    mos_value(0),
    frozen_rate_by_custom_plc_count(0),
    plc_count(0),
    fec_decode_ms(-1),
    frozen_count_10_ms(0),
    total_active_time(0),
    publish_duration(0),
    e2e_delay_ms(0),
    qoe_quality(0),
    quality_changed_reason(0),
    downlink_effect_type(0) {}
};

/**
 * Properties of receive parameters for IAudioEncodedFrameReceiver
 *
 */
struct AudioEncFrameRecvParams {
  /**
   * The callback mode of IAudioEncodedFrameReceiver
   */
  enum ReceiveMode {
    /**
     * IAudioEncodedFrameReceiver callback the down-link audio packet directly
     */
    ORIGINAL = 0,

    /**
     * IAudioEncodedFrameReceiver whill callback the original down-link audio packet while
     * the codec of down-link packet is same as target_codec.
     * 
     * Othewise convert down-link audio packet to new target packet which parameter contain
     * the combination of (target_codec, target_sample_rate, target_sample_rate).
     */
    MATCHED_CODEC = 1,

    /**
     * IAudioEncodedFrameReceiver whill callback the original down-link audio packet while 
     * the combination of (codec, sampling rate, channels) of down-link packet is same as
     * the combination of (target_codec, target_sample_rate, target_sample_rate).
     * 
     * Othewise convert down-link audio packet to new target packet which parameter contain
     * the combination of (target_codec, target_sample_rate, target_sample_rate).
     */
    MATCHED_ALL = 2,
  };

  /**
   * The trans mode of audio packet
   */
  ReceiveMode receive_mode;

  /**
   * The audio codec of target audio packet, Now we only support AACLC
   */
  AUDIO_CODEC_TYPE target_codec;  
  
  /**
   * The sample rate HZ of target audio packet
   */
  int32_t target_sample_rate;
  
  /**
   * The channel numbers of target audio packet
   */
  int32_t target_num_channels;


  AudioEncFrameRecvParams() :
    receive_mode(ORIGINAL),
    target_codec(AUDIO_CODEC_AACLC),
    target_sample_rate(0),
    target_num_channels(0) {}  

  AudioEncFrameRecvParams(const AudioEncFrameRecvParams& src_params) {
    receive_mode = src_params.receive_mode;
    target_codec = src_params.target_codec;
    target_sample_rate = src_params.target_sample_rate;
    target_num_channels = src_params.target_num_channels;    
  }  
};

/**
 * The IRemoteAudioTrack class.
 */
class IRemoteAudioTrack : public IAudioTrack {
 public:
  /**
   * Gets the statistics of the remote audio track.
   * @param stats A reference to the statistics of the remote audio track: RemoteAudioTrackStats.
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool getStatistics(RemoteAudioTrackStats& stats) = 0;

  /**
   * Gets the state of the remote audio.
   * @return The state of the remote audio: #REMOTE_AUDIO_STATE.
   */
  virtual REMOTE_AUDIO_STATE getState() = 0;

  /**
   * Registers an `IMediaPacketReceiver` object.
   *
   * You need to implement the `IMediaPacketReceiver` class in this method. Once you successfully register
   * the media packet receiver, the SDK triggers the `onMediaPacketReceived` callback when it receives an
   * audio packet.
   *
   * @param packetReceiver The pointer to the `IMediaPacketReceiver` object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerMediaPacketReceiver(IMediaPacketReceiver* packetReceiver, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Releases the `IMediaPacketReceiver` object.
   * @param packetReceiver The pointer to the `IMediaPacketReceiver` object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterMediaPacketReceiver(IMediaPacketReceiver* packetReceiver) = 0;

  /**
   * Registers an `IAudioEncodedFrameReceiver` object.
   *
   * You need to implement the `IAudioEncodedFrameReceiver` class in this method. Once you successfully register
   * the media packet receiver, the SDK triggers the `onEncodedAudioFrameReceived` callback when it receives an
   * audio packet.
   *
   * @param packetReceiver The pointer to the `IAudioEncodedFrameReceiver` object.
   * @param recvParams The parameter of receive encoded frame
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioEncodedFrameReceiver(IAudioEncodedFrameReceiver* packetReceiver,
                                                const AudioEncFrameRecvParams& recvParams,
                                                aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /**
   * Releases the `IAudioEncodedFrameReceiver` object.
   * @param packetReceiver The pointer to the `IAudioEncodedFrameReceiver` object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterAudioEncodedFrameReceiver(IAudioEncodedFrameReceiver* packetReceiver) = 0;

  /** Sets the sound position and gain

   @param pan The sound position of the remote user. The value ranges from -1.0 to 1.0:
   - 0.0: the remote sound comes from the front.
   - -1.0: the remote sound comes from the left.
   - 1.0: the remote sound comes from the right.
   @param gain Gain of the remote user. The value ranges from 0.0 to 100.0. The default value is 100.0 (the original gain of the remote user). The smaller the value, the less the gain.

   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int setRemoteVoicePosition(float pan, float gain, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /** Sets the volume of each audio decoded channel

   @param decoded_index The channel index of the remote user. The value ranges from 0 to 100:
   @param volume The channel index of the remote user. The value ranges from 0 to 100.
   - 0: mute the channel.
   - 100: keep the origin volume of the channel.

   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int adjustDecodedAudioVolume(int decoded_index, int volume, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /** mute remote stream from timestamp
   
   @note
   - unmuteRemoteFromTimestamp should be called after muteRemoteFromTimestamp, othewise this stream will be muted all time

   @param timestamp The rtp timestamp of start mute
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int muteRemoteFromTimestamp(uint32_t timestamp) = 0;
  
  /** unmute remote stream from timestamp
   
   @note
   - unmuteRemoteFromTimestamp should be called after muteRemoteFromTimestamp, othewise this stream will be muted all time

   @param timestamp The rtp timestamp of start unmute
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int unmuteRemoteFromTimestamp(uint32_t timestamp) = 0;
  
  /** set percentage of audio acceleration during poor network
   
   @note
   - The relationship between this percentage and the degree of audio acceleration is non-linear and varies with different audio material.

   @param percentage The percentage of audio acceleration. The value ranges from 0 to 100. The higher the
   * percentage, the faster the acceleration. The default value is 100 (no change to the acceleration):
   - 0: disable audio acceleration.
   - > 0: enable audio acceleration.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int adjustAudioAcceleration(int percentage, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /** set percentage of audio deceleration during poor network
   
   @note
   - The relationship between this percentage and the degree of audio deceleration is non-linear and varies with different audio material.

   @param percentage The percentage of audio deceleration. The value ranges from 0 to 100. The higher the
   * percentage, the faster the deceleration. The default value is 100 (no change to the deceleration):
   - 0: disable audio deceleration.
   - > 0: enable audio deceleration.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int adjustAudioDeceleration(int percentage, aosl_ref_t ares = AOSL_REF_INVALID) = 0;  

  /** enable spatial audio
   
   @param enabled enable/disable spatial audio:
   - true: enable spatial audio.
   - false: disable spatial audio.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int enableSpatialAudio(bool enabled, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  /** Sets remote user parameters for spatial audio
   
   @param params spatial audio parameters
   
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int setRemoteUserSpatialAudioParams(const agora::SpatialAudioParams& params, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  virtual AudioTrackType getType() OPTIONAL_OVERRIDE { return REMOTE_AUDIO_TRACK; }
  
};

}  // namespace rtc
}  // namespace agora
