# 1.添加cmake版本说明
cmake_minimum_required(VERSION 3.1.3)

# 2.声明工程名称
project(odb-test)

# 3.检测并生成ODB框架代码
#   1.添加所需的ODB映射代码文件名称
set(ODB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/entity)
set(ODB_FILES student.hpp)
#   2.检测框架代码文件是否已经生成
set(ODB_HXX "")
set(ODB_CXX "")
set(ODB_SRCS "")
foreach(ODB_FILE ${ODB_FILES})
#   3.如果没有生成, 则预定义生成指令 -- 用于在构建项目之前先生成框架代码
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
#   4.将所有生成的框架源码文件名称都保存起来
    list(APPEND ODB_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX})
endforeach()

# 4.获取源码目录下的所有源码文件
set(SRC_FILES "")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SRC_FILES)

# 5.声明目标及依赖
add_executable(main ${SRC_FILES} ${ODB_SRCS})

# 6.设置头文件默认搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/entity)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# 7.设置需要链接的库
target_link_libraries(main odb-mysql odb odb-boost gflags)

# 8.设置安装路径
INSTALL(TARGETS main RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)