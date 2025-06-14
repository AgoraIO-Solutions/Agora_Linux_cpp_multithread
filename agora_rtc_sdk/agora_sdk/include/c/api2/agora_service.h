//
//  Agora C SDK
//
//  Created by Ender Zheng in 2020.5
//  Copyright (c) 2020 Agora.io. All rights reserved.
//
#pragma once

#include "agora_base.h"
//#include "api2/agora_rtc_connection.h" // for rtc_conn_config

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct _audio_encoder_config {
  /**
   * The audio profile: #AUDIO_PROFILE_TYPE
   */
  int audio_profile;
} audio_encoder_config;

typedef struct _agora_service_config {
  /**
   * Determines whether to enable the audio processor.
   * - `true`: (Default) Enable the audio processor. Once enabled, the underlying
   * audio processor is initialized in advance.
   * - `false`: Disable the audio processor. Set this member
   * as `false` if you do not need audio at all.
   */
  int enable_audio_processor;
  /**
   * Determines whether to enable the audio device.
   * - `true`: (Default) Enable the audio device. Once enabled, the underlying
   * audio device module is initialized in advance to support audio
   * recording and playback.
   * - `false`: Disable the audio device. Set this member as `false` if
   * you do not need to record or play the audio.
   *
   * @note
   * When this member is set as `false`, and `enableAudioProcessor` is set as `true`,
   * you can still pull the PCM audio data.
   */
  int enable_audio_device;
  /**
   * Determines whether to enable video.
   * - `true`: Enable video. Once enabled, the underlying video engine is
   * initialized in advance.
   * - `false`: (Default) Disable video. Set this parameter as `false` if you
   * do not need video at all.
   */
  int enable_video;
  /**
   * The user context, for example, the activity context in Android.
   */
  void* context;
  /**
   * The App ID of your project
   */
  const char* app_id;
  /**
   * The supported area code, default is AREA_CODE_GLOB
   */
  unsigned int area_code;
  /** 
   * The channel profile.
   */
  int channel_profile;
  /**
   * The default audio scenario.
   */
  int audio_scenario;
  /**
   * Whether to enable string uid.
   */
  int use_string_uid;
  /**
   * Determines whether to enable domain limit.
   * - `1`: only connect to servers that already parsed by DNS
   * - `0`: connect to servers with no limit
   */
  int domain_limit;
} agora_service_config;

/**
 * The global audio session configuration.
 */
