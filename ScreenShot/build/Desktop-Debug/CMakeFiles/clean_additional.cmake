# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/ScreenShot_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ScreenShot_autogen.dir/ParseCache.txt"
  "ScreenShot_autogen"
  )
endif()
