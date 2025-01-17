cmake_minimum_required(VERSION 3.1.3)
project(file_server)

set(TARGET "file_server")

# PROTO
set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../src/proto)
set(PROTO_FILES file.proto base.proto)

set(PROTO_CC "")
set(PROTO_H "")
set(PROTO_SRCS "")

foreach(PROTO_FILE ${PROTO_FILES})
    string(REPLACE ".proto" ".pb.cc" PROTO_CC ${PROTO_FILE})
    string(REPLACE ".proto" ".pb.h" PROTO_H  ${PROTO_FILE})
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}${PROTO_CC})
        add_custom_command(
            PRE_BUILD
            COMMAND protoc
            ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR} -I ${PROTO_PATH}
                --experimental_allow_proto3_optional ${PROTO_PATH}/${PROTO_FILE}
            DEPENDS ${PROTO_PATH}/${PROTO_FILE}
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_CC}
            COMMENT "Generate a Protobuf framework code file: "
                ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_CC}
        )
    endif()
    list(APPEND PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_CC})
endforeach()

# Generate
set(SRC_FILES "")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SRC_FILES)
add_executable(${TARGET} ${SRC_FILES} ${PROTO_SRCS})
target_link_libraries(${TARGET} 
    -lgflags -lspdlog -lfmt -lbrpc -lssl -lcrypto
    -lprotobuf -lleveldb -letcd-cpp-api -lcpprest -lcurl
)

# set(TEST_CLIENT "file_client")
# set(TEST_FILES "")
# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/test TEST_FILES)
# add_executable(${TEST_CLIENT} ${TEST_FILES} ${PROTO_SRCS})
# target_link_libraries(${TEST_CLIENT} 
#     -lgtest -lgflags -lspdlog -lfmt -lbrpc -lssl -lcrypto
#     -lprotobuf -lleveldb -letcd-cpp-api -lcpprest -lcurl
# )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../third/)

INSTALL(TARGETS ${TARGET} RUNTIME DESTINATION bin)