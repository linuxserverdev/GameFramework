set(TARGET google)
add_library(${TARGET} SHARED)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} GOOGLE_SOURCES)

set(GOOGLE_SOURCES ${GOOGLE_SOURCES})

target_sources(${TARGET} PRIVATE ${GOOGLE_SOURCES})
target_link_libraries(${TARGET} PUBLIC)


set(INC_LIST ${INC_LIST} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(AFTER ${INC_LIST})
add_subdirectory(tests)