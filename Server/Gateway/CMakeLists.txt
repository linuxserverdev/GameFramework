set(INC_LIST ${INC_LIST} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(AFTER ${INC_LIST})

add_subdirectory(GateEngine)
add_subdirectory(GateFrame)