cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)


# Build sample_send_yuv_pcm
file(GLOB SAMPLE_SEND_YUV_PCM_CPP_FILES
     "${PROJECT_SOURCE_DIR}/sample_send_capture_yuv.cpp"
     "${PROJECT_SOURCE_DIR}/yuv_capture.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_send_yuv_pcm ${SAMPLE_SEND_YUV_PCM_CPP_FILES})

