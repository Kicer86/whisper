
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(OpenLibrary 2.4 REQUIRED COMPONENTS utils_qt)
find_package(OpenSSL 1.1.1 REQUIRED)

add_executable(whisper
                encryption/encrypted_connection.cpp
                encryption/encrypted_connection.hpp
                encryption/encrypted_server.cpp
                encryption/encrypted_server.hpp
                encryption/iencrypted_connection.hpp
                iuser_manager.hpp
                main.cpp
                main_window.cpp
                main_window.hpp
                server.cpp
                server.hpp
                user_keys_manager.cpp
                user_keys_manager.hpp
                user_list_widget.cpp
                user_list_widget.hpp
                user_manager.cpp
                user_manager.hpp
)

target_include_directories(whisper
                            SYSTEM PRIVATE
                                ${Qt5Widgets_INCLUDE_DIRS}
                                ${Qt5Netwok_INCLUDE_DIRS}
                                ${OPENLIBRARY_INCLUDE_DIRS}
)

target_link_libraries(whisper
                        PRIVATE
                        Qt5::Widgets
                        Qt5::Network
                        OpenLibrary::utils_qt
                        OpenSSL::Crypto
)


set_target_properties(whisper PROPERTIES AUTOMOC TRUE)
