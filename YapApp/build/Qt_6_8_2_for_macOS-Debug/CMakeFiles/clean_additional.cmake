# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/YapApp_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/YapApp_autogen.dir/ParseCache.txt"
  "YapApp_autogen"
  )
endif()
