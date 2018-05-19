
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)


qt5_wrap_cpp(WRAPPED_CPP
                server.hpp
)

add_executable(whisper
                client.cpp
                client.hpp
                connection_manager.cpp
                connection_manager.hpp
                main.cpp
                main_window.cpp
                main_window.hpp
                server.cpp
                server.hpp
                user_manager.cpp
                user_manager.hpp

                ${WRAPPED_CPP}
)

target_include_directories(whisper SYSTEM PRIVATE ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Netwok_INCLUDE_DIRS})
target_link_libraries(whisper PRIVATE Qt5::Widgets Qt5::Network)

