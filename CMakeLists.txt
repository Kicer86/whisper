
cmake_minimum_required(VERSION 3.0)

project(whisper)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

add_subdirectory(src)

if(ANDROID)
    include(QtAndroidCMake/AddQtAndroidApk.cmake)
    add_qt_android_apk(whisper_apk whisper)
endif()
