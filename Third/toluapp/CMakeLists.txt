
include_directories (./ src/lib ../lua/lib)

# Build lib
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/lib SRC_LIBTOLUAPP)
add_library (toluapp SHARED)
target_sources(toluapp PRIVATE ${SRC_LIBTOLUAPP})
target_link_libraries(toluapp PUBLIC lua51)

# Build app
include_directories (src/bin)
set (SRC_TOLUA src/bin/tolua.c src/bin/toluabind.c)
add_executable (tolua++ ${SRC_TOLUA} )
target_link_libraries (tolua++ toluapp lua51)


