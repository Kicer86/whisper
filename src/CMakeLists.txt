
find_package(Qt5Widgets REQUIRED)


qt5_wrap_cpp(WRAPPED_CPP
                server.hpp
)

add_executable(whisper
                main.cpp
                main_window.cpp
                main_window.hpp
                server.cpp
                server.hpp

                ${WRAPPED_CPP}
)

target_include_directories(whisper SYSTEM PRIVATE ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries(whisper PRIVATE Qt5::Widgets)

