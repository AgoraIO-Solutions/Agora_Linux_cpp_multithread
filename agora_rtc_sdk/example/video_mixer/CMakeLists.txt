cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)


# Build sample_send_h264_pcm
file(GLOB SAMPLE_VIDEO_MIXER_FILES
     "${PROJECT_SOURCE_DIR}/video_mixer.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_receive_video_and_mix ${SAMPLE_VIDEO_MIXER_FILES}
                                    ${FILE_PARSER_CPP_FILES})
