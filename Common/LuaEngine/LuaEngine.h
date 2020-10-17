#pragma once
#include "lua_op_t.h"
#include "lua.hpp"
#include "tolua++.h"
#include <string>

class LuaEngine
{
public:
    template<typename T>
    bool getGlobalVariable(const std::string& field_name_, T& ret)
    {
        return getGlobalVariable<T>(field_name_.c_str(), ret);
    }

    template<typename T>
    bool getGlobalVariable(const char* fieldName, T& ret)
    {
        int rc = 0;

        lua_getglobal(luaState_, fieldName);
        rc = lua_op_t<T>::get_lua_value(luaState_, -1, ret);

        lua_pop(luaState_, 1);
        return rc == 0;
    }

    bool create(bool bFromPackLoadLua = true);
    bool loadLuaFile(const std::string& luaFileName);
    std::string dumpStack();
    std::string dumpError(const char *fmt, ...);
    void openLuaByteBuffer()
    {
        int tolua_export_lua_byte_buffer_open (lua_State* tolua_S);
        if (luaState_)
        {
            tolua_export_lua_byte_buffer_open(luaState_);
        }
    }
    bool isValid()
    {
        return luaState_ != NULL;
    }
    lua_State* getLuaState()
    {
        return luaState_;
    }
    const std::string& getLastError()
    {
        return lastErr_;
    }
    ~LuaEngine()
    {
        if(luaState_ != NULL)
        {
            lua_close(luaState_);
        }
    }
private:
    bool fromPackLoadLua_{true};
    lua_State* luaState_{NULL};
    std::string lastErr_{""};
};