typedef struct _audio_session_config {
  /**
   * Determines whether to enable recording (input) and playback (output) of audio:
   * - `true`: Enable audio recording and playback.
   * - `false`: Disable audio recording or playback, which prevents audio input
   * and output.
   *
   * @note
   * - For the recording function to work, the user must grant permission for audio recording.
   * - By default, your app's audio is nonmixable, which means
   * activating audio sessions in your app interrupts other nonmixable audio sessions.
   * To allow mixing, set `allowMixWithOthers` as `true`.
   */
  int playback_and_record;
  /**
   * Determines whether to enable the chat mode:
   * - `true`: Enable the chat mode. Specify this mode is your app is engaging in two-way
   * real-time communication, such as a voice or video chat.
   * - `false`: Disable the chat mode.
   *
   * For a video chat, set this member as true and set the audio route to the speaker.
   */
  int chat_mode;
  /**
   * Determines whether audio from this session defaults to the built-in speaker instead
   * of the receiver:
   * - `true`: Audio from this session defaults to the built-in speaker instead
   * of the receiver.
   * - `false`: Audio from this session does not default to the built-in speaker instead
   * of the receiver.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  int default_to_speaker;
  /**
   * Determines whether to temporarily change the current audio route to the built-in speaker:
   * - `true`: Set the current audio route to the built-in speaker.
   * - `false`: Do not set the current audio route to the built-in speaker.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  int override_speaker;
  /**
   * Determines whether audio from this session is mixed with audio from active sessions
   * in other audio apps.
   * - `true`: Mix audio from this session with audio from active sessions in
   * other audio apps, that is, your app's audio is mixed with audio playing in background
   * apps.
   * - `false`: Do not mix audio from this session with audio from active sessions in
   * other audio apps.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  int allow_mix_with_others;
  /**
   * Determines whether audio from this session is ducked with audio from active sessions
   * in other audio apps.
   * - `true`: Duck audio from this session with audio from active sessions in
   * other audio apps, that is, your app's audio is mixed with audio playing in background
   * apps.
   * - `false`: Do not duck audio from this session with audio from active sessions in
   * other audio apps.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  int allow_duck_others;
  /**
   * Determines whether Bluetooth handsfree devices appear as available audio input
   * routes:
   * - `true`: Bluetooth handsfree devices appear as available audio input routes.
   * - `false`: Bluetooth handsfree devices do not appear as available audio input
   * routes.
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  int allow_bluetooth;
  /**
   * Determines whether audio from the current session can be streamed to Bluetooth
   * devices that support the Advanced Audio Distribution Profile (A2DP).
   * - `true`: Audio from the current session can be streamed to Bluetooth devices that
   * support the Advanced Audio Distribution Profile (A2DP).
   * - `false`: Audio from the current session cannot be streamed to Bluetooth devices that
   * support the Advanced Audio Distribution Profile (A2DP).
   *
   * This member is available only when the `playbackAndRecord` member is set as `true`.
   */
  int allow_bluetooth_a2dp;
  /**
   * Sets the preferred hardware sample rate (kHz) for the session. The value range is
   * [8, 48]. Depending on the hardware in use, the actual sample rate might be different.
   */
  double sample_rate;
  /**
   * Sets the preferred hardware input and output buffer duration (ms) for the session.
   * The value range is [0, 93]. Depending on the hardware in use, the actual I/O buffer
   * duration might be lower.
   */
  double io_buffer_duration;
  /**
   * Sets the preferred number of audio input channels for the current route.
   */
  int input_number_of_channels;
  /**
   * Sets the preferred number of audio output channels for the current route.
   */
  int output_number_of_channels;
} audio_session_config;

/**
 * The IServiceObserver class.
 * @ANNOTATION:TYPE:OBSERVER
 */
typedef struct _service_observer {
  void (*on_permission_error)(AGORA_HANDLE, int permission_type);
  void (*on_audio_device_error)(AGORA_HANDLE, int error, const char* description);
} service_observer;

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_service
 */
AGORA_API_C_HDL agora_service_create();

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_INT agora_service_initialize(AGORA_HANDLE agora_svc, const agora_service_config* config);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_VOID agora_service_at_exit(AGORA_HANDLE agora_svc);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:DTOR:agora_service
 */
AGORA_API_C_INT agora_service_release(AGORA_HANDLE agora_svc);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_INT agora_service_set_audio_session_preset(AGORA_HANDLE agora_svc, int audio_scenario);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_INT agora_service_set_audio_session_config(AGORA_HANDLE agora_svc, const audio_session_config* config);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C audio_session_config* AGORA_CALL_C agora_service_get_audio_session_config(AGORA_HANDLE agora_svc);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_VOID agora_service_destroy_audio_session_config(AGORA_HANDLE agora_svc, audio_session_config* config);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_INT agora_service_set_log_file(AGORA_HANDLE agora_svc, const char* file_path, unsigned int file_size);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_INT agora_service_set_log_filter(AGORA_HANDLE agora_svc, unsigned int filters);

/**
 * @ANNOTATION:GROUP:agora_local_audio_track
 * @ANNOTATION:DTOR:agora_local_audio_track
 */
AGORA_API_C_VOID agora_local_audio_track_destroy(AGORA_HANDLE agora_local_audio_track);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_audio_track
 */
AGORA_API_C_HDL agora_service_create_local_audio_track(AGORA_HANDLE agora_svc);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_audio_track
 */
AGORA_API_C_HDL agora_service_create_custom_audio_track_pcm(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_audio_pcm_data_sender /* pointer to RefPtrHolder */);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_audio_track
 */
AGORA_API_C_HDL agora_service_create_custom_audio_track_encoded(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_audio_encoded_frame_sender, int mix_mode);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_audio_track
 */
