cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)

# Common file parsers
file(GLOB FILE_PARSER_CPP_FILES
     "${PROJECT_SOURCE_DIR}/../common/file_parser/helper_h264_parser.cpp")

# Build sample_stringuid_send
file(GLOB SAMPLE_STRINGUID_SEND_FILES
     "${PROJECT_SOURCE_DIR}/sample_stringuid_send.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_stringuid_send ${SAMPLE_STRINGUID_SEND_FILES}
                                    ${FILE_PARSER_CPP_FILES})

# Build sample_stringuid_receive
file(GLOB SAMPLE_STRINGUID_RECEIVE
     "${PROJECT_SOURCE_DIR}/sample_stringuid_receive.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_stringuid_receive ${SAMPLE_STRINGUID_RECEIVE})

