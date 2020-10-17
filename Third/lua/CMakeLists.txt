
add_definitions(-DLUA_USE_LINUX)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} LUA_LIB_SOURCES)

add_library(lua51 SHARED)
target_sources(lua51 PRIVATE ${LUA_LIB_SOURCES})
target_link_libraries(lua51 PRIVATE m dl)

add_subdirectory(tests)