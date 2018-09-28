
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGE "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
set(OPENSL_LIB OpenSLUtil)
set(LIB_MP3 Mp3Codec)
set(OPENSL_PLAYER OpenSLPlayer)
include_directories(
             src/main/cpp/include
            )
add_subdirectory(src/main/cpp/libmp3lame)

add_subdirectory(src/main/cpp/lameutils)

add_library(
             ${OPENSL_LIB}
             SHARED
             src/main/cpp/opensl_util.cpp
             src/main/cpp/audio_common.cpp
             src/main/cpp/audio_recorder.cpp)
add_library(
            ${LIB_MP3}
            SHARED
            src/main/cpp/mp3codec.cpp
       )

add_library(
            ${OPENSL_PLAYER}
            SHARED
            src/main/cpp/OpenSLPlayer.cpp
            src/main/cpp/WavTools.cpp)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


target_link_libraries( # Specifies the target library.
                        ${OPENSL_LIB}
                        android
                        atomic
                        OpenSLES
                       ${log-lib}
                        ${LIB_MP3}
                        )
target_link_libraries(${LIB_MP3}
                        ${log-lib}
                        #mp3lame
                        lame_utils
)
target_link_libraries(${OPENSL_PLAYER}
                        ${log-lib}
                        OpenSLES
                        android
                        )