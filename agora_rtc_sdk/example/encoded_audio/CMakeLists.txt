cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)

# Common file parsers
file(GLOB FILE_PARSER_CPP_FILES
     "${PROJECT_SOURCE_DIR}/../common/file_parser/helper_h264_parser.cpp"
     "${PROJECT_SOURCE_DIR}/../common/file_parser/helper_aac_parser.cpp")

# Opus file parser
file(GLOB OPUS_FILE_PARSER_CPP_FILES
     "${PROJECT_SOURCE_DIR}/../common/file_parser/helper_opus_parser.cpp")

# Build sample_send_aac
file(GLOB SAMPLE_SEND_AAC_CPP_FILES "${PROJECT_SOURCE_DIR}/sample_send_aac.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_send_aac ${SAMPLE_SEND_AAC_CPP_FILES}
                               ${FILE_PARSER_CPP_FILES})
# Build sample_send_h264_opusw
file(GLOB SAMPLE_SEND_H264_OPUS_CPP_FILES
     "${PROJECT_SOURCE_DIR}/sample_send_opus.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")

add_subdirectory(${THIRD_PARTY}/opusfile_parser/ build)

add_executable(
  sample_send_opus ${SAMPLE_SEND_H264_OPUS_CPP_FILES}
                        ${FILE_PARSER_CPP_FILES} ${OPUS_FILE_PARSER_CPP_FILES})
target_compile_definitions(sample_send_opus PRIVATE __SUPPORT_OPUS__)
target_link_libraries(sample_send_opus opusfile opus ogg)

# Build sample_receive_encoded_audio
file(GLOB SAMPLE_RECEIVE_ENCODED_AUDIO_CPP_FILES
     "${PROJECT_SOURCE_DIR}/sample_receive_encoded_audio.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")

add_executable(
          sample_receive_encoded_audio ${SAMPLE_RECEIVE_ENCODED_AUDIO_CPP_FILES}
                         ${FILE_PARSER_CPP_FILES})