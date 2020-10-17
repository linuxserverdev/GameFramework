#include "LuaEngine.h"
#include <Logger/Logger.h>
#include <Share/Utils.h>
#include <Share/FileSystem.h>

enum LuaEngineCode{
    lec_success = 0,
    lec_exception,
    lec_unkown,
    lec_no_function,
    lec_input_type,
    lec_output_type
};

const char* _luaEngineMsg[] = {
    "work well done",
    "exception encountered",
    "unknown error",
    "no such function",
    "invalid input param type",
    "invalid output param type"
};

static int luaTrace(lua_State *L)
{
    std::string out_str;
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        out_str += s;
        if (s == NULL)
            return luaL_error(L, "`tostring' must return a string to `print'");
        if (i>1) out_str += "\t";
        lua_pop(L, 1);  /* pop result */
    }
    LOG_TRACE << out_str;
    return 0;
}

static int luaInfo(lua_State *L)
{
    std::string out_str;
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        out_str += s;
        if (s == NULL)
            return luaL_error(L, "`tostring' must return a string to `print'");
        if (i>1) out_str += "\t";
        lua_pop(L, 1);  /* pop result */
    }
    LOG_INFO << out_str;
    return 0;
}

static int luaError (lua_State *L)
{
    std::string out_str;
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        out_str += s;
        if (s == NULL)
            return luaL_error(L, "`tostring' must return a string to `print'");
        if (i>1) out_str += "\t";
        lua_pop(L, 1);  /* pop result */
    }
    LOG_ERROR << out_str;
    return 0;
}

static int luaWarning(lua_State *L)
{
    std::string out_str;
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        out_str += s;
        if (s == NULL)
            return luaL_error(L, "`tostring' must return a string to `print'");
        if (i>1) out_str += "\t";
        lua_pop(L, 1);  /* pop result */
    }
    LOG_WARN << out_str;
    return 0;
}

static int luaLoadFile(lua_State *L, const char *filename)
{
    int status = -1;

    FileSystem& fs = FileSystem::getIns();

    FileStream fStream = fs.open(filename);
    if(fStream.isValid())
    {
        std::string content = fStream.readAll();
        if(!content.empty())
        {
//            char* slash = strchr((char*)filename, '/');
//            while (slash != NULL)
//            {
//                *slash = '.';
//                slash = strchr(slash, '/');
//            }
            status = luaL_loadbuffer(L, content.c_str(), content.length(), filename);
        }
    }
    return status;
}

static int luaDoFile(lua_State * L)
{
    size_t l;
    const char * sFileName = luaL_checklstring(L, 1, &l);
    if(sFileName != NULL )
    {
        luaLoadFile(L,sFileName);
        return 1;
    }

    return 0;
}

static int luaLoaderLua(lua_State *L)
{
    const char *name = luaL_gsub(L, luaL_checkstring(L, 1), ".", LUA_DIRSEP);
    char filename[255];
    //sprintf(filename, "%s.luc", name);
    //int status = luaA_LoadFile(L, filename);
    //if (status != 0) {
        snprintf(filename, sizeof(filename), "%s.lua", name);
        int status = luaLoadFile(L, filename);
    //}
    if (status != 0) {
        luaL_error(L, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s",
                lua_tostring(L, 1), name, lua_tostring(L, -1));
    }
    return 1;
}

static int luaSetLoader(lua_State *L, lua_CFunction fn)
{
    lua_getglobal(L, LUA_LOADLIBNAME);
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "loaders");
        if (lua_istable(L, -1)) {
            lua_pushcfunction(L, fn);
            lua_rawseti(L, -2, 2);
            return 0;
        }
    }
    return -1;
}


bool LuaEngine::create(bool bFromPackLoadLua)
{
    fromPackLoadLua_ = bFromPackLoadLua;
    luaState_ = lua_open();
    if(luaState_ == NULL)
    {
        return false;
    }

    luaL_openlibs(luaState_);
    luaSetLoader(luaState_, luaLoaderLua);
    lua_checkstack(luaState_, 4096);

    lua_register(luaState_, "dofile", luaDoFile);
    lua_register(luaState_, "print", luaInfo);
    lua_register(luaState_, "trace", luaTrace);
    lua_register(luaState_, "error", luaError);
    lua_register(luaState_, "warning", luaWarning);
    return true;
}

bool LuaEngine::loadLuaFile(const std::string& luaFileName)
{
    if(luaFileName.empty()) return false;

    int top = lua_gettop(luaState_);
    int nResult = 0;
    try{
        nResult = fromPackLoadLua_ ?
                luaLoadFile(luaState_, luaFileName.c_str())
                :luaL_loadfile(luaState_, luaFileName.c_str());
        if(nResult == 0)
        {
            nResult = lua_pcall(luaState_, 0, 0, 0);
        }
        //If something goes wrong, try to find the reason
        if(nResult !=0 )
        {
            const char* errInfo = lua_tostring(luaState_, -1);
            lastErr_ = errInfo ? errInfo : _luaEngineMsg[lec_unkown];
        }
    }catch(...){
        lastErr_ = _luaEngineMsg[nResult = lec_exception];
    }
    //whatever the case, try to restore the stack top
    lua_settop(luaState_, top);
    if(nResult !=0 )
    {
        LOG_ERROR << utils::formattedString("[lua_engine] load and execute file %s failed, because of %s",
                luaFileName.c_str(), lastErr_.c_str());
    }
    return (nResult == 0);
}

std::string LuaEngine::dumpStack()
{
    std::string ret;
    int i;
    int top = lua_gettop(luaState_);

    for (i = 1; i <= top; i++)
    {
        int t = lua_type(luaState_, i);
        switch (t)
        {
            case LUA_TSTRING:
            {
                ret.append(utils::formattedString("`%s'", lua_tostring(luaState_, i)));
            }
                break;
            case LUA_TBOOLEAN:
            {
                ret.append(lua_toboolean(luaState_, i) ? "true" : "false");
            }
                break;
            case LUA_TNUMBER:
            {
                ret.append(utils::formattedString("`%g`", lua_tonumber(luaState_, i)));
            }
                break;
            case LUA_TTABLE:
            {
                ret.append("table begin\n");
                lua_pushnil(luaState_);
                while (lua_next(luaState_, i) != 0) {
                    ret.append(utils::formattedString("	%s - %s\n",
                           lua_typename(luaState_, lua_type(luaState_, -2)),
                           lua_typename(luaState_, lua_type(luaState_, -1))));
                    lua_pop(luaState_, 1);
                }
                ret.append("table end");
            }
                break;
            default:
            {
                ret.append(utils::formattedString("`%s`", lua_typename(luaState_, t)));
            }
                break;
        }
        ret.append(" ");
    }
    ret.append("\n");
    return ret;
}
std::string LuaEngine::dumpError(const char *fmt, ...)
{
    std::string ret;
    char buff[1024];

    va_list argp;
    va_start(argp, fmt);
    vsnprintf(buff, sizeof(buff), fmt, argp);
    va_end(argp);

    ret = buff;
    snprintf(buff, sizeof(buff), " tracback:%s", lua_tostring(luaState_, -1));
    ret += buff;

    return ret;
}