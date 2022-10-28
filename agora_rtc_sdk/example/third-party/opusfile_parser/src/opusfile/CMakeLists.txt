cmake_minimum_required(VERSION 2.4)

project(libopusfile)

if( ANDROID )
	set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
	set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -s -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g3 -Og")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -s -DNDEBUG")
	set(CMAKE_C_FLAGS_MINSIZEREL "-Os -s -DNDEBUG")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories( . include ../libogg/include ../opus/include )

add_definitions(-DOP_FIXED_POINT)


add_library( opusfile STATIC
src/http.c
src/info.c
src/internal.c
src/opusfile.c
src/stream.c
src/wincerts.c 
)

set_target_properties(opusfile PROPERTIES CLEAN_DIRECT_OUTPUT 1)


