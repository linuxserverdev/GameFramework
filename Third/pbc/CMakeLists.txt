set(TARGET pbc)
add_library(${TARGET} SHARED)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PBC_SOURCES)

#set(PBC_SOURCES ${PBC_SOURCES} PbR.cc PbW.cc)

target_sources(${TARGET} PRIVATE ${PBC_SOURCES})
target_include_directories(${TARGET}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_subdirectory(tests)