try_compile(TOOLCHAIN_CHECK /tmp/$ENV{USER}-CMakeTest ${CMAKE_SOURCE_DIR}/scripts/.test.c
		CMAKE_FLAGS "-DLINK_LIBRARIES=${LIB_SDK}"
		CMAKE_FLAGS "-DLINK_DIRECTORIES=${CMAKE_SOURCE_DIR}/../agora_sdk/lib/"
    OUTPUT_VARIABLE OUTPUT_CHECK
    )
if(NOT TOOLCHAIN_CHECK)
#  message("${OUTPUT_CHECK}")
  message(FATAL_ERROR "\nTOOLCHAIN ERROR! Modify the toolchain script for your cross compiler:\n${CMAKE_TOOLCHAIN_FILE}\n")
endif()
