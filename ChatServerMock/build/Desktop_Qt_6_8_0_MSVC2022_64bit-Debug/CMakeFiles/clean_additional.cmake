# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ChatServerMock_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ChatServerMock_autogen.dir\\ParseCache.txt"
  "ChatServerMock_autogen"
  )
endif()
