set(TARGET gateway_server)

#set(API2LUA_CC ${CMAKE_CURRENT_SOURCE_DIR}/api2lua.cc)
#set(API2LUA_PKG ${CMAKE_CURRENT_SOURCE_DIR}/pkg/api2lua.pkg)
#add_custom_command(
#        OUTPUT ${API2LUA_CC}
#        DEPENDS ${API2LUA_PKG}
#        COMMAND tolua++ -o ${API2LUA_CC} -n api2lua ${API2LUA_PKG}
#        COMMENT "Generating ${API2LUA_CC} from: ${API2LUA_PKG}")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} GATEWAY_FRAME_SOURCES)
add_executable(${TARGET} ${GATEWAY_FRAME_SOURCES})
target_link_libraries(${TARGET} PRIVATE logger lua51 lua_engine share network gateway_engine)
add_dependencies(${TARGET} build_pb)