#cmake获取当前文件夹名作为当前项目的TAGET
string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
set(TARGET ${CURRENT_FOLDER})
add_library(${TARGET} SHARED)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} LOGGER_SOURCES)

set(LOGGER_SOURCES ${LOGGER_SOURCES} ../share/Date.cc ../share/Utilities.cc)

target_sources(${TARGET} PRIVATE ${LOGGER_SOURCES})
target_link_libraries(${TARGET} PRIVATE pthread)

add_subdirectory(tests)