
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(OpenLibrary 2.4 REQUIRED COMPONENTS utils_qt)
find_package(Botan 2.12 REQUIRED)

set(WHISPER_SOURCES
                encryption/connection_monitor.cpp
                encryption/connection_monitor.hpp
                encryption/encrypted_client.cpp
                encryption/encrypted_client.hpp
                encryption/encrypted_connection.cpp
                encryption/encrypted_connection.hpp
                encryption/encrypted_server.cpp
                encryption/encrypted_server.hpp
                encryption/iconnection_manager.hpp
                encryption/identity.cpp
                encryption/identity.hpp
                encryption/iencrypted_connection.hpp
                encryption/iidentity_checker.hpp
                encryption/iencryption_primitives_provider.hpp
                connection_manager.cpp
                connection_manager.hpp
                identity_checker.cpp
                identity_checker.hpp
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
                utils.hpp
)

if (ANDROID)
    add_library(whisper SHARED ${WHISPER_SOURCES})
else()
    add_executable(whisper ${WHISPER_SOURCES})
endif()

target_include_directories(whisper
                            SYSTEM PRIVATE
                                ${Qt5Widgets_INCLUDE_DIRS}
                                ${Qt5Netwok_INCLUDE_DIRS}
                                ${OPENLIBRARY_INCLUDE_DIRS}
                                ${BOTAN_INCLUDE_DIRS}
)

target_link_libraries(whisper
                        PRIVATE
                        Qt5::Widgets
                        Qt5::Network
                        OpenLibrary::utils_qt
                        ${BOTAN_LIBRARIES}
)


set_target_properties(whisper PROPERTIES AUTOMOC TRUE)


if(ANDROID)
    include(${CMAKE_SOURCE_DIR}/QtAndroidCMake/AddQtAndroidApk.cmake)
    add_qt_android_apk(whisper_apk whisper
        DEPENDS ${BOTAN_LIBRARIES} OpenLibrary::utils_qt
    )
endif()
