cmake_minimum_required(VERSION 3.1.3)
project(Microservices-based-Chat-System)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/message)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/user)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/file)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/speech)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/transmite)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/friend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/gateway)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})