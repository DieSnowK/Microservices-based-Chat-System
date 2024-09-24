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
    string(REPLACE )