AGORA_API_C_HDL agora_service_create_custom_audio_track_packet(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_media_packet_sender);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_audio_track
 */
AGORA_API_C_HDL agora_service_create_media_player_audio_track(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_media_player_source);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_audio_track
 */
AGORA_API_C_HDL agora_service_create_recording_device_audio_track(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_record_device);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_audio_device_manager
 */
AGORA_API_C_HDL agora_service_create_audio_device_manager(AGORA_HANDLE agora_svc);

/**
 * @ANNOTATION:GROUP:agora_audio_device_manager
 * @ANNOTATION:DTOR:agora_audio_device_manager
 */
AGORA_API_C_VOID agora_audio_device_manager_destroy(AGORA_HANDLE agora_audio_device_manager);


/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_media_node_factory
 */
AGORA_API_C_HDL agora_service_create_media_node_factory(AGORA_HANDLE agora_svc);

/**
 * @ANNOTATION:GROUP:agora_media_node_factory
 * @ANNOTATION:DTOR:agora_media_node_factory
 */
AGORA_API_C_VOID agora_media_node_factory_destroy(AGORA_HANDLE agora_media_node_factory);

/**
 * @ANNOTATION:GROUP:agora_local_video_track
 * @ANNOTATION:DTOR:agora_local_video_track
 */
AGORA_API_C_VOID agora_local_video_track_destroy(AGORA_HANDLE agora_local_video_track);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_video_track
 */
AGORA_API_C_HDL agora_service_create_camera_video_track(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_camera_capturer);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_video_track
 */
// AGORA_API_C_HDL agora_service_create_screen_video_track(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_screen_capturer);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_video_track
 */
AGORA_API_C_HDL agora_service_create_mixed_video_track(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_video_mixer);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_video_track
 */
AGORA_API_C_HDL agora_service_create_custom_video_track_frame(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_video_frame_sender);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_video_track
 */
AGORA_API_C_HDL agora_service_create_custom_video_track_encoded(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_video_encoded_image_sender, sender_options* options);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_video_track
 */
AGORA_API_C_HDL agora_service_create_custom_video_track_packet(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_media_packet_sender);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_local_video_track
 */
AGORA_API_C_HDL agora_service_create_media_player_video_track(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_media_player_source);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_rtmp_streaming_service
 */
AGORA_API_C_HDL agora_service_create_rtmp_streaming_service(AGORA_HANDLE agora_svc, AGORA_HANDLE agora_rtc_conn, const char* app_id);

/**
 * @ANNOTATION:GROUP:agora_rtmp_streaming_service
 * @ANNOTATION:DTOR:agora_rtmp_streaming_service
 */
AGORA_API_C_VOID agora_rtmp_streaming_service_destroy(AGORA_HANDLE agora_rtmp_streaming_service);

/**
 * @ANNOTATION:GROUP:agora_service
 * @ANNOTATION:CTOR:agora_rtm_service
 */
AGORA_API_C_HDL agora_service_create_rtm_service(AGORA_HANDLE agora_svc);

/**
 * @ANNOTATION:GROUP:agora_rtm_service
 * @ANNOTATION:DTOR:agora_rtm_service
 */
AGORA_API_C_VOID agora_rtm_service_destroy(AGORA_HANDLE agora_rtm_service);

/**
 * @ANNOTATION:GROUP:agora_service
 */
#if defined(_WIN32) || defined(__linux__) || defined(__ANDROID__)
  AGORA_API_C_INT agora_service_load_extension_provider(AGORA_HANDLE agora_svc, const char* path, unsigned int unload_after_use);
#endif

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_INT agora_service_enable_extension(AGORA_HANDLE agora_svc, const char* provider_name, const char* extension_name, 
                              const char* track_id, unsigned int auto_enable_on_track);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_INT agora_service_disable_extension(AGORA_HANDLE agora_svc, const char* provider_name, const char* extension_name, const char* track_id);

/**
 * @ANNOTATION:GROUP:agora_service
 */
AGORA_API_C_HDL agora_service_get_agora_parameter(AGORA_HANDLE agora_svc);

#ifdef __cplusplus
}
#endif  // __cplusplus
