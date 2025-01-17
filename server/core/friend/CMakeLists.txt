cmake_minimum_required(VERSION 3.1.3)
project(friend_server)

set(TARGET "friend_server")

# PROTO
set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../src/proto)
set(PROTO_FILES base.proto user.proto friend.proto message.proto)

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

# ODB
set(ODB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../src/odb)
set(ODB_FILES chat_session_member.hpp chat_session.hpp friend_apply.hpp relation.hpp)

set(ODB_HXX "")
set(ODB_CXX "")
set(ODB_SRCS "")
foreach(ODB_FILE ${ODB_FILES})
    string(REPLACE ".hpp" "-odb.hxx" ODB_HXX ${ODB_FILE})
    string(REPLACE ".hpp" "-odb.cxx" ODB_CXX ${ODB_FILE})
    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX})
        add_custom_command(
            COMMAND odb
            ARGS -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time ${ODB_PATH}/${ODB_FILE}
            DEPENDS ${ODB_PATH}/${ODB_FILE}
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX}
            COMMENT "Generate an ODB framework code file: " ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX}
        )
    endif()
    list(APPEND ODB_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX})
endforeach()

# Generate
set(SRC_FILES "")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SRC_FILES)
add_executable(${TARGET} ${SRC_FILES} ${PROTO_SRCS} ${ODB_SRCS})
target_link_libraries(${TARGET} 
    -lgflags -lspdlog -lfmt -lbrpc -lssl -lcrypto 
    -lprotobuf -lleveldb -letcd-cpp-api 
    -lcpprest -lcurl -lodb-mysql -lodb -lodb-boost
    -lcpr -lelasticlient
    /usr/local/lib/libjsoncpp.so.19
)

# set(TEST_CLIENT "friend_client")
# set(TEST_FILES "")
# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/test/test TEST_FILES)
# add_executable(${TEST_CLIENT} ${TEST_FILES} ${PROTO_SRCS})
# target_link_libraries(${TEST_CLIENT} 
#     -pthread -lgtest -lgflags -lspdlog -lfmt 
#     -lbrpc -lssl -lcrypto -lprotobuf -lleveldb 
#     -letcd-cpp-api -lcpprest -lcurl 
#     /usr/local/lib/libjsoncpp.so.19
# )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/odb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../third/)

# INSTALL(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
INSTALL(TARGETS ${TARGET} ${TEST_CLIENT} RUNTIME DESTINATION bin)