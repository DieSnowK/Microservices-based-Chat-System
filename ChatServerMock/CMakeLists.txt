cmake_minimum_required(VERSION 3.16)
project(ChatServerMock VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH 
    "E:/Qt/6.8.0/msvc2022_64"
    "C:/Users/w1752/Desktop/My_Repository/vcpkg/installed/x64-windows"
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets Protobuf
    HttpServer WebSockets Network
)

set(PROJECT_SOURCES 
    main.cpp
    resource.qrc
)

file(GLOB PB_FILES "../client/proto/*.proto")
file(GLOB_RECURSE CORE_FILES "./core/*")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChatServerMock
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CORE_FILES}
    )

    qt_add_protobuf(ChatServerMock PROTO_FILES ${PB_FILES})
endif()

target_include_directories(ChatServerMock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)

target_link_libraries(ChatServerMock PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt6::Protobuf
    Qt6::HttpServer Qt6::WebSockets Qt6::Network
)

set_target_properties(ChatServerMock PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ChatServerMock
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChatServerMock)
endif()
