cmake_minimum_required(VERSION 3.16)

project(streamberry-desktop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets Multimedia)
find_package(Qt6 REQUIRED COMPONENTS WebSockets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(SWSCALE REQUIRED libswscale)

add_executable(streamberry-desktop
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/network/discovery.cpp
    src/network/discovery.h
    src/network/stream_client.cpp
    src/network/stream_client.h
    src/video/decoder.cpp
    src/video/decoder.h
    src/video/virtual_cam.cpp
    src/video/virtual_cam.h
    resources.qrc
)

target_link_libraries(streamberry-desktop PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::WebSockets
    Qt6::Widgets
    Qt6::Multimedia
    ${AVCODEC_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)

target_include_directories(streamberry-desktop PRIVATE
    src
    ${AVCODEC_INCLUDE_DIRS}
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${SWSCALE_INCLUDE_DIRS}
)

# Installation
install(TARGETS streamberry-desktop DESTINATION bin)

if(UNIX AND NOT APPLE)
    install(FILES streamberry-desktop.desktop DESTINATION share/applications)
    install(FILES src/ui/icon.png DESTINATION share/icons/hicolor/256x256/apps RENAME streamberry-desktop.png)
endif()

# Packaging
set(CPACK_PACKAGE_NAME "streamberry-desktop")
set(CPACK_PACKAGE_VENDOR "StreamBerry Labs")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "StreamBerry Desktop Client")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "support@streamberry.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")

# Linux (DEB)
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "StreamBerry Labs")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt6-base-dev, qt6-websockets-dev, libavcodec-dev, libavformat-dev, libavutil-dev, libswscale-dev, v4l2loopback-dkms")
endif()

# Windows (NSIS)
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_DISPLAY_NAME "StreamBerry Desktop")
    set(CPACK_NSIS_PACKAGE_NAME "StreamBerry Desktop")
    # Add icon for installer if available
    # set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/icon.ico")
endif()

include(CPack)
