cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)

# Common file parsers
file(GLOB FILE_PARSER_CPP_FILES
     "${PROJECT_SOURCE_DIR}/../common/file_parser/helper_h264_parser.cpp")

# Build sample_receive_mixed_audio
file(GLOB SAMPLE_RECEIVE_MIXED_AUDIO_FILES
     "${PROJECT_SOURCE_DIR}/sample_receive_mixed_audio.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_receive_mixed_audio ${SAMPLE_RECEIVE_MIXED_AUDIO_FILES})