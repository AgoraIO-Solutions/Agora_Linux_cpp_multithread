cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)


# Build sample_send_ivfvp8
file(GLOB SAMPLE_SEND_IVFVP8_CPP_FILES
     "${PROJECT_SOURCE_DIR}/sample_send_ivfvp8.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_send_ivfvp8 ${SAMPLE_SEND_IVFVP8_CPP_FILES})
