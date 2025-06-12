cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)

# Common file parsers
file(GLOB FILE_PARSER_CPP_FILES
     "${PROJECT_SOURCE_DIR}/../common/file_parser/helper_h265_parser.cpp")

# Build sample_send_h264_pcm
file(GLOB SAMPLE_SEND_H265
     "${PROJECT_SOURCE_DIR}/sample_send_h265.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_send_h265 ${SAMPLE_SEND_H265}
                                    ${FILE_PARSER_CPP_FILES})

