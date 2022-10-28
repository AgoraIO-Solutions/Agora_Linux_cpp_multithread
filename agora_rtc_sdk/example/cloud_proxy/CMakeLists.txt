cmake_minimum_required(VERSION 2.4)
project(DefaultSamples)

# Build sample_send_pcm_by_cloud_proxy
file(GLOB SAMPLE_SEND_PCM_BY_CLOUD_PROXY
     "${PROJECT_SOURCE_DIR}/sample_send_pcm_by_cloud_proxy.cpp"
     "${PROJECT_SOURCE_DIR}/../common/*.cpp")
add_executable(sample_send_pcm_by_cloud_proxy ${SAMPLE_SEND_PCM_BY_CLOUD_PROXY})