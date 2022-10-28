cmake_minimum_required(VERSION 2.4)
project(opusfile_parser)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/${MACHINE})

add_subdirectory(src/ogg)
add_subdirectory(src/opus)
add_subdirectory(src/opusfile)
