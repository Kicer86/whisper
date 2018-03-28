
find_package(Qt5Widgets REQUIRED)

add_executable(whisper main.cpp)

target_include_directories(whisper SYSTEM PRIVATE ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(whisper PRIVATE Qt5::Widgets)
