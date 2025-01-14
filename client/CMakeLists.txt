cmake_minimum_required(VERSION 3.16)
project(ChatClient VERSION 0.1 LANGUAGES CXX)

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
    Widgets Protobuf Network
    Multimedia WebSockets
)

set(PROJECT_SOURCES 
    main.cpp
    resource.qrc
)

file(GLOB PB_FILES "./proto/*.proto")
file(GLOB_RECURSE CORE_FILES "./core/*")
file(GLOB NET_FILES "./network/*")
file(GLOB MODEL_FILES "./model/*")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChatClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CORE_FILES}
        ${NET_FILES}
        ${MODEL_FILES}
    )

    qt_add_protobuf(ChatClient PROTO_FILES ${PB_FILES})
endif()

target_include_directories(ChatClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/network
)

target_link_libraries(ChatClient PRIVATE 
	Qt${QT_VERSION_MAJOR}::Widgets 
	Qt6::Network Qt6::WebSockets 
	Qt6::Protobuf Qt6::Multimedia
)

set_target_properties(ChatClient PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ChatClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChatClient)
endif()
