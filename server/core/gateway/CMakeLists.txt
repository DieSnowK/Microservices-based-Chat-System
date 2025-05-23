cmake_minimum_required(VERSION 3.1.3)
project(gateway_server)

set(TARGET "gateway_server")

# PROTO
set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../src/proto)
set(PROTO_FILES 
    base.proto user.proto file.proto friend.proto gateway.proto 
    message.proto notify.proto speech.proto transmite.proto
)

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
add_executable(${TARGET} ${SRC_FILES} ${PROTO_SRCS} ${ODB_SRCS})
target_link_libraries(${TARGET} 
    -lgflags -lspdlog -lfmt -lbrpc -lssl -lcrypto 
    -lprotobuf -lleveldb -letcd-cpp-api -lodb-mysql 
    -lodb -lodb-boost -lhiredis -lredis++
    -lcpprest -lcurl -lpthread -lboost_system
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/odb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../third/)

# INSTALL(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
INSTALL(TARGETS ${TARGET} RUNTIME DESTINATION bin)