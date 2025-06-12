cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)

# Build sample_send_h264_pcm
file(GLOB SAMPLE_SEND_DATASTREAM_CPP_FILES
     "${PROJECT_SOURCE_DIR}/sample_send_datastream.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_send_datastream ${SAMPLE_SEND_DATASTREAM_CPP_FILES})

file(GLOB SAMPLE_RECE_DATASTREAM_CPP_FILES
     "${PROJECT_SOURCE_DIR}/sample_receive_datastream.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_receive_datastream ${SAMPLE_RECE_DATASTREAM_CPP_FILES})