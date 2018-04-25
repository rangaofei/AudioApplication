# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGE "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
set(LIB_NAME OpenSLUtil)
set(LIB_MP3 Mp3Codec)

#add_library(mp3lame SHARED IMPORTED)
#set_target_properties(mp3lame PROPERTIES IMPORTED_LOCATION
#${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libmp3lame.so)

include_directories(
             src/main/cpp/include
            )
add_subdirectory(src/main/cpp/libmp3lame)
add_library( # Sets the name of the library.
             ${LIB_NAME}

             # Sets the library as a shared library.
             SHARED
             src/main/cpp/opensl_util.cpp
             src/main/cpp/audio_common.cpp
             src/main/cpp/audio_recorder.cpp)
add_library(
            ${LIB_MP3}
            SHARED
            src/main/cpp/mp3codec.cpp
            src/main/cpp/Mp3Encoder.cpp)


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


target_link_libraries( # Specifies the target library.
                        ${LIB_NAME}
                        android
                        atomic
                        OpenSLES
                       ${log-lib} )
target_link_libraries(
                        ${LIB_MP3}
                        ${log-lib}
                        mp3lame
)