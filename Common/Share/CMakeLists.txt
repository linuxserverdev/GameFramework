set(TARGET share)
add_library(${TARGET} SHARED)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SHARE_SOURCES)

set(SHARE_SOURCES ${SHARE_SOURCES})

target_sources(${TARGET} PRIVATE ${SHARE_SOURCES})
target_link_libraries(${TARGET} PUBLIC logger dl)

add_subdirectory(tests)