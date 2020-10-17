set(TARGET lua_engine)
add_library(${TARGET} SHARED)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} LUA_ENGINE_SOURCES)

set(EXPORT_CC ${CMAKE_CURRENT_SOURCE_DIR}/export_lua_byte_buffer.cc)
set(EXPORT_PKG ${CMAKE_CURRENT_SOURCE_DIR}/ExportLuaByteBuffer.pkg)
add_custom_command(
        OUTPUT ${EXPORT_CC}
        DEPENDS ${EXPORT_PKG}
        COMMAND tolua++ -o ${EXPORT_CC} -n export_lua_byte_buffer ${EXPORT_PKG}
        COMMENT "Generating ${EXPORT_CC} from: ${EXPORT_PKG}")

set(LUA_ENGINE_SOURCES ${LUA_ENGINE_SOURCES} ${EXPORT_CC})

target_sources(${TARGET} PRIVATE ${LUA_ENGINE_SOURCES})
target_link_libraries(${TARGET} PRIVATE share toluapp)

add_subdirectory(tests)