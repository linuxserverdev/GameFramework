#pragma once
#include "lua.hpp"
#include "tolua++.h"
#include "lua_op_t.h"
#include <string>
#include <vector>
#ifdef __LUA_FUN_PERF__
#include <Share/PerfTool.h>
#endif

bool findLuaMethod(lua_State* L, const char* func_name);

struct Type_Null{};
extern Type_Null nil;

struct UserData
{
    void* pData_;
    UserData(void* pData = NULL): pData_(pData){}
};
template<class T> void PushParam(lua_State* L, T param)
{
    lua_op_t<T>::push_stack(L, param);
};
template<> inline void PushParam(lua_State* L, UserData param)
{
    lua_op_t<void*>::push_stack(L, param.pData_);
}

template<class T1 = Type_Null,class T2= Type_Null, class T3= Type_Null, class T4= Type_Null, class T5= Type_Null, class T6= Type_Null, class T7= Type_Null>
class ParamPusher
{
public:
    static void Push(lua_State* L, short& index, const T1& t1=nil, const T2& t2 = nil, const T3& t3 = nil, const T4& t4 = nil, const T5& t5 = nil, const T6& t6 = nil, const T7& t7 = nil)
    {
        PushParam(L, t1);
        ParamPusher<T2, T3, T4, T5, T6, T7>::Push(L, ++index, t2, t3, t4, t5, t6, t7);
    }
};

template<>
class ParamPusher<Type_Null, Type_Null, Type_Null, Type_Null, Type_Null, Type_Null, Type_Null>
{
public:
    static void Push(lua_State* L, short& index, const Type_Null& t1 = nil,const Type_Null& t2 = nil, const Type_Null& t3 = nil, const Type_Null& t4 = nil, const Type_Null& t5 = nil, const Type_Null& t6 = nil, const Type_Null& t7 = nil){}
};

template<class T>
inline bool IsNull(const T& obj){ return false; }
template<>
inline bool IsNull(const Type_Null& obj){ return true; }

template<class T> bool PopResult(lua_State* L, T& result)
{
    int rc = 0;
    rc = lua_op_t<T>::get_lua_value(L, -1, result);
    lua_pop(L, 1);
    return rc == 0;
}

template<> inline bool PopResult(lua_State* L, UserData& result)
{
    if (lua_isuserdata(L, -1))
    {
        result.pData_ = lua_touserdata(L, -1);
        lua_pop(L, 1);
        return true;
    }
    return false;
}

template<class RT>
class ResultPoper
{
public:
    static bool Pop(lua_State* L, RT& result)
    {
        return PopResult(L, result);
    }
};

template<class RT = Type_Null, class PT1 = Type_Null, class PT2 = Type_Null, class PT3 = Type_Null, class PT4 = Type_Null, class PT5 = Type_Null, class PT6 = Type_Null, class PT7 = Type_Null>
class LuaFunctor
{
public:
    LuaFunctor(lua_State* state, const char* func):  ls_(state), index_(LUA_NOREF), fname_(func)
    {
        strError_ = "unknown error";
    }
    ~LuaFunctor()
    {
        if (index_ != LUA_NOREF)
        {
            luaL_unref(ls_, LUA_REGISTRYINDEX, index_);
        }
    }
    bool operator()(int selfIndex = LUA_NOREF, RT& pResult = nil, const PT1& param1 = nil, const PT2& param2 = nil, const PT3& param3 = nil, const PT4& param4 = nil, const PT5& param5 = nil, const PT6& param6 = nil, const PT7& param7 = nil) const;
    const char* getLastError()
    {
        return strError_.c_str();
    }

private:
    LuaFunctor& operator=(const LuaFunctor& fun);

private:
    lua_State* ls_;
    mutable int index_;
    const char* fname_;
    mutable std::string strError_;
};

template<class RT, class PT1, class PT2, class PT3, class PT4, class PT5, class PT6, class PT7>
bool LuaFunctor<RT, PT1, PT2, PT3, PT4, PT5, PT6, PT7>::operator()(int selfIndex,  RT& result, const PT1& param1, const PT2& param2, const PT3& param3, const PT4& param4, const PT5& param5, const PT6& param6, const PT7& param7) const
{
    int top = lua_gettop(ls_);
    if (!findLuaMethod(ls_, fname_))
    {
        return false;
    }

    index_ = luaL_ref(ls_, LUA_REGISTRYINDEX);
    if (index_ == LUA_NOREF)
    {
        lua_settop(ls_, top);
        return false;
    }
    // retrieve function
    lua_rawgeti(ls_, LUA_REGISTRYINDEX, index_);
    short nargs = 0, nResult = 0;
    if (selfIndex != LUA_NOREF)
    {
        //push self
        lua_rawgeti(ls_, LUA_REGISTRYINDEX, selfIndex);
        ++nargs;
    }
    //push param
    ParamPusher<PT1, PT2, PT3, PT4, PT5, PT6, PT7>::Push(ls_, nargs, param1, param2, param3, param4, param5, param6, param7);
    if (!IsNull(result))
    {
        nResult++;
    }

#ifdef __LUA_FUN_PERF__
    {
        PerfTool perfGuard(std::string("luaFunctor-") + fname_);
#endif

        int error = lua_pcall(ls_, nargs, nResult, 0);
        if (error) {
            const char *errInfor = lua_tostring(ls_, -1);
            if (errInfor) strError_ = errInfor;
            lua_settop(ls_, top);
            return false;
        }

        if (!IsNull(result)) {
            PopResult<RT>(ls_, result);
        }
        lua_settop(ls_, top);

#ifdef __LUA_FUN_PERF__
    }
#endif

    return true;